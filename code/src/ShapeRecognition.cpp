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
                cv::Point2d proj_v_i = proj_p_i_2-proj_p_i_1;
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
    return ret;
}

void getShapes(std::vector<cv::Point2d> points, std::vector<cv::Vec4i> lines){
    double corner_influence_radius = 10;
    std::vector<cv::Vec4i> valid_edges;
    for(cv::Point2d point : points){
        
    }
    
    for(cv::Vec4i line : lines)
    {

    }
}