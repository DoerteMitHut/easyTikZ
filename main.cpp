#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;
int main (int argc, char** argv)
{
    //check whether there is a command line argument given
    if (argc !=2){

		printf("usage: main <Image_Path>\n");
		return -1;
	}
    //if there is a command line argument, use it to set the path for the source image
	Mat img =  imread(argv[1]);
	if(img.empty())
	{
		printf("No image data!\n");
		return -1;
	}

    //converting the original image into grayscale
    Mat imgGrayScale;
    cvtColor(img, imgGrayScale,CV_BGR2GRAY);

    cvNamedWindow("Tracked");
    cv::imshow("Tracked",imgGrayScale);

    cvWaitKey(0); //wait for a key press


    //thresholding the grayscale image to get better results
    Mat imgBinary;  
    //adaptiveThreshold(imgGrayScale,imgBinary,255,CV_ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,3,0);
    //apply Otsu's binarization
    threshold(imgGrayScale,imgBinary,128,255,THRESH_BINARY_INV | CV_THRESH_OTSU);
    cvNamedWindow("Tracked");
    cv::imshow("Tracked",imgBinary);

    cvWaitKey(0); //wait for a key press

    //depth 1 morphological closing
    for(int i = 0; i< 1;i++){
    dilate(imgBinary,imgBinary,getStructuringElement(MORPH_CROSS,Size(7,7),Point(-1,-1)));
    cvNamedWindow("Tracked");
    cv::imshow("Tracked",imgBinary);

    cvWaitKey(0); //wait for a key press
    }
    for(int i = 0; i< 1;i++){
    erode(imgBinary,imgBinary,getStructuringElement(MORPH_CROSS,Size(7,7),Point(-1,-1)));
    cvNamedWindow("Tracked");
    cv::imshow("Tracked",imgBinary);

    cvWaitKey(0); //wait for a key press
    }

    // vector<Vec4i> lines;
    // HoughLinesP(imgBinary,lines, 10,CV_PI/180,50,30);
    // for (Vec4i line : lines){
    //     cv::line(img,Point(line[0],line[1]),Point(line[2],line[3]),cvScalar(0,0,255),4);
    // }

    // cvNamedWindow("Tracked");
    // cv::imshow("lines",imgBinary);

    // cvWaitKey(0); //wait for a key press

    vector<vector<Point> > contours; // vector of point vectors to hold the detected contours.
    vector<Point> result;   //hold sequence of points of a contour

    findContours(imgBinary, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE,Point(0,0));
    cout<<"#contours = "<<contours.size()<<endl;
    int count =0;
    while(contours.size()>100){
        
        findContours(imgBinary, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE,Point(0,0));
        cout<<"#contours2 = "<<contours.size()<<endl;
        cvNamedWindow("Tracked");
        cv::imshow("Tracked",imgBinary);

        cvWaitKey(0);
    }
    for(int i = 0; i<count;i++){
        erode(imgBinary,imgBinary,getStructuringElement(MORPH_CROSS,Size(3,3),Point(-1,-1)));
    }
    
    //iterating through each contour
    for(vector<Point> contour : contours)
    {
        //obtain a sequence of points of contour, pointed by the variable 'contour'
        vector<Point> result;
        approxPolyDP(contour, result , arcLength(contour,true)*0.02, true);
        if(result.size() <= 6){
            //initialize array of CvPoints
            vector<Point> pt;
            //iterate over CvSeq-Array results and cast all elements into CvPoints to be inserted into the array
            for(Point p : result){
                pt.insert(pt.end(),p);
            }
            //iterate over the point array and draw lines between consecutive points
            for(int i = 0; i < result.size();i++){
                //second point of line modulo #points to loop back to pt[0]
                line(img, pt[i],pt[ (i+1) % result.size()],cvScalar(0,255,0),4);
            }
        }
    }

    cvNamedWindow("Tracked");
    cv::imshow("toll",img);

    //wait for a key press
    cvWaitKey(0); 

    //cleaning up
    cvDestroyAllWindows();

    return 0;
}