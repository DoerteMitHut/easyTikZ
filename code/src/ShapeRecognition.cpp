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

double pointDotProduct(const cv::Point2d &u,const cv::Point2d &v)
{
    return cv::sqrt(u.x*v.x+u.y*v.y); 
}

double twoPointDist(const cv::Point& p,const cv::Point& q)
{
    return std::sqrt(std::pow(p.x-q.x,2)+std::pow(p.y-q.y,2));
}

double clusterFunction(std::vector<cv::Vec4i> segments)
{   
    sortLineVector(segments);

    double best_nacken = -1;
    cv::Vec4i best_s_l;
    cv::Vec4i best_s_i;

    for(cv::Vec4i s_l : segments)
    {
        cv::Point2d p_l_1,p_l_2;

        // order endpoints along x-axis
        if(s_l[0]>s_l[2])
        {
            p_l_1 = cv::Point2d(s_l[2],s_l[3]);
            p_l_2 = cv::Point2d(s_l[0],s_l[1]);
        }
        else
        {
            p_l_2 = cv::Point2d(s_l[2],s_l[3]);
            p_l_1 = cv::Point2d(s_l[0],s_l[1]);
        }

        //directional vector of s_l
        cv::Point2d v_l = cv::Point2d(p_l_2-p_l_1);
        //length of s_l
        double len_s_l = sqrt(cv::pow(v_l.x,2.)+cv::pow(v_l.y,2.));
        //normalized directional vector of s_l
        cv::Point2d v_l_normalized = v_l / len_s_l;
        
        //endpoints of s_c
        cv::Point2d first_proj,last_proj;
        first_proj = cv::Point2d(HUGE_VAL,HUGE_VAL);
        first_proj = cv::Point2d(-HUGE_VAL,-HUGE_VAL);
        //accumulated length of all segment projections onto candidate s_l
        double gross_proj_length = 0;

        for(cv::Vec4i s_i : segments)
        {
            if(s_i != s_l)
            {
                 cv::Point2d p_i_1,p_i_2;

                // order endpoints along x-axis
                if(s_i[0]>s_i[2])
                {
                    p_i_1 = cv::Point2d(s_l[2],s_i[3]);
                    p_i_2 = cv::Point2d(s_l[0],s_i[1]);
                }
                else
                {
                    p_i_2 = cv::Point2d(s_i[2],s_i[3]);
                    p_i_1 = cv::Point2d(s_i[0],s_i[1]);
                }
                //projections of endpoints of s_i
                cv::Point2d proj_p_i_1,proj_p_i_2;
                
                //shift origin to p_l_1, project endpoints of s_i onto resulting line and shift back
                proj_p_i_1 = p_l_1 + pointDotProduct(v_l_normalized,p_i_1-p_l_1)*v_l_normalized;
                proj_p_i_2 = p_l_1 + pointDotProduct(v_l_normalized,p_i_2-p_l_1)*v_l_normalized;
                //vector connecting the projections of p_i_1 and p_i_2
                //cv::Point2d proj_v_i = proj_p_i_2-proj_p_i_1;
                //update endpoints of s_c
                if (proj_p_i_1.x < first_proj.x)
                {
                    first_proj = proj_p_i_1;
                }
                if (proj_p_i_1.x > last_proj.x)
                {
                    first_proj = proj_p_i_1;
                }
                //update length covered by projections
                if (proj_p_i_1.x<last_proj.x)
                {//if the first endpoint of s_i has a lower x-component than the latest projected point, the overlap has to be subtracted from s_i's contribution to the gross projection length
                    //the overlap might be equal to either the distance between s_i's second projected endpoint and the last projected endpoint or zero if s_i's projection lies completely within the last projection. 
                    gross_proj_length += proj_p_i_2.x>last_proj.x ? cv::sqrt(cv::pow((proj_p_i_2-last_proj).x,2.)+cv::pow((proj_p_i_2-last_proj).y,2.)) : 0.;
                }
                double nacken_l_i = nackenDist(s_i,s_l,false,20,10,10);
                //std::cout<<"lines:\n ("<<p_l_1.x<<"|"<<p_l_1.y<<")--("<<p_l_2.x<<"|"<<p_l_2.y<<")\n("<<p_i_1.x<<"|"<<p_i_1.y<<")--("<<p_l_2.x<<"|"<<p_l_2.y<<")\n have nacken distance "<<nacken_l_i<<"\n\n"<<std::endl;  
                if(nacken_l_i > best_nacken)
                {
                    best_nacken = nacken_l_i;
                    best_s_i = s_i;
                    best_s_l = s_l;
                }
            }
        }
    }
    //std::cout<<"\n=============\nlines:\n ("<<best_s_l[0]<<"|"<<best_s_l[1]<<")--("<<best_s_l[2]<<"|"<<best_s_l[3]<<")\n("<<best_s_i[0]<<"|"<<best_s_i[1]<<")--("<<best_s_i[2]<<"|"<<best_s_i[3]<<")\n WIN with nacken distance "<<best_nacken<<"\n\n"<<std::endl;  
    //std::cout<<"BTW: DEG2RAD was "<<DEG2RAD<<". I hope that's OK.";
            return 3.;

}
//computes Nacken's metric for line segments
double nackenDist(cv::Vec4d s1,cv::Vec4d s2, bool consider_translation, double sigma_d,double sigma_w,double sigma_l)
{
    std::cout.setstate(std::ios_base::failbit);
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

    std::cout << "Calculating Nacken metric for ("<<p_1_1.x<<"|"<<p_1_1.y<<")--("<<p_1_2.x<<"|"<<p_1_2.y<<") and ("<<p_2_1.x<<"|"<<p_2_1.y<<")--("<<p_2_2.x<<"|"<<p_2_2.y<<")";

    //calculating vectors
    cv::Point2d v_1 = p_1_2-p_1_1;
    cv::Point2d v_2 = p_2_2-p_2_1;
    std::cout << "\nvectors are ("<<v_1.x<<"|"<<v_1.y<<") and ("<<v_2.x<<"|"<<v_2.y<<")"<<std::endl; 
    
    //calculating line lengths
    double l_1,l_2;
    l_1 = cv::sqrt(v_1.x*v_1.x+v_1.y*v_1.y);
    l_2 = cv::sqrt(v_2.x*v_2.x+v_2.y*v_2.y); 

    std::cout << "l1: " << l_1 << std::endl << "l2: " << l_2 << std::endl;

    //calculating angles in deg
    double phi_1, phi_2, delta_phi;
    phi_1 = (double)cv::fastAtan2(v_1.y,v_1.x);
    phi_2 = (double)cv::fastAtan2(v_2.y,v_2.x);
    std::cout << "angles are "<< phi_1<< " and " << phi_2 <<std::endl;
    
    phi_1 = abs(phi_1)<=90?phi_1:phi_1-180;
    phi_2 = abs(phi_2)<=90?phi_2:phi_2-180;
    
    delta_phi = phi_2-phi_1;
    std::cout << "phi_1: " << phi_1 << std::endl << "phi_2: " << phi_2 << std::endl<< "delta_phi: "<<delta_phi<<std::endl;


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
    std::cout<<"centers are: "<<c_1 <<" and "<<c_2<<std::endl;
    //difference between centers
    c_2_1 = c_1-c_2;
    std::cout<<"center distance: "<<cv::sqrt(cv::pow(c_2_1.x,2.)+cv::pow(c_2_1.y,2.)) << std::endl;
    // rotating everything to align s_2 with the x-axis
    c_2_1_r = cv::Point2d(c_1.x*cos(-phi_2/DEG2RAD)-c_1.y*sin(-phi_2/DEG2RAD) , c_1.x*sin(-phi_2/DEG2RAD)+c_1.y*cos(-phi_2/DEG2RAD));

    double disp_comp = consider_translation ?
    cv::exp((-cv::pow((c_2_1_r.x),2.)/cv::pow(sigma_l,2.)) * cv::pow((c_2_1_r.y),2.)/cv::pow(sigma_w,2.))
    :cv::exp(-(cv::pow((c_2_1_r.y)/sigma_w,2.)));
    // double disp_comp = consider_translation ?
    // (1/(cv::sqrt(CV_PI*cv::pow(sigma_l,2.)*cv::pow(sigma_w,2.))))*cv::exp(-cv::pow((c_2_1_r.x)/sigma_l,2.) - cv::pow((c_2_1_r.y)/sigma_w,2.))
    // :(1/(cv::sqrt(CV_PI*cv::pow(sigma_w,2.))))*cv::exp(-((c_2_1_r.y)/cv::pow(sigma_w,2.)));
    
    
    std::cout << "r: " << rot_comp << std::endl << "t: " << disp_comp << std::endl;
    double ret = rot_comp*disp_comp;
    if(ret>0.1)
    {
        std::cout<<"**************************************"<<std::endl<<ret<<std::endl<<"**************************************"<<std::endl;
    }
    std::cout.clear();
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
    double toRet = 0;
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
                toRet = ii;
                maximum = val;
            }
        }
        wB = wB + hist[ii];
        sumB = sumB + (ii-1) * hist[ii];
    }
    return toRet;
}

