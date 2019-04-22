#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;
int main (int argc, char** argv)
{

    if (argc !=2){

		printf("usage: main <Image_Path>\n");
		return -1;
	}
	IplImage* img =  cvLoadImage(argv[1]);
	if(!img)
	{
		printf("No image data!\n");
		return -1;

	}

    //converting the original image into grayscale
    IplImage* imgGrayScale = cvCreateImage(cvGetSize(img), 8, 1); 
    cvCvtColor(img,imgGrayScale,CV_BGR2GRAY);

    //thresholding the grayscale image to get better results
    cvThreshold(imgGrayScale,imgGrayScale,128,255,CV_THRESH_BINARY);  

    CvSeq* contours;  //hold the pointer to a contour in the memory block
    CvSeq* result;   //hold sequence of points of a contour
    CvMemStorage *storage = cvCreateMemStorage(0); //storage area for all contours

    //finding all contours in the image
    cvFindContours(imgGrayScale, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

    //iterating through each contour
    while(contours)
    {
        //obtain a sequence of points of contour, pointed by the variable 'contour'
        result = cvApproxPoly(contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, cvContourPerimeter(contours)*0.02, 0);

        //initialize array of CvPoints
        CvPoint *pt[result->total];
        //iterate over CvSeq-Array results and cast all elements into CvPoints to be inserted into the array
        for(int i = 0; i < result->total;i++){
            pt[i] = (CvPoint*)cvGetSeqElem(result, i);
        }
        //iterate over the point array and draw lines between consecutive points
        for(int i = 0; i < result->total;i++){
            //second point of line modulo #points to loop back to pt[0]
            cvLine(img, *pt[i],*pt[ (i+1) % result->total ],cvScalar(0,255,0),4);
        }

        //obtain the next contour
        contours = contours->h_next; 
    }
    // apply hough circles to find circles and draw a cirlce around it
    IplImage* imgCanny = cvCreateImage(cvGetSize(img), 8, 1); 

    cvCanny(imgGrayScale,imgCanny,0,0,3);
    CvSeq* circles = cvHoughCircles(imgCanny,
        storage,
        CV_HOUGH_GRADIENT,
        2,
        imgCanny->height/4,
        200,
        100 );

    for (int i = 0; i < circles->total; i++) 
    {
        float* p = (float*)cvGetSeqElem( circles, i );
        cvCircle( img, cvPoint(cvRound(p[0]),cvRound(p[1])), 
            3, CV_RGB(0,255,0), -1, 8, 0 );
        cvCircle( img, cvPoint(cvRound(p[0]),cvRound(p[1])), 
            cvRound(p[2]), CV_RGB(0,255,255), 3, 8, 0 );
    }
    //show the image in which identified shapes are marked   
    cvNamedWindow("Tracked");
    cvShowImage("Tracked",img);

    cvWaitKey(0); //wait for a key press

    //cleaning up
    cvDestroyAllWindows(); 
    cvReleaseMemStorage(&storage);
    cvReleaseImage(&img);
    cvReleaseImage(&imgGrayScale);

    return 0;
	//##################################
	// if (argc !=2){

	// 	printf("usage: DisplayImage.out <Image_Path>\n");
	// 	return -1;
	// }
	// Mat image;
	// image = imread(argv[1],1);
	// if(!image.data)
	// {
	// 	printf("No image data!\n");
	// 	return -1;

	// }
	// namedWindow("Display Image", WINDOW_AUTOSIZE);
	// imshow("Display Image",image);
	// waitKey(0);
	// return 0;
}