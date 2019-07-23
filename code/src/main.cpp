#include <iostream>
#include <opencv2/opencv.hpp>
#include "Diagram.h"
#include "TikzGenerator.h"
#include "ShapeRecognition.h"
#include "preprocessing.h"
#include "NodeShape.h"
#include "NodePoint.h"
#include "Edge.h"
#include "Shapes.h"
#include "utils.h"

int main (int argc, char** argv)
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    /////// PARAMETER PROCESSING //////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    //Flag for outputting \tikzpicture environment
    bool TIKZ_ENV_FLAG = false;
    //Flag for outputting LaTeX document Frame
    bool TEX_DOC_FLAG = false;
    //Flag for outputting TikZ cosmetic variable settings
    bool COSMETICS_FLAG = false;
    //Flag for labelling shapes in tikzpicture with their identifiers
    bool LABEL_FLAG = false;
    //Flag for writing command line parameter values to config file
    bool SET_DEFAULT_PARAMS = false;
    //Flag for displaying images for visual debugging
    bool DISPLAY_IMAGES_FLAG = false;
    //Alignment mode parameter
    Alignments ALIGNMENT_MODE = DEFAULT_ALIGNMENT;
    //default or user-set grid size
    std::pair<float,float> GRID_SIZE(0,0);
    //maximum distance between two line segment endpoints to be merged into a corner or intersection
    double CORNER_MERGE_THRESHOLD = 20;
    //minimum number of hough line votes for an edge to be valid
    int LINE_SUPPORT_THRESHOLD = 1;
    //the original input image
    cv::Mat img;
    
    //import default settings from config file
    readConfigFile(TIKZ_ENV_FLAG , TEX_DOC_FLAG , COSMETICS_FLAG , LABEL_FLAG , DISPLAY_IMAGES_FLAG , ALIGNMENT_MODE , GRID_SIZE , CORNER_MERGE_THRESHOLD , LINE_SUPPORT_THRESHOLD);

    //overwrite settings made via command line options and exit with failure if those are invalid
    if(!processCLArguments(argc,argv, img, TIKZ_ENV_FLAG , TEX_DOC_FLAG , COSMETICS_FLAG, LABEL_FLAG , DISPLAY_IMAGES_FLAG , SET_DEFAULT_PARAMS , ALIGNMENT_MODE , GRID_SIZE , CORNER_MERGE_THRESHOLD , LINE_SUPPORT_THRESHOLD))
    { return -1;}

    //write settings to config file if command line flag was set
    if(SET_DEFAULT_PARAMS)
    {
        writeConfigFile(TIKZ_ENV_FLAG , TEX_DOC_FLAG , COSMETICS_FLAG , LABEL_FLAG , DISPLAY_IMAGES_FLAG,  ALIGNMENT_MODE , GRID_SIZE , CORNER_MERGE_THRESHOLD , LINE_SUPPORT_THRESHOLD);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    /////// IMAGE PREPROCESSING ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    //input image converted to grayscale (0-255)
    cv::Mat imgGrayScale;
    //inverted grayscale image
    cv::Mat imgGrayscaleInv;
    //binarized input image
    cv::Mat imgBinary;
    //inverted binarized input image
    cv::Mat imgBinaryInv;
    //image used for visual output
    cv::Mat imgOutput;

    //convert input image to all needed versions
    processImg(img,imgGrayScale,imgGrayscaleInv,imgBinary,imgBinaryInv);
    cv::cvtColor(imgGrayScale,imgOutput,cv::COLOR_GRAY2BGR);

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    /////// SHAPE EXTRACTION //////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    /////// POLYGONS //////////////////////////////////////
    ///////////////////////////////////////////////////////
    
    //binary image containing only the insides of closed shapes as foreground
    cv::Mat imgFilled;
    //vector of polygonal shapes detected in the input image
    std::vector<std::vector<cv::Point2d>> shapes;
    //vector of circles detected in the input image
    std::vector<cv::Vec3f> circles;

    //find polygons and circles and store them in the corresponding vectors
    computeShapes(imgBinary,imgFilled,shapes,circles);

    if(DISPLAY_IMAGES_FLAG)
    {
        displayImg("befor filling",imgBinary);
        displayImg("filled",imgFilled);
    }
    if(DISPLAY_IMAGES_FLAG)
    {//display image with found polygons and circles
        cv::Mat tempImg;
        imgOutput.copyTo(tempImg);
        for(const std::vector<cv::Point2d>& shape :shapes)
        {
            for(unsigned int i = 1; i <= shape.size(); i++)
            {
                cv::line(tempImg,shape[i-1],shape[i%shape.size()],cv::Scalar(0,255,0),4);
            }
        }
        for(const cv::Vec3f& circ : circles)
        {
            cv::circle(tempImg,cv::Point2f(circ[0],circ[1]),circ[2],cv::Scalar(0,255,0),4);
        }
        displayImg("Shapes",tempImg);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    /////// EDGE DETECTION ////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////


    /////// ISOLATE LINES /////////////////////////////////
    ///////////////////////////////////////////////////////

    //binary image containing only the insides of shapes as background
    cv::Mat imgFilledInv;
    //binary image containing only the drawn lines sans shape outlines as foreground
    cv::Mat imgBinaryLines;

    //"erase" shape outlines from binary image
    connectorImage(imgFilled,imgBinaryInv,imgBinaryLines);
    if(DISPLAY_IMAGES_FLAG)
    {
        displayImg("lines only",imgBinaryLines);
    }
    /////// DETECT INTERSECTS AND CORNERS /////////////////
    ///////////////////////////////////////////////////////

    //vector containing the positions of all detected corners from the binary line image
    std::vector<cv::Point2d> corners;
    //vector containing the number of supporting hough-lines for each undirected connection of two corners
    std::vector<double> support;
    //vector containing all above-threshold corner connections (line segments)
    std::vector<cv::Vec4d> edges;
    
    //get corners on binary line image and store them in a vector
    findCorners(imgBinaryLines,corners);

    /////// CONSTRUCT EDGES ///////////////////////////////
    ///////////////////////////////////////////////////////

    { 
        //temporary vector containing all line segments that connect two detected corners
        std::vector<cv::Vec4d> edgeCandidates;
        //generate all unoredered pairs of corners and construct the line segment between them
        generateEdges(corners,edgeCandidates);

        //temporary vector for storing detected hough-lines during edge detection 
        std::vector<cv::Vec4d> lines;
        //temporary vector for storing all support counts for all edge candidates
        std::vector<double> l_support;

        //search for hough-lines in the binary line image
        cv::HoughLinesP(imgBinaryLines,lines, 1,CV_PI/180,15,50,10);

        //use Nacken's metric to have hough-lines vote for their most similar edge candidate
        computeEdgeSupport(lines, edgeCandidates, l_support);

        //push above-threshold edges and their support values to the persistent vectors
        for(unsigned int i = 0; i < edgeCandidates.size(); i++)
        {
            if(l_support[i] > 0)
            {
                edges.push_back(edgeCandidates[i]);
                support.push_back(l_support[i]);
            }
        }
    }

    if(DISPLAY_IMAGES_FLAG)
    {//Display input image with detected edges
        cv::Mat tempImg;
        imgOutput.copyTo(tempImg);
        for (unsigned int i=0; i < edges.size();i++)
        {
            if(support[i]>=LINE_SUPPORT_THRESHOLD)
            {
                cv::line(tempImg,cv::Point(edges[i][0],edges[i][1]),cv::Point(edges[i][2],edges[i][3]),cv::Scalar(0,0,255),4);
            }
        }
        
        displayImg("Shapes, Circles and Lines", tempImg);
    }


    //vector containing all edges of the graph
    std::vector<std::shared_ptr<Edge>> graphEdges;
    //vector containing all nodes of the graph that wrap detected shapes
    std::vector<std::shared_ptr<NodeShape>> graphNodes;
    //mappping of shape types onto vectors of corresponding ShapeNodes
    std::unordered_map<std::type_index,std::vector<std::shared_ptr<NodeShape>>> graphNodesMap;
    
    //construct empty Graph edges from line segments
    for(const cv::Vec4d& e : edges)
    {   
        std::shared_ptr<Edge> ep = std::make_shared<Edge>(e,std::pair<std::optional<std::shared_ptr<Node>>,std::optional<std::shared_ptr<Node>>>()); 
        graphEdges.push_back(ep);
    }

    //vector containing the Diagram-compatible shape representations to be wrapped in graph nodes
    std::vector<std::shared_ptr<Shape>> shapeObjects;

    //numer of non-quadrilateral polygons for the construction of numbered identifiers
    int polys = 0;
    //numer of axis-parallel for the construction of numbered identifiers
    int rects = 0;
    //numer of rotated rectangles for the construction of numbered identifiers
    int diamonds = 0;
    //numer of circles for the construction of numbered identifiers
    int circs = 0;
    //Diagram to be passed to the TikZ-generator
    Diagram diagram;

    
    for(const std::vector<cv::Point2d>& shape : shapes)
    {   
        //compute Centroid of polygon
        cv::Point2d centroid(0,0);
        for(const cv::Point2d& pt : shape)
        {   
            centroid+=pt;
        }
        centroid/=(int)shape.size();

        //vector of shape vertices converted to float-vectors for compatibility reasons
        std::vector<cv::Point2f> floatPoints;
        for (const cv::Point2d& pt : shape)
        {
            floatPoints.push_back(pt);
        }

        //axis-parallel bounding rectangle of the vertices of the current shape 
        cv::Rect2d axisParallelBoundingRect = cv::boundingRect(floatPoints);

        //construct rectangle shapes, wrap them in NodeShapes and insert those into the diagram
        if(shape.size() == 4)
        {
            cv::RotatedRect r = cv::minAreaRect(floatPoints);

            Rectangle gutesRect;
            if(std::atan(std::abs(r.angle*0.017453293))<std::atan(std::abs(68*0.017453293))&& std::atan(std::abs(r.angle*0.017453293))>std::atan(std::abs(23*0.017453293)))
            {
                gutesRect = Rectangle(axisParallelBoundingRect.width,axisParallelBoundingRect.height,true,"D-"+std::to_string(diamonds),centroid.x,centroid.y);
                diamonds++;
            }
            else
            {
                gutesRect = Rectangle(axisParallelBoundingRect.width,axisParallelBoundingRect.height,false,"R-"+std::to_string(rects),centroid.x,centroid.y);
                rects++;
            }
            std::shared_ptr<NodeShape> node = std::make_shared<NodeShape>(cv::Point2d(gutesRect.getRootCoordX(),gutesRect.getRootCoordY()),gutesRect,gutesRect.getIdentifier());
            node->setInnerRad(innerRad(shape,centroid));
            node->setOuterRad(outerRad(shape,centroid));
            graphNodes.push_back(node);
            graphNodesMap[typeid(Rectangle)].push_back(node);

            gutesRect.setMinWidth(gutesRect.getMinWidth()/100);
            gutesRect.setMinHeight(gutesRect.getMinHeight()/100);
            gutesRect.setRootCoordX(gutesRect.getRootCoordX()/100);
            gutesRect.setRootCoordY(gutesRect.getRootCoordY()/-100);
            gutesRect.setLabel(LABEL_FLAG?gutesRect.getIdentifier():"");
            diagram.insertNode(std::make_shared<Rectangle>(gutesRect));

        }

        //construct non-rectangular shapes, wrap them in NodeShapes and insert those into the diagram
        else
        {
            Polygon poly(std::min(axisParallelBoundingRect.height,axisParallelBoundingRect.width),shape.size(),"P-"+std::to_string(polys),centroid.x,centroid.y);

            std::shared_ptr<NodeShape> node = std::make_shared<NodeShape>(cv::Point2d(poly.getRootCoordX(),poly.getRootCoordY()),poly,poly.getIdentifier());
            node->setInnerRad(innerRad(shape,centroid));
            node->setOuterRad(outerRad(shape,centroid));
            graphNodes.push_back(node);
            graphNodesMap[typeid(Polygon)].push_back(node);

            poly.setMinSize(poly.getMinSize()/100);
            poly.setRootCoordX(poly.getRootCoordX()/100);
            poly.setRootCoordY(poly.getRootCoordY()/-100);
            poly.setLabel(LABEL_FLAG?poly.getIdentifier():"");

            diagram.insertNode(std::make_shared<Polygon>(poly));

            polys++;
        }
    }

    //construct circular shapes, wrap them in NodeShapes and insert those into the diagram
    for(const cv::Vec3f& circ: circles)
    {
        Circle c(circ[2],"C-"+std::to_string(circs),circ[0],circ[1]);

        std::shared_ptr<NodeShape> node = std::make_shared<NodeShape>(cv::Point2d(c.getRootCoordX(),c.getRootCoordY()),c,c.getIdentifier());
        node->setInnerRad(innerRad(circ));
        node->setOuterRad(outerRad(circ));
        graphNodes.push_back(node);
        graphNodesMap[typeid(Circle)].push_back(node);

        c.setMinSize(2*c.getMinSize()/100);
        c.setRootCoordX(c.getRootCoordX()/100);
        c.setRootCoordY(c.getRootCoordY()/-100);
        c.setLabel(LABEL_FLAG?c.getIdentifier():"");

        diagram.insertNode(std::make_shared<Circle>(c));

        circs++;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    /////// GRAPH CONSTRUCTION ////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    /////// CONNECT SHAPES AND EDGES //////////////////////
    ///////////////////////////////////////////////////////

    //associate the NodeShapes with their incident edges
    for (const auto it : graphNodesMap)
    {
        for (const auto currentNode : it.second)
        {
            currentNode->connectIncidentEdges(graphEdges);
        }
    }

    /////// CONSTRUCT CORNERS AND INTERSECTIONS ///////////
    ///////////////////////////////////////////////////////

    //vector that contains all corners and intersections
    std::vector<std::shared_ptr<NodePoint>> nodePts;

    //TODO: REWRITE AND WRAP THE FOLLOWING IN A FUNCTION
    /*Here all edge-embedding endpoints, which are not associated with a NodeShape, are compared and merged into
    corner- and intersection-representing NodePoints.
    All endpoints are first compared to existing corners and then to other loose endpoints.*/
    for(std::shared_ptr<Edge>& edge : graphEdges)
    {
        //leftmost endpoint of the current line segment
        cv::Point2d firstEndPoint(edge->getLine()[0],edge->getLine()[1]);
        //rightmost endpoint of the current line segment
        cv::Point2d secondEndPoint(edge->getLine()[2],edge->getLine()[3]);
        
        //Check first endpoint
        if(!edge->getFirstNode())
        {   
            //the endpoint starts out unassigned
            bool assigned = false;
            //we first search for an existing Node in the vicinity
            for(std::shared_ptr<NodePoint>& node : nodePts)
            {
                //if one is found, it becomes the first node of the current edge
                if(twoPointDist(node->getPosition(),firstEndPoint)<CORNER_MERGE_THRESHOLD)
                {
                    edge->setFirstNode(node);
                    node->addEdge(Position::first,edge);
                    assigned = true;
                }
            }
            //if no node was found, close unassigned endpoints of other edges are searched
            if(!assigned)
            {
                //iterate over all edges
                for(const std::shared_ptr<Edge>& incEdge : graphEdges)
                {
                    //if incEdge is not the current edge
                    if (incEdge != edge)
                    {   
                        //if first node of incEdge is unset
                        if(!incEdge->getFirstNode())
                        {
                            //check for proximity
                            cv::Point2d incPoint(incEdge->getLine()[0],incEdge->getLine()[1]);
                            if(twoPointDist(incPoint,firstEndPoint)<CORNER_MERGE_THRESHOLD)
                            {
                                //construct new point Node if endpoints are close enough
                                std::shared_ptr<NodePoint> node = std::make_shared<NodePoint>(firstEndPoint+((incPoint-firstEndPoint)/2.));
                                node->addEdge(Position::first,edge);
                                edge->setFirstNode(node);
                                nodePts.push_back(node);
                                break;
                            }
                        }
                        //alternatively check second node of incEdge
                        else if(!incEdge->getSecondNode())
                        {
                            cv::Point2d incPoint(incEdge->getLine()[2],incEdge->getLine()[3]);
                            if(twoPointDist(incPoint,firstEndPoint)<CORNER_MERGE_THRESHOLD)
                            {
                                std::shared_ptr<NodePoint> node = std::make_shared<NodePoint>(firstEndPoint+((incPoint-firstEndPoint)/2.));
                                node->addEdge(Position::first,edge);
                                edge->setFirstNode(node);
                                nodePts.push_back(node);
                                break;
                            }
                        }
                    }
                }
            }
        }
            
        //Same thing for the second endpoint
        if(!edge->getSecondNode())
        {
            bool assigned = false;
            for(std::shared_ptr<NodePoint>& node : nodePts)
            {
                if(twoPointDist(node->getPosition(),secondEndPoint)<CORNER_MERGE_THRESHOLD)
                {
                    edge->setSecondNode(node);
                    node->addEdge(Position::second,edge);
                    assigned = true;
                }
            }
           
            if(!assigned)
            {
                for(const std::shared_ptr<Edge>& incEdge : graphEdges)
                {
                    //if incEdge is not the current edge
                    if (incEdge != edge)
                    {
                        if(!incEdge->getFirstNode())
                        {
                            cv::Point2d incPoint(incEdge->getLine()[0],incEdge->getLine()[1]);
                            if(twoPointDist(incPoint,secondEndPoint)<CORNER_MERGE_THRESHOLD)
                            {
                                std::shared_ptr<NodePoint> node = std::make_shared<NodePoint>(secondEndPoint+((incPoint-secondEndPoint)/2.));
                                node->addEdge(Position::second,edge);
                                edge->setSecondNode(node);
                                nodePts.push_back(node);
                                break;
                            }
                        }
                        else if(!incEdge->getSecondNode())
                        {
                            cv::Point2d incPoint(incEdge->getLine()[2],incEdge->getLine()[3]);
                            if(twoPointDist(incPoint,secondEndPoint)<CORNER_MERGE_THRESHOLD)
                            {
                                std::shared_ptr<NodePoint> node = std::make_shared<NodePoint>(secondEndPoint+((incPoint-secondEndPoint)/2.));
                                node->addEdge(Position::second,edge);
                                edge->setSecondNode(node);
                                nodePts.push_back(node);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    if(DISPLAY_IMAGES_FLAG)
    {//display input image with detected corners
        cv::Mat tempImg;
        imgOutput.copyTo(tempImg);
        for(const auto n : nodePts)
        {
            cv::circle(tempImg,n->getPosition(),5,cv::Scalar(0,255,0),4);
        }    
        displayImg("corners", tempImg);
    }

    //display input image with all edges with marked left and right edpoints
    if(DISPLAY_IMAGES_FLAG)
    {
        for(const auto e : graphEdges)
        {
            cv::Mat tempImg;
            imgOutput.copyTo(tempImg);
            cv::line(tempImg,cv::Point2d(e->getLine()[0],e->getLine()[1]),cv::Point2d(e->getLine()[2],e->getLine()[3]),cv::Scalar(0,0,255),4);
            cv::circle(tempImg,e->getFirstNode()->get()->getPosition(),5,cv::Scalar(0,255,0),4);
            cv::circle(tempImg,e->getSecondNode()->get()->getPosition(),5,cv::Scalar(255,0,0),4);

            displayImg("corners", tempImg);
        }
    }
    /////// CONSTRUCT CONNECTIONS BETWEEN SHAPES //////////
    ///////////////////////////////////////////////////////
    
    //vector of all detected connections between shapes
    std::vector<Connection> connections;

    //perform DFS on the graph to find connections between shapes and store them in the corresponding vector 
    linkShapes(graphNodesMap,connections);

    //insert detected connections into the diagram
    for(const Connection& con: connections)
    {
        diagram.insertConnection(std::make_shared<Connection>(con));
    }

    //the TikZ-generator that will produce the output file
    TikzGenerator gen;

    //TODO: FIND MORE ELEGANT WAY FOR THIS
    //construct appropriate alignment object and generate output file
    switch(ALIGNMENT_MODE)
    {
        case DEFAULT_ALIGNMENT:
            {
                DefaultAlign alignmentOptionDefault;
                gen.generateEasyTikZ(diagram, &alignmentOptionDefault,TIKZ_ENV_FLAG,TEX_DOC_FLAG, COSMETICS_FLAG);
            }
            break;
        case SIZE_ALIGNMENT:
            {
                SizeAlign alignmentOptionDefault;
                gen.generateEasyTikZ(diagram, &alignmentOptionDefault,TIKZ_ENV_FLAG,TEX_DOC_FLAG, COSMETICS_FLAG);
            }
            break;
        case MANUAL_ALIGNMENT:
            {
                ManualAlign alignmentOptionManual;
                gen.generateEasyTikZ(diagram, &alignmentOptionManual,TIKZ_ENV_FLAG,TEX_DOC_FLAG, COSMETICS_FLAG, GRID_SIZE.first, GRID_SIZE.second);
            }
            break;
        default:
            return -1;
    }
    return 0;
}