void fillShapes(cv::Mat src, cv::Mat& dst)
{
    
    cv::Rect* fillBounds = new cv::Rect();
    //Image to hold result of floodfill. Receives copy of src.
    src.copyTo(dst);
    //DEBUGGING OUTPUT
    std::cout <<"floddfill returned "<< cv::floodFill(dst, cv::Point(1,1), cv::Scalar(0,0,0), fillBounds, cv::Scalar(1), cv::Scalar(1), 4|(0<<8))<<std::endl;
    std::cout << "floodfill returned Rect at (" << fillBounds->x <<"|"<<fillBounds->y<<") with width " << fillBounds->width << " and height" << fillBounds->height<<"."<<std::endl;
    std::cout<<typeid(fillBounds->width).name()<<"|"<<typeid(src.cols).name()<<std::endl;
    std::cout << dst.rows <<"|"<<dst.cols<<std::endl;
    std::cout << src.rows <<"|"<<src.cols<<std::endl;
    std::cout << (src.rows==fillBounds->height)<<std::endl;
    std::cout << (src.cols==fillBounds->width)<<std::endl;
    if((fillBounds->height!=src.rows))
    {
        std::cout<<"height wrong"<<std::endl;
        if( (fillBounds->width != src.rows))
        {
            std::cout<<"width also wrong"<<std::endl;
        }
        // abort program if the outter pixel frame of the binary image was not filled. This indicates wrong foreground-background-separation.
        delete fillBounds;
    }
    if( (fillBounds->width != src.rows))
    {
        std::cout<<"width wrong"<<std::endl;
        delete fillBounds;
    }
    //delete bounding rect
    delete fillBounds;

}

