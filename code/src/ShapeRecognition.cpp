#include "ShapeRecognition.h"

const double DEG2RAD = 360./CV_2PI;

//computes a clustering function 

void sortLineVector(std::vector<cv::Vec4i>& segments)
{
    std::sort(segments.begin(),segments.end(),
        [](const cv::Vec4i& l1, const cv::Vec4i& l2)->bool
        {
            return std::min(l1[0],l1[2])<std::min(l2[0],l2[2]);
        });
}

//computes Nacken's metric for line segments
double nackenDist(cv::Vec4d s1,cv::Vec4d s2, bool consider_translation, double sigma_d,double sigma_w,double sigma_l)
{
    cv::Point2d p_1_1,p_1_2,p_2_1,p_2_2;

    // making sure both lines consider their (x-component wise) endpoint to be the first one
    if(s1[0]>s1[2])
    {
        p_1_1 = cv::Point2d(s1[2],s1[3]);
        p_1_2 = cv::Point2d(s1[0],s1[1]);
    }
    else
    {
        p_1_2 = cv::Point2d(s1[2],s1[3]);
        p_1_1 = cv::Point2d(s1[0],s1[1]);
    }
    if(s2[0]>s2[2])
    {
        p_2_1 = cv::Point2d(s2[2],s2[3]);
        p_2_2 = cv::Point2d(s2[0],s2[1]);
    }
    else
    {
        p_2_2 = cv::Point2d(s2[2],s2[3]);
        p_2_1 = cv::Point2d(s2[0],s2[1]);
    }
    //calculating vectors
    cv::Point2d v_1 = p_1_2-p_1_1;
    cv::Point2d v_2 = p_2_2-p_2_1;
    
    //calculating angles in deg
    double phi_1, phi_2;
    phi_1 = (double)cv::fastAtan2(v_1.y,v_1.x);
    phi_2 = (double)cv::fastAtan2(v_2.y,v_2.x);
    
    phi_1 = abs(phi_1)<=90?phi_1:phi_1-180;
    phi_2 = abs(phi_2)<=90?phi_2:phi_2-180;
    
    //calculating standard deviations
    //sigma_l is not needed for the method proposed by other people
    // double sigma_d, sigma_w, sigma_l;
    sigma_d = 20;//270/cv::sqrt(30*l_2);
    sigma_w = 2000;//l_2/4.;
    sigma_l = 2000;
    // computing Nacken's metric for line segments
    // rotational component
    //double rot_comp = (1./(cv::pow(CV_PI*cv::pow(sigma_d,2),1./4.))*cv::exp(-((phi_2-phi_1)/cv::pow(sigma_d,2.))));
    double rot_comp = cv::exp(-cv::pow((phi_2-phi_1),2.)/cv::pow(sigma_d,2.));
    // translational component
    // calculating the centers of the lines
    cv::Point2d c_1,c_2,c_2_1,c_2_1_r;
    c_1 = p_1_1+v_1/2.;
    c_2 = p_2_1+v_2/2.;
    //difference between centers
    c_2_1 = c_1-c_2;
    // rotating everything to align s_2 with the x-axis
    c_2_1_r = cv::Point2d(c_1.x*cos(-phi_2/DEG2RAD)-c_1.y*sin(-phi_2/DEG2RAD) , c_1.x*sin(-phi_2/DEG2RAD)+c_1.y*cos(-phi_2/DEG2RAD));

    double disp_comp = consider_translation ?
    cv::exp((-cv::pow((c_2_1_r.x),2.)/cv::pow(sigma_l,2.)) * cv::pow((c_2_1_r.y),2.)/cv::pow(sigma_w,2.))
    :cv::exp(-(cv::pow((c_2_1_r.y)/sigma_w,2.)));
    // double disp_comp = consider_translation ?
    // (1/(cv::sqrt(CV_PI*cv::pow(sigma_l,2.)*cv::pow(sigma_w,2.))))*cv::exp(-cv::pow((c_2_1_r.x)/sigma_l,2.) - cv::pow((c_2_1_r.y)/sigma_w,2.))
    // :(1/(cv::sqrt(CV_PI*cv::pow(sigma_w,2.))))*cv::exp(-((c_2_1_r.y)/cv::pow(sigma_w,2.)));
    
    
    double ret = rot_comp*disp_comp;
    return ret;
}

