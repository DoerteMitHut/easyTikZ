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
void displayImg(std::string window_name, cv::Mat img){
    cv::namedWindow(window_name,cv::WINDOW_NORMAL);
    cv::resizeWindow(window_name, 800,800);
    cv::imshow(window_name,img);
    cv::waitKey(0); //wait for a key press
    cv::destroyWindow(window_name);
}

int main (int argc, char** argv)
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    /////// PARAMETER PROCESSING //////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    //check whether there is a command line argument given
    if (argc <2 || argc >4){
		std::cout<<"u   sage: main <Image_Path> [--tikz [--tex]]\n";
		return -1;
	}
    //if there is a command line argument, use it to set the path for the source image
	cv::Mat img =  cv::imread(argv[1]);
	if(img.empty())
	{
		std::cout<<"No image data!\n";
		return -1;
    }
    ///////////// flags to include LaTeX document frame and tikz environment in the output file
    //Flag to envelop output in tikzpicture environment
    bool TIKZ_ENV_FLAG = false;
    //Flag to envelop tikzpicture in Latex Document
    bool TEX_DOC_FLAG = false;
    if(argc >= 3)
    {
        if(std::strcmp(argv[2],"--tikz")==0)
        {
            TIKZ_ENV_FLAG = true;
            
            if(argc==4 && std::strcmp(argv[3],"--tex")==0)
            {
                TEX_DOC_FLAG = true;
            }
            else
            {
                TEX_DOC_FLAG = false;
            }      
        }
        else
        {
            TIKZ_ENV_FLAG = false;
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    /////// IMAGE PREPROCESSING ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    //declaring all needed images
    cv::Mat imgGrayScale, imgGrayscaleInv, imgBinary, imgBinaryInv, imgOutput;

    //converting the original image into grayscale
    processImg(img,imgGrayScale,imgGrayscaleInv,imgBinary,imgBinaryInv);
    cv::cvtColor(imgGrayScale,imgOutput,cv::COLOR_GRAY2BGR);

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    /////// SHAPE EXTRACTION //////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    /////// POLYGONS //////////////////////////////////////
    ///////////////////////////////////////////////////////
    cv::Mat imgFilled;
    std::vector<std::vector<cv::Point2d>> shapes;

    displayImg("befor filling",imgBinary);
    fillShapes(imgBinary,imgFilled);
    displayImg("filled",imgFilled);
    findShapes(imgFilled,shapes);
    {
        cv::Mat tempImg;
        imgOutput.copyTo(tempImg);
        for(const std::vector<cv::Point2d>& shape :shapes)
        {
            for(unsigned int i = 1; i <= shape.size(); i++)
            {
                cv::line(tempImg,shape[i-1],shape[i%shape.size()],cv::Scalar(0,255,0),4);
            }
        }
        displayImg("Shapes",tempImg);
    }
    /////// CIRCLES ///////////////////////////////////////
    ///////////////////////////////////////////////////////

    // apply hough circles to find circles and draw a cirlce around it
    std::vector<cv::Vec3f> circles;

    {
        cv::Mat gaussian;
        GaussianBlur( imgFilled, gaussian, cv::Size(9, 9), 2, 2 );
        HoughCircles(imgFilled, circles, CV_HOUGH_GRADIENT, 1, gaussian.rows/8, 200,18);
        std::cout <<" | FOUND " << circles.size() << "circles"<<std::endl;
    }
    {
        cv::Mat tempImg;
        imgOutput.copyTo(tempImg);
        for(const cv::Vec3f& circ : circles)
        {
            cv::circle(tempImg,cv::Point2f(circ[0],circ[1]),circ[2],cv::Scalar(0,255,0),4);
        }
        displayImg("Shapes and Circles",tempImg);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    /////// EDGE DETECTION ////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    double pointDistThresh = 20;

    /////// ISOLATE LINES /////////////////////////////////
    ///////////////////////////////////////////////////////

    cv::Mat imgFilledInv,imgBinaryLines;
    connectorImage(imgFilled,imgBinaryInv,imgBinaryLines);
    displayImg("lines only",imgBinaryLines);

    /////// DETECT INTERSECTS AND CORNERS /////////////////
    ///////////////////////////////////////////////////////

    std::vector<cv::Point2d> corners;
    std::vector<double> support;
    std::vector<cv::Vec4d> edges;
    
    findCorners(imgBinaryLines,corners);

    /////// CONSTRUCT EDGES ///////////////////////////////
    ///////////////////////////////////////////////////////

    {//begin scope of all volatile variables to be needed in edge detection 
        std::vector<cv::Vec4d> edgeCandidates;
        std::cout<<"Points found: "<<corners.size()<<std::endl;
        generateEdges(corners,edgeCandidates);

        //find hough lines in line image
        std::vector<cv::Vec4d> lines;
        std::vector<double> l_support;

        //search for Houghlines
        cv::HoughLinesP(imgBinaryLines,lines, 1,CV_PI/180,15,50,10);

        //**************************************************************************
        ////////////////////////////////////////////////////////////////////////////
        std::cout<<"found the following hough-lines:\n===========================\n";
        for(const cv::Vec4d& line : lines)
        {
            std::cout<<"("<<line[0]<<"|"<<line[1]<<")--("<<line[2]<<"|"<<line[3]<<")\n"<<std::endl;
        }
        std::cout<<"detected "<<lines.size()<<" lines"<<std::endl;
        ////////////////////////////////////////////////////////////////////////////
        //**************************************************************************
        
        computeEdgeSupport(lines, edgeCandidates, l_support);
        std::cout<< edgeCandidates.size()<<"|"<<l_support.size()<<"|"<<support.size()<<std::endl; 
        for(unsigned int i = 0; i < edgeCandidates.size(); i++)
        {
            std::cout<<"("<<edgeCandidates[i][0]<<"|"<<edgeCandidates[i][1]<<")--("<<edgeCandidates[i][2]<<"|"<<edgeCandidates[i][3]<<")"<<"\t: "<<(l_support[i]>0?std::to_string(l_support[i]):"--")<<std::endl;
            if(l_support[i] > 0)
            {
                edges.push_back(edgeCandidates[i]);
                support.push_back(l_support[i]);
            }
        }
        std::cout<<"TEST"<<std::endl;

        ///////////////////////////////////////////////////////////////////
    }//end edge scope

    double thresh = 0;//otsu(support);
    std::cout<<"threshold is: "<<thresh<<std::endl;
    {
        cv::Mat tempImg;
        imgOutput.copyTo(tempImg);
        for (unsigned int i=0; i < edges.size();i++)
        {
            if(support[i]>=thresh)
            {
                cv::line(tempImg,cv::Point(edges[i][0],edges[i][1]),cv::Point(edges[i][2],edges[i][3]),cv::Scalar(0,0,255),4);
            }
        }
        
        displayImg("Shapes, Circles and Lines", tempImg);
    }


    // construct Edge structs from detected edges
    std::vector<std::shared_ptr<Edge>> graphEdges;
    std::vector<std::shared_ptr<NodeShape>> graphNodes; //■■■■■■■■■■■■■■■■■■■ OLD graphNodes; check me, daddy! ■■■■■■■■■■■■■■■■■■■
    std::unordered_map<std::type_index,std::vector<std::shared_ptr<NodeShape>>> graphNodesMap; //maybe use other type for key?
    //For all detected line segments...
    for(const cv::Vec4d& e : edges)
    {   
        //...add shared Pointer to corresponding Edge object
        std::shared_ptr<Edge> ep = std::make_shared<Edge>(e,std::pair<std::optional<std::shared_ptr<Node>>,std::optional<std::shared_ptr<Node>>>()); 
        graphEdges.push_back(ep);
    }

    std::vector<std::shared_ptr<Shape>> shapeObjects;
    //Construct Shape Derivatives from polygonal shapes
    int polys = 0;
    int rects = 0;
    for(const std::vector<cv::Point2d>& shape : shapes)
    {   
        //compute Centroid of polygon
        cv::Point2d centroid(0,0);
        for(const cv::Point2d& pt : shape)
        {   
            centroid+=pt;
        }
        centroid/=(int)shape.size();

        //find bounding box of shape
        std::vector<cv::Point2f> ooorg;
        for (const cv::Point2d& pt : shape)
        {
            ooorg.push_back(pt);
        }

        cv::Rect2d axisParallelBoundingRect = cv::boundingRect(ooorg);


        //Polygons have between 3 and maxPolySides Sides.
        //Rectangles
        if(shape.size() == 4)
        {
            cv::RotatedRect r = cv::minAreaRect(ooorg);
            std::cout<<"DID MIN AREA THING"<<std::endl;

            std::shared_ptr<Rectangle> gutesRect;
            if(std::atan(std::abs(r.angle*0.017453293))<std::atan(std::abs(68*0.017453293))&& std::atan(std::abs(r.angle*0.017453293))>std::atan(std::abs(23*0.017453293)))
            {
                gutesRect = std::make_shared<Rectangle>(axisParallelBoundingRect.width,axisParallelBoundingRect.height,true,"Shape-"+std::to_string(rects),centroid.x,centroid.y);
            }
            else
            {
                gutesRect = std::make_shared<Rectangle>(axisParallelBoundingRect.width,axisParallelBoundingRect.height,false,"Shape-"+std::to_string(rects),centroid.x,centroid.y);
            }
            std::shared_ptr<NodeShape> node = std::make_shared<NodeShape>(cv::Point2d(gutesRect->getRootCoordX(),gutesRect->getRootCoordY()),*gutesRect,gutesRect->getIdentifier());
            node->setInnerRad(innerRad(shape,centroid));
            node->setOuterRad(outerRad(shape,centroid));
            graphNodes.push_back(node); //■■■■■■■■■■■■■■■■■■■ OLD graphNodes; check me, daddy! ■■■■■■■■■■■■■■■■■■■
            graphNodesMap[typeid(Rectangle)].push_back(node);

            rects++;
        }
        //Triangles and n>4-gons
        else
        {
            std::shared_ptr<Polygon> poly(std::make_shared<Polygon>(std::min(axisParallelBoundingRect.height,axisParallelBoundingRect.width),shape.size(),"Poly-"+std::to_string(polys),centroid.x,centroid.y));

            std::shared_ptr<NodeShape> node = std::make_shared<NodeShape>(cv::Point2d(poly->getRootCoordX(),poly->getRootCoordY()),*poly,poly->getIdentifier());
            node->setInnerRad(innerRad(shape,centroid));
            node->setOuterRad(outerRad(shape,centroid));
            graphNodes.push_back(node); //■■■■■■■■■■■■■■■■■■■ OLD graphNodes; check me, daddy! ■■■■■■■■■■■■■■■■■■■
            graphNodesMap[typeid(Polygon)].push_back(node);

            polys++;
        }
    }

    std::cout<<"REACHED CIRCLES"<<std::endl;

    //Construct Shape Objects from circles
    int circs = 0;
    for(const cv::Vec3f& circ: circles)
    {
        std::shared_ptr<Circle> c = std::make_shared<Circle>(circ[2],"Circ-"+std::to_string(circs),circ[0],circ[1]);

        std::shared_ptr<NodeShape> node = std::make_shared<NodeShape>(cv::Point2d(c->getRootCoordX(),c->getRootCoordY()),*c,c->getIdentifier());
        node->setInnerRad(innerRad(circ));
        node->setOuterRad(outerRad(circ));
        graphNodes.push_back(node); //■■■■■■■■■■■■■■■■■■■ OLD graphNodes; check me, daddy! ■■■■■■■■■■■■■■■■■■■
        graphNodesMap[typeid(Circle)].push_back(node);
        circs++;
    }
    std::cout<<"FINISHED CIRCLES"<<std::endl;
    //construct Nodes from shapes and associate them with their incident edges
    //for(std::shared_ptr<Node> node : graphNodes) ■■■■■■■■■■■■■■■■■■■ OLD graphNodes; check me, daddy! ■■■■■■■■■■■■■■■■■■■
    for (const auto it : graphNodesMap)
    {
        for (const auto currentNode : it.second)
        {   
            //hand the constructed edges to the node so it can figure out with which of them to connect itself 
            std::cout<<"before:"<<currentNode->getIncidentEdges().size()<<std::endl;
            currentNode->connectIncidentEdges(graphEdges);
            std::cout<<"after:"<<currentNode->getIncidentEdges().size()<<std::endl;
        }
    }

    std::cout<<"FINISHED EDGES"<<std::endl;

    std::vector<std::shared_ptr<NodePoint>> nodePts;
    //construct NodePoint objects where unassociated endpoints of edges are close enough
    double cornerThreshold = 10;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    for(std::shared_ptr<Edge>& edge : graphEdges)
    {
        cv::Point2d firstEndPoint(edge->getLine()[0],edge->getLine()[1]);
        cv::Point2d secondEndPoint(edge->getLine()[2],edge->getLine()[3]);
        //if the first node is not set
        if(!edge->getFirstNode())
        {   
            //the endpoint starts unassigned
            bool assigned = false;
            //we first search for an existing Node in the vicinity
            for(std::shared_ptr<NodePoint>& node : nodePts)
            {
                //if one is found, it becomes the first node of the current edge
                if(twoPointDist(node->getPosition(),firstEndPoint)<cornerThreshold)
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
                            if(twoPointDist(incPoint,firstEndPoint)<cornerThreshold)
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
                            if(twoPointDist(incPoint,firstEndPoint)<cornerThreshold)
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
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            
        if(!edge->getSecondNode())
        {
            bool assigned = false;
            for(std::shared_ptr<NodePoint>& node : nodePts)
            {
                if(twoPointDist(node->getPosition(),secondEndPoint)<cornerThreshold)
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
                            if(twoPointDist(incPoint,secondEndPoint)<cornerThreshold)
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
                            if(twoPointDist(incPoint,secondEndPoint)<cornerThreshold)
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
    std::cout<<"FINISHED CORNERS"<<std::endl;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::cout <<"===NODE SHAPES==="<<std::endl;
    std::cout <<"there are "<<graphNodes.size()<<std::endl; //■■■■■■■■■■■■■■■■■■■ OLD graphNodes; check me, daddy! ■■■■■■■■■■■■■■■■■■■
    //for(const std::shared_ptr<NodeShape> node : graphNodes)   ■■■■■■■■■■■■■■■■■■■ OLD graphNodes; check me, daddy! ■■■■■■■■■■■■■■■■■■■
    for (const auto it : graphNodesMap)
    {
        for (const std::shared_ptr<NodeShape> currentNode : it.second)    
        {
            std::cout<<"ShapeNode: "<<currentNode->getIdentifier() << " at "<<currentNode->getPosition().x<<"|"<<currentNode->getPosition().y<<std::endl;
        }
    }
    std::cout <<"===NODE POINTS==="<<std::endl;
    std::cout <<"there are "<<nodePts.size()<<std::endl;

    for(const std::shared_ptr<NodePoint> node : nodePts)
    {
        std::cout<<"Point Node at "<<node->getPosition().x<<"|"<<node->getPosition().y<<std::endl;
    }
    std::cout <<"===Edges==="<<std::endl;
    std::cout <<"there are "<<graphEdges.size()<<std::endl;
    for(const std::shared_ptr<Edge> edge : graphEdges)
    {
        std::cout<<(bool)edge->getFirstNode()<<(bool)edge->getSecondNode()<<std::endl;
        std::cout<<"Edge: "
            << (edge->getFirstNode() ?
                std::to_string(edge->getFirstNode().value()->getPosition().x) :
                "None")
            << "|"
            << (edge->getFirstNode() ?
                std::to_string(edge->getFirstNode().value()->getPosition().y) :
                "None")
            <<"--"
            << (edge->getSecondNode() ?
                std::to_string(edge->getSecondNode().value()->getPosition().x) :
                "None")
            <<"|"
            <<(edge->getSecondNode() ?
                std::to_string(edge->getSecondNode().value()->getPosition().y) :
                "None")
            <<std::endl;
    }


    std::vector<Connection> connections;
    //■■■■■■■■■■■■■■■■■■■ OLD graphNodes; check me, daddy! ■■■■■■■■■■■■■■■■■■■
    std::cout<<"||"<<graphEdges[0]->getFirstNode().value()<<"|"<<graphNodes[0]<<"|"<<graphNodes[1]<<"||"<<std::endl;
    linkShapes(graphNodesMap,connections);
    std::cout<<"FINISHED LINKING"<<std::endl;
    std::cout<<"===CONNECTIONS==="<<std::endl;
    for(const Connection& con : connections)
    {
        std::cout << "("<<con.getIdentifierOrigin()<<")--{";
        for(const std::pair<float,float>& ic : con.getIntermediateCorners())
        {
            std::cout<<"("<<ic.first<<"|"<<ic.second<<")";
        }
        std::cout << "}--("<<con.getIdentifierTarget()<<")"<<std::endl;
    }

    Diagram littleD;
    DefaultAlign defaultAlign;

    //for(const std::shared_ptr<NodeShape> node : graphNodes) 
    for (const auto& it : graphNodesMap)
    {
        for (const std::shared_ptr<NodeShape>& node : it.second)
        {
            //TODO produce shared_ptr<Rectangle|Polygon|Circle> from NodeShapes
            /* Shape* sp = new Shape(node->getShape());
            Rectangle* rptr = (Rectangle*)(&(sp)); */
            
            if(it.first == typeid(Rectangle))
            {
                std::shared_ptr<Shape> shape_ptr = std::make_shared<Shape>(node->getShape());
                const auto& shape_ptr_r = shape_ptr;
                auto& rec_ptr = (std::shared_ptr<Rectangle>&)shape_ptr_r;
                littleD.insertNode(rec_ptr);
            }
            else if(it.first == typeid(Circle))
            {
                std::shared_ptr<Shape> shape_ptr = std::make_shared<Shape>(node->getShape());
                const auto& shape_ptr_r = shape_ptr;
                auto& circ_ptr = (std::shared_ptr<Circle>&)shape_ptr_r;
                littleD.insertNode(circ_ptr);
            }
            else if(it.first == typeid(Polygon))
            {
                std::shared_ptr<Shape> shape_ptr = std::make_shared<Shape>(node->getShape());
                const auto& shape_ptr_r = shape_ptr;
                auto& poly_ptr = (std::shared_ptr<Polygon>&)shape_ptr_r;
                littleD.insertNode(poly_ptr);
            }

            /*Rectangle  r(rptr->getMinWidth(),rptr->getMinHeight(),node->getIdentifier(),node->getPosition().x/100,node->getPosition().y/-100);
            littleD.insertNode(FITTINGNODEPOINTERHERE);*/
        }
    }
    for(const Connection& con: connections)
    {
        littleD.insertConnection(std::make_shared<Connection>(con));
    }

    TikzGenerator gen;
    gen.generateEasyTikZ(littleD, &defaultAlign,TIKZ_ENV_FLAG,TEX_DOC_FLAG);
    return 0;
}