void findShapes(cv::Mat src, std::vector<std::vector<cv::Point2d>>& shapes, int maxVertCount)
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

void connectorImage(cv::Mat srcShapes,cv::Mat srcBin, cv::Mat& dst,int dilationDepth, int dilationShape, cv::Size kernelSize,cv::Point anchor)
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
        std::cout << "("<<m.m10/m.m00<<"|"<< m.m01/m.m00<<")"<<std::endl;
        cv::Point2d centroid = cv::Point2d( m.m10/m.m00 , m.m01/m.m00);
        bool valid = true;
        for (const cv::Point2d& p : corners)
        {
            if (cv::sqrt(cv::pow(centroid.x - p.x,2.) + cv::pow(centroid.y - p.y,2.)) < minDist)
            {
                std::cout<<"INVALIDATED"<<std::endl;
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

std::vector<std::shared_ptr<Edge>> findIncidentEdges(const std::vector<cv::Point2d>& shape, std::vector<std::shared_ptr<Edge>>& edges)
{
    cv::Moments mom = cv::moments(shape,false);
    cv::Point2d centroid = cv::Point2d( mom.m10/mom.m00 , mom.m01/mom.m00);  
    double outterRad = 0;
    for (const cv::Point2d& pt : shape)
    {
        if (twoPointDist(pt,centroid) > outterRad)
        {
            outterRad = twoPointDist(pt,centroid);
        }
    }
    double innerRad = outterRad; 

    for(int i = 1; i< shape.size();i++)
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

    std::vector<std::shared_ptr<Edge>> toRet;

    for(std::shared_ptr<Edge> e : edges)
    {
        cv::Vec4d line = e->line;
        cv::Point2d endPointL(line[0],line[1]);
        cv::Point2d endPointR(line[0],line[1]);
        cv::Point2d vec = endPointR-endPointL; 
        cv::Point2d centroidVec = centroid-endPointL;
        if(twoPointDist(endPointL,centroid)<=outterRad)
        {
            double angle = std::acos(pointDotProduct(vec,centroidVec)/(pointDotProduct(vec,vec)*pointDotProduct(centroidVec,centroidVec)));
            double proj = std::sin(angle)*pointDotProduct(centroidVec,centroidVec);
            if(proj < innerRad)
            {
                toRet.push_back(e);
            }
        }
    }

    
}