double otsu(std::vector<double> hist)
{
    // sum over all bins
    double total = 0;
    for(const double d : hist)
    {
            total += d;
    }

    double sumB = 0;
    double wB = 0;
    double maximum = 0.0;
    double sum1 = 0;
    double s = 0;
    //iterate over all bins
    for (unsigned int i = 0; i < hist.size(); i++)
    {
        //weigh the bin numbers with their respective occurences
        sum1 += i*hist[i];
    }

    //iterate over hist again
    for (unsigned int ii = 0; ii < hist.size();ii++)
    {   
        double wF = total - wB;
        if (wB > 0 && wF > 0)
        {
            double mF = (sum1 - sumB) / wF;
            double val = wB * wF * ((sumB / wB) - mF) * ((sumB / wB) - mF);
            if (val >= maximum)
            {
                s = ii;
                maximum = val;
            }
        }
        wB = wB + hist[ii];
        sumB = sumB + (ii-1) * hist[ii];
    }
    return s;
}

void fillShapes(cv::Mat src, cv::Mat& dst)
{
    
    cv::Rect* fillBounds = new cv::Rect();
    //Image to hold result of floodfill. Receives copy of src.
    src.copyTo(dst);
    //DEBUGGING OUTPUT
    cv::floodFill(dst, cv::Point(1,1), cv::Scalar(0,0,0), fillBounds, cv::Scalar(1), cv::Scalar(1), 4|(0<<8));
    if((fillBounds->height!=src.rows))
    {
        // abort program if the outter pixel frame of the binary image was not filled. This indicates wrong foreground-background-separation.
        delete fillBounds;
    }
    if( (fillBounds->width != src.rows))
    {
        delete fillBounds;
    }
    //delete bounding rect
    delete fillBounds;

}

void findShapes(cv::Mat src, std::vector<std::vector<cv::Point2d>>& shapes, unsigned int maxVertCount)
{
    
    //finding contours to approximate shapes
    std::vector<std::vector<cv::Point> > contours;

    cv::findContours(src, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE,cv::Point(0,0));
    
    //vector to hold shape centroids
    std::vector<cv::Point2d>              shape_centroids;
    std::vector<std::vector<cv::Point2d>> polys;

    //iterate over contours
    for(const std::vector<cv::Point>& contour : contours)
    {
        //approximated polygon from contour via Douglas-Peucker algorithm
        std::vector<cv::Point2d> result;
        cv::approxPolyDP(contour, result , cv::arcLength(contour,true)*0.02, true);

        //consider only polygons with max. 6 vertices
        if(result.size() <= maxVertCount){
            shapes.push_back(result);
        }
    }
}

void findShapeCentroids(std::vector<std::vector<cv::Point>> shapes, std::vector<cv::Point2d>& centroids)
{
    for(const std::vector<cv::Point>& shape : shapes)
    {
        cv::Moments mom = cv::moments( shape, false );
        centroids.push_back(cv::Point2d( mom.m10/mom.m00 , mom.m01/mom.m00));
    }
}

void connectorImage(cv::Mat srcShapes,cv::Mat srcBin, cv::Mat& dst,unsigned int dilationDepth, int dilationShape, cv::Size kernelSize,cv::Point anchor)
{
    cv::Mat imgFilledInv;
    for(unsigned int i = 0; i< dilationDepth;i++){
        cv::dilate(srcShapes,srcShapes,cv::getStructuringElement(dilationShape,kernelSize,anchor));
    }

    //compute !imgFilled & imgBinary to delete shapes and their outlines and leave only connecting lines
    cv::bitwise_not( srcShapes    ,imgFilledInv );
    cv::bitwise_and( imgFilledInv ,srcBin ,dst );
}

