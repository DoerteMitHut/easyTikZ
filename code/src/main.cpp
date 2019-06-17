#include <iostream>
#include <opencv2/opencv.hpp>
#include "Diagram.h"
#include "TikzGenerator.h"
#include "ShapeRecognition.h"
#include "preprocessing.h"


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
        HoughCircles(imgFilled, circles, CV_HOUGH_GRADIENT, 1, gaussian.rows/8, 200,15);
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
    // for(const cv::Point2d& p : shape_centroids)
    // {
    //     //cv::circle(fin2,p,4,cv::Scalar(0,0,255),-1,8,0);
    // }

    //displayImg("finished",fin2);

    std::vector<std::shared_ptr<Edge>> graphEdges;
    std::vector<std::shared_ptr<Node>> graphNodes;
    for(const cv::Vec4d& e : edges)
    {   
        std::shared_ptr<Edge> ep = std::make_shared<Edge>(e,std::pair<std::shared_ptr<Node>,std::shared_ptr<Node>>()); 
        graphEdges.push_back(ep);
    }
    
    std::cout<<"GRAPH EDGES"<<std::endl;
    for(const std::shared_ptr<Edge>& e: graphEdges)
    {
        std::cout<<"("<<e->line[0]<<"|"<<e->line[1]<<")--("<<e->line[2]<<"|"<<e->line[3]<<")"<<std::endl;     
    }

    for(const std::vector<cv::Point2d>& shape : shapes)
    {
        std::vector<std::shared_ptr<Edge>> incidentEdges = findIncidentEdges(shape,graphEdges);
        graphNodes.push_back(std::make_shared<Node>(true,false,shape,incidentEdges));
    }

    int num  = 0;
    for(const std::shared_ptr<Node>& node : graphNodes)
    {
        cv::Mat tempImg;
        imgOutput.copyTo(tempImg);
        for(int i = 1; i <= node->shape.size(); i++)
        {
            cv::line(tempImg,node->shape[i-1],node->shape[i%node->shape.size()],cv::Scalar(0,255,0),4);
        }
        for(const std::shared_ptr<Edge> e : node->edges)
        {
            cv::line(tempImg,cv::Point2d(e->line[0],e->line[1]),cv::Point2d(e->line[2],e->line[3]),cv::Scalar(0,0,255),4);
        }
        displayImg("shape "+std::to_string(num),tempImg);
        num++;
    }

    Diagram littleD;
    DefaultAlign defaultAlign;

    // for (const std::vector<cv::Point>&  contour : contours )
    // {
    //     cv::Rect2d r = cv::boundingRect(contour);
    //     cv::Moments mom = cv::moments(contour,false);
    //     auto gutesRect = std::make_shared<Rectangle>(r.width/100,r.height/100,"firstBoi",(mom.m10/mom.m00)/100, -(mom.m01/mom.m00)/100);
    //     littleD.insertRectangle(gutesRect);
    // }

    TikzGenerator turningCertainShapesToAsh;
    turningCertainShapesToAsh.generateEasyTikZ(littleD, &defaultAlign, TIKZ_ENV_FLAG, TEX_DOC_FLAG);
    return 0;
}
