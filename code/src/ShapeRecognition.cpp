#include "ShapeRecognition.h"
//computes Nacken's metric for line segments
double nackenDist(cv::Vec4i s1,cv::Vec4i s2, bool consider_translation)
{
    cv::Point p_1_1,p_1_2,p_2_1,p_2_2;
    // making sure both lines consider their (x-component wise) endpoint to be the first one
    if(s1[0]>s1[2])
    {
        p_1_1 = cv::Point(s1[2],s1[3]);
        p_1_2 = cv::Point(s1[0],s1[1]);
    }
    else
    {
        p_1_2 = cv::Point(s1[2],s1[3]);
        p_1_1 = cv::Point(s1[0],s1[1]);
    }
    if(s2[0]>s2[2])
    {
        p_2_1 = cv::Point(s2[2],s2[3]);
        p_2_2 = cv::Point(s2[0],s2[1]);
    }
    else
    {
        p_2_2 = cv::Point(s2[2],s2[3]);
        p_2_1 = cv::Point(s2[0],s2[1]);
    }

    //calculating vectors
    cv::Point v_1 = p_1_2-p_1_1;
    cv::Point v_2 = p_2_2-p_2_1;
    
    //calculating line lengths
    double l_1,l_2;
    l_1 = cv::sqrt(v_1.x*v_1.x+v_1.y*v_1.y);
    l_2 = cv::sqrt(v_2.x*v_2.x+v_2.y*v_2.y); 

    std::cout << "l1: " << l_1 << std::endl << "l2: " << l_2 << std::endl;


    
    //calculating angles in rad
    double phi_1, phi_2, delta_phi;
    phi_1 = (double)cv::fastAtan2(v_1.y,v_1.x);
    phi_2 = (double)cv::fastAtan2(v_2.y,v_2.x);
    delta_phi = phi_2-phi_1;
    std::cout << "phi_1: " << phi_1 << std::endl << "phi_2: " << phi_2 << std::endl;


    //calculating standard deviations
    //sigma_l is not needed for the method proposed by other people
    double sigma_d, sigma_w, sigma_l;
    sigma_d = (3.*CV_PI/2.)/cv::sqrt(30*l_2);
    sigma_w = 5.;
    sigma_l = 2.*cv::sqrt(30.*l_2);
    // computing Nacken's metric for line segments
    // rotational component
    double rot_comp = (1./(cv::pow(CV_PI*cv::pow(sigma_d,2),1./4.))*cv::exp(-((phi_2-phi_1)/cv::pow(sigma_d,2.))));
    // translational component
    // calculating the centers of the lines
    cv::Point c_1,c_2,c_2_1,c_2_1_r;
    c_1 = p_1_1+v_1/2.;
    c_2 = p_2_1+v_2/2.;
    //difference between centers
    c_2_1 = c_1-c_2;
    std::cout<<"center distance: "<<cv::sqrt(cv::pow(c_2_1.x,2.)+cv::pow(c_2_1.y,2.)) << std::endl;
    // rotating everything to align s_2 with the x-axis
    c_2_1_r = cv::Point(c_1.x*cos(-phi_2)-c_1.y*sin(-phi_2),c_1.x*sin(-phi_2)+c_1.y*cos(-phi_2));
    double disp_comp = consider_translation ?
    (1/(cv::sqrt(CV_PI*cv::pow(sigma_l,2.)*cv::pow(sigma_w,2.))))*cv::exp(-cv::pow((c_2_1_r.x)/sigma_l,2.) - cv::pow((c_2_1_r.y)/sigma_w,2.))
    :(1/(cv::sqrt(CV_PI*cv::pow(sigma_w,2.))))*cv::exp(-((c_2_1_r.y)/cv::pow(sigma_w,2.)));
    
    
    std::cout << "r: " << rot_comp << std::endl << "t: " << disp_comp << std::endl;
    return rot_comp*disp_comp;
}