void findCorners(cv::Mat src ,std::vector<cv::Point2d>& corners,double minDist)
{
    cv::Mat imgHarris;

    //find corners (endpoints) of edgeCandidates via Harris corner detection
    cv::cornerHarris(src,imgHarris,5,5,0.1);

    cv::dilate(imgHarris,imgHarris,cv::getStructuringElement(cv::MORPH_RECT,cv::Size(7,7),cv::Point(-1,-1)));
    cv::threshold(imgHarris,imgHarris,0.5,1,cv::THRESH_BINARY);

    double min, max;
    cv::minMaxLoc(imgHarris, &min, &max);
    imgHarris-=min;
    imgHarris.convertTo(imgHarris,CV_8U,255.0/(max-min));

    std::vector<std::vector<cv::Point>> contours2;
    std::vector<cv::Vec4i> hierarchy;
    cv::Canny(imgHarris,imgHarris,50,150,3);
    
    cv::findContours(imgHarris,contours2,hierarchy,cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
    // get the moments
    std::vector<cv::Moments> blobMoments(contours2.size());
    for(unsigned int i = 0; i<contours2.size(); i++ )
    {
        blobMoments[i] = moments( contours2[i], false );
    }
    // get the centroid of figures.
    for(const cv::Moments m : blobMoments)
    {
        cv::Point2d centroid = cv::Point2d( m.m10/m.m00 , m.m01/m.m00);
        bool valid = true;
        for (const cv::Point2d& p : corners)
        {
            if (cv::sqrt(cv::pow(centroid.x - p.x,2.) + cv::pow(centroid.y - p.y,2.)) < minDist)
            {
                valid = false;
            }
        }
        if(valid)
        {
            corners.push_back(centroid);
            cv::circle(imgHarris,cv::Point(centroid),4,cv::Scalar(255),-1,8,0);
        }
    }
}

void generateEdges( const std::vector<cv::Point2d>& corners, std::vector<cv::Vec4d>& edges)
{
    for(const cv::Point& p : corners)
    {
        for (const cv::Point& q : corners)
        {
            if(p!=q)
            {
                if(q.x <= p.x)
                {
                    edges.push_back(cv::Vec4i(q.x,q.y,p.x,p.y));
                }
                else
                {
                    edges.push_back(cv::Vec4i(p.x,p.y,q.x,q.y));
                }
            }
        }
    }
}

void computeEdgeSupport(std::vector<cv::Vec4d> lines, std::vector<cv::Vec4d> edgeCandidates, std::vector<double>& support)
{
    support.resize(edgeCandidates.size());
    std::fill(support.begin(), support.end(), 0.);

    // iterate over all houghlines to find the respective edge candidate each one supports.
    for (const cv::Vec4d& line : lines){
        unsigned int bestIndex = edgeCandidates.size();
        double bestNacken = -1;

        //iterate over all edgeCandidates to calculate which one is most supported by line
        for(unsigned int i = 0; i < edgeCandidates.size(); i++)
        {
            //compute Nacken's metric for line segment similarity
            double nacken = nackenDist(line,edgeCandidates[i],true,15,50,20);
            //replace line's favorite edge if necessary
            if(nacken > bestNacken)
            {
                bestIndex = i;
                bestNacken = nacken;
            }
        }
        //gather supporting lines for winning edgeCandidates
        support[bestIndex]++;
    }
}

void findIncidentEdges(  const std::vector<cv::Point2d>& shape,
                         const std::vector<std::shared_ptr<Edge>>& edges,
                         std::vector<std::pair<Position,std::shared_ptr<Edge>>>& dstEdges )
{
    cv::Point2d centroid(0,0);
    
    for(const cv::Point2d &  pt : shape)
    {
        centroid.x += pt.x;
        centroid.y += pt.y;
    }
    centroid.x /= shape.size();
    centroid.y /= shape.size();

    double outterRad = 0;
    for (const cv::Point2d& pt : shape)
    {
        if (twoPointDist(pt,centroid) > outterRad)
        {
            outterRad = twoPointDist(pt,centroid);
        }
    }
    outterRad *=1.5;
    double innerRad = outterRad; 

    for(unsigned int i = 1; i< shape.size();i++)
    {
        cv::Point2d p = shape[i-1];
        cv::Point2d q = shape[i%shape.size()];

        cv::Point2d vec = cv::Point2d((q.x-p.x)/2.,(q.y-p.y)/2.);
        cv::Point2d center = p+vec;

        if(twoPointDist(center,centroid)<innerRad)
        {
            innerRad = twoPointDist(center,centroid);
        }
    }

    for(std::shared_ptr<Edge> e : edges)
    {
        cv::Vec4d line = e->getLine();
        cv::Point2d endPointL(line[0],line[1]);
        cv::Point2d endPointR(line[2],line[3]);
        cv::Point2d vec = endPointR-endPointL; 
        cv::Point2d centroidVec = centroid-endPointL;
        if(twoPointDist(endPointL,centroid)<=outterRad)
        {   
            double normVec;
            normVec = pointNorm(vec);
            vec /= normVec;
            cv::Point2d projPoint = vec * pointDotProduct(vec,centroidVec);
            cv::Point2d projvec = projPoint-centroidVec;
            if((pointNorm(projvec) < innerRad))
            {
                dstEdges.push_back(std::pair(Position::first,e));
                continue;
            }
        }
        else{}

        if(twoPointDist(endPointR,centroid)<=outterRad)
        {
            vec = endPointL-endPointR; 
            centroidVec = centroid-endPointR;
            
            double normVec;
            normVec = pointNorm(vec);
            vec /= normVec;
            double dottiProducti = pointDotProduct(vec,centroidVec);
            
            cv::Point2d projPoint = vec * dottiProducti;
            cv::Point2d projvec = projPoint-centroidVec;
            if((pointNorm(projvec) < innerRad))
            {
                dstEdges.push_back(std::pair(Position::second,e));
                continue;
            }
        }
        else{}
    }
}

/*Called once to start a DFS on each connected component of the graph to draw connections between shapes. Works on vectors of shared_ptrs for existing nodes and edges and writes found connections to dstConnections.*/
void linkShapes(std::unordered_map<std::type_index,std::vector<std::shared_ptr<NodeShape>>>& nodeMap, std::vector<Connection>& dstConnections)
{  
    //iterate over nodes as possible starting points for DFS
    for (const auto& it : nodeMap)
    {
        for (const auto& node : it.second)
        {
            //nodes may be marked as start during DFS to prevent superfluous DFS inits
            //also DFS can only start on nodes that represent shapes
            if(!node->getMarkedStart())
            {
                //global vertex stack is initialized with first(random) node
                //start search for shapeNodes from this one
                std::unordered_map<std::shared_ptr<Node>,std::shared_ptr<Connection>> unfinishedConnections;
                node->dfsStep(unfinishedConnections,dstConnections);
            }
        }
    }
}

/*Handles the node at stack.back() during a search for shapes on one connected component.
Receives the current stack, a map of Node pointers and Connections pointers to store unfinished Connections and a reference to the output vector to emplace finished Connections in.*/

void computeShapes(const cv::Mat& imgBinary,cv::Mat& imgFilled, std::vector<std::vector<cv::Point2d>>& shapes, std::vector<cv::Vec3f>& circles)
{
        /////// POLYGONS //////////////////////////////////////
    ///////////////////////////////////////////////////////
    fillShapes(imgBinary,imgFilled);
    findShapes(imgFilled,shapes);

    /////// CIRCLES ///////////////////////////////////////
    ///////////////////////////////////////////////////////

    // apply hough circles to find circles and draw a cirlce around it
    {
        cv::Mat gaussian;
        GaussianBlur( imgFilled, gaussian, cv::Size(9, 9), 2, 2 );
        HoughCircles(imgFilled, circles, CV_HOUGH_GRADIENT, 1, gaussian.rows/8, 200,18);
    }
}