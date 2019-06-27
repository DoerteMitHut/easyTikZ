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
            for(int i = 1; i <= shape.size(); i++)
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
    std::vector<std::shared_ptr<Node>> graphNodes;
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
        cv::Rect2d axisParallelBoundingRect = cv::boundingRect(shape);


        //Polygons have between 3 and maxPolySides Sides.
        //Rectangles
        if(shape.size() == 4)
        {
            cv::RotatedRect r = cv::minAreaRect(shape);
            std::shared_ptr<Rectangle> gutesRect;
            if(std::atan(std::abs(r.angle*0.017453293))<std::atan(std::abs(68*0.017453293))&& std::atan(std::abs(r.angle*0.017453293))>std::atan(std::abs(23*0.017453293)))
            {
                gutesRect = std::make_shared<Rectangle>(axisParallelBoundingRect.width/100,axisParallelBoundingRect.height/100,true,"Shape_"+std::to_string(rects),centroid.x/100,centroid.y/100);
            }
            else
            {
                gutesRect = std::make_shared<Rectangle>(axisParallelBoundingRect.width/100,axisParallelBoundingRect.height/100,false,"Shape_"+std::to_string(rects),centroid.x/100,centroid.y/100);
            }
            std::shared_ptr<NodeShape> node = std::make_shared<NodeShape>(cv::Point2d(gutesRect->getRootCoordX(),gutesRect->getRootCoordY()),gutesRect,gutesRect->getIdentifier());
            node->setInnerRad = innerRad(shape,centroid);
            node->setOuterRad = outerRad(shape,centroid);
            graphNodes.push_back(node);

            rects++;
        }
        //Triangles and n>4-gons
        else
        {
            std::shared_ptr<Polygon> poly(std::make_shared<Polygon>(std::min(axisParallelBoundingRect.height/100,axisParallelBoundingRect.width/100),shape.size(),"Poly_"+std::to_string(polys),centroid.x/100,centroid.y/100));

            std::shared_ptr<NodeShape> node = std::make_shared<NodeShape>(cv::Point2d(poly->getRootCoordX(),poly->getRootCoordY()),poly,poly->getIdentifier());
            node->setInnerRad = innerRad(shape,centroid);
            node->setOuterRad = outerRad(shape,centroid);
            graphNodes.push_back(node);

            polys++;
        }
    }

    //Construct Shape Objects from circles
    int circs = 0;
    for(const cv::Vec3f& circ: circles)
    {
        std::shared_ptr<Circle> c = std::make_shared<Circle>(circ[2],"Circ_"+std::to_string(circs),circ[0]/100,circ[1]/100);

        std::shared_ptr<NodeShape> node = std::make_shared<NodeShape>(cv::Point2d(c->getRootCoordX(),c->getRootCoordY()),c,c->getIdentifier());
        node->setInnerRad = innerRad(circ);
        node->setOuterRad = outerRad(circ);
        graphNodes.push_back(node);
        shapeObjects.push_back(std::make_shared<Circle>(circ[2],"Circ_"+std::to_string(circs),circ[0]/100,circ[1]/100));
        circs++;
    }
    //construct Nodes from shapes and associate them with their incident edges
    for(std::shared_ptr<Node> node : graphNodes)
    {   
        //hand the constructed edges to the node so it can figure out with which of them to connect itself 
        node->connectIncidentEdges(graphEdges);
    }


    //construct NodePoint objects where unassociated endpoints of edges are close enough
    double cornerThreshold = 10;
    for(std::shared_ptr<Edge>& edge : graphEdges)
    {
        //if the first node is not set
        if(!edge->getFirstNode())
        {
            for(const std::shared_ptr<Edge>& incEdge : graphEdges)
            {
                //if incEdge is not the current edge
                if (incEdge != edge)
                {
                    if(!incEdge->getFirstNode().value())
                    {
                        
                    }
                }
            }
        }
    }

    //number. keep it. might get important later on.
    int num  = 0;

    // for(const std::shared_ptr<Node>& node : graphNodes)
    // {
    //     cv::Mat tempImg;
    //     imgOutput.copyTo(tempImg);
    //     for(int i = 1; i <= node->getShape().value().size(); i++)
    //     {
    //         cv::line(tempImg,node->shape.value()[i-1],node->shape.value()[i%node->shape.value().size()],cv::Scalar(0,255,0),4);
    //     }
    //     for(auto& it : node->edges)
    //     {
    //         cv::line(tempImg,cv::Point2d(it.second->line[0],it.second->line[1]),cv::Point2d(it.second->line[2],it.second->line[3]),cv::Scalar(0,0,255),4);
    //     }
    //     displayImg("shape "+std::to_string(num),tempImg);
    //     num++;
    // }

    Diagram littleD;
    DefaultAlign defaultAlign;
    int shapenum = 0;
    for (const std::vector<cv::Point2d>&  shape2 : shapes )
    {   
        std::vector<cv::Point2f> shape;
        for(const cv::Point2d& p : shape2)
        {
            shape.push_back((cv::Point2f)p);
        }

        std::cout<<shape.size()<<std::endl; 
        cv::RotatedRect r = cv::minAreaRect(shape);
        std::shared_ptr<Rectangle> gutesRect;
        cv::Rect2d axisParallelBoundingRect = cv::boundingRect(shape);
        cv::Moments mom = cv::moments(shape,false);
        std::cout<<"angle: "<<r.angle<<std::endl;
        if(std::atan(std::abs(r.angle*0.017453293))<std::atan(std::abs(68*0.017453293))&& std::atan(std::abs(r.angle*0.017453293))>std::atan(std::abs(23*0.017453293)))
        {
            std::cout<<"ROTATED"<<std::endl;
            gutesRect = std::make_shared<Rectangle>(axisParallelBoundingRect.width/100,axisParallelBoundingRect.height/100,true,"Shape_"+std::to_string(shapenum),(mom.m10/mom.m00)/100, -(mom.m01/mom.m00)/100);
        }
        else
        {
            std::cout<<"UNROTATED"<<std::endl;
            gutesRect = std::make_shared<Rectangle>(axisParallelBoundingRect.width/100,axisParallelBoundingRect.height/100,false,"Shape_"+std::to_string(shapenum),(mom.m10/mom.m00)/100, -(mom.m01/mom.m00)/100);
        }
        littleD.insertNode(gutesRect);
        shapenum++;
    }
    for (const cv::Vec3f& circ : circles )
    {
        std::shared_ptr<Circle> guterCircle = std::make_shared<Circle>(circ[2]/100,"Shape_"+std::to_string(shapenum),circ[0]/100,-circ[1]/100);
        littleD.insertNode(guterCircle);
        shapenum++;
    }

    TikzGenerator gen;
    gen.generateEasyTikZ(littleD, &defaultAlign,TIKZ_ENV_FLAG,TEX_DOC_FLAG);
    return 0;
}
