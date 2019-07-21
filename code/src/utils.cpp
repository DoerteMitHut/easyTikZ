#include "utils.h"
#include <limits>

double pointNorm(const cv::Point2d& p)
{
    return cv::sqrt(cv::pow(p.x,2)+cv::pow(p.y,2));
}

double pointDotProduct(const cv::Point2d &u,const cv::Point2d &v)
{
    double s = (u.x*v.x)+(u.y*v.y);
    return s; 
}

double twoPointDist(const cv::Point2d &u,const cv::Point2d &v)
{
    return std::sqrt(std::pow(u.x-v.x,2)+std::pow(u.y-v.y,2));
}

double twoPointDist(const cv::Point& p,const cv::Point& q)
{
    return std::sqrt(std::pow(p.x-q.x,2)+std::pow(p.y-q.y,2));
}

double innerRad(const cv::Vec3f& circle)
{
    //return 80% of the circles radius as the innerRad
    return circle[2]*1.3;
}
double innerRad(const std::vector<cv::Point2d>& polygon,const cv::Point2d& centroid)
{
    //return 80% of the center-distance of the edge center closest to the centroid
    std::vector<cv::Point2d>::const_iterator it;
    double minRad = std::numeric_limits<double>::max();
    for(it = polygon.begin(); it!=polygon.end(); ++it)
    {
        cv::Point2d middle = (*it - *((it+1)==polygon.end() ? polygon.begin() : it+1))/2.;
        if(twoPointDist(middle,centroid)<minRad){
            minRad = twoPointDist(middle,centroid);
        }
    }
    return minRad;
}

double outerRad(const cv::Vec3f& circle)
{
    //return 120% of the circles radius as the innerRad
    return 3*circle[2];
}
double outerRad(const std::vector<cv::Point2d>& polygon,const cv::Point2d& centroid)
{
    //return 120% of the center distance of the vertex farthest from the centroid
    std::vector<cv::Point2d>::const_iterator it;
    double maxRad = std::numeric_limits<double>::min();
    for(it = polygon.begin(); it!=polygon.end(); ++it)
    {
        if(twoPointDist(*it,centroid)>maxRad){
            maxRad = twoPointDist(*it,centroid);
        }
    }
    return 1.5*maxRad;
}

void writeConfigFile(bool TIKZ_ENV_FLAG , bool TEX_DOC_FLAG , bool COSMETICS_FLAG , bool LABEL_FLAG , Alignments ALIGNMENT_MODE , std::pair<float,float> GRID_SIZE , double CORNER_MERGE_THRESHOLD , int LINE_SUPPORT_THRESHOLD)
    
{
    
    std::string delimiter = ":";
    std::ofstream filestream(".easyTikZ_config");
    filestream << "TIKZ_ENV_FLAG" << delimiter <<std::to_string(TIKZ_ENV_FLAG)<<std::endl;
    filestream << "TEX_DOC_FLAG" << delimiter <<std::to_string(TEX_DOC_FLAG)<<std::endl;
    filestream << "COSMETICS_FLAG" << delimiter <<std::to_string(COSMETICS_FLAG)<<std::endl;
    filestream << "LABEL_FLAG" << delimiter <<std::to_string(LABEL_FLAG)<<std::endl;
    filestream << "ALIGNMENT_MODE" << delimiter <<std::to_string((int)ALIGNMENT_MODE)<<std::endl;
    filestream << "GRID_SIZE_H" << delimiter <<std::to_string(GRID_SIZE.first)<<std::endl;
    filestream << "GRID_SIZE_V" << delimiter <<std::to_string(GRID_SIZE.second)<<std::endl;
    filestream << "CORNER_MERGE_THRESHOLD" << delimiter <<std::to_string(CORNER_MERGE_THRESHOLD)<<std::endl;
    filestream << "LINE_SUPPORT_THRESHOLD" << delimiter <<std::to_string(LINE_SUPPORT_THRESHOLD)<<std::endl;
}

bool processCLArguments(int argc, char** argv, cv::Mat& img, bool& TIKZ_ENV_FLAG , bool& TEX_DOC_FLAG , bool& COSMETICS_FLAG , bool& SET_DEFAULT_PARAMS , Alignments& ALIGNMENT_MODE , std::pair<float,float>& GRID_SIZE , double& CORNER_MERGE_THRESHOLD , int& LINE_SUPPORT_THRESHOLD)
{
    
    std::vector<std::string> args;
    for (int i = 1; i < argc; i++)
    {
        args.emplace_back(argv[i]);
    }

    for (unsigned int i = 0; i < args.size(); ++i)
    {
        if (i == 0)
        {
            img =  cv::imread(argv[1]);
            if(img.empty())
            {
                std::cout<<"No image data!\n";
                return false;
            }
        }
        else if (args[i] == "--tikz" || args[i] == "-t")
        {
            TIKZ_ENV_FLAG = true;
        }
        else if (args[i] == "--tex" || args[i] == "-T")
        {
            TIKZ_ENV_FLAG = true;
            TEX_DOC_FLAG = true;
        }
        else if (args[i] == "--cosmetics" || args[i] == "-c")
        {
            COSMETICS_FLAG = true;
        }
        else if (args[i] == "--size-alignment" || args[i] == "-s")
        {
            ALIGNMENT_MODE = SIZE_ALIGNMENT;
        }
        else if (args[i] == "--manual-alignment" || args[i] == "-m")
        {
            ALIGNMENT_MODE = MANUAL_ALIGNMENT;
            GRID_SIZE = std::pair<float,float>(atof(args[i+1].c_str()),atof(args[i+2].c_str()));
            i+=2;
        }
        else if (args[i] == "--corner-merge-threshold" || args[i] == "-C")
        {
            CORNER_MERGE_THRESHOLD = atof(args[i+1].c_str());
            i++;
        }
        else if (args[i] == "--line-support-threshold" || args[i] == "-L")
        {
            LINE_SUPPORT_THRESHOLD = atoi(args[i+1].c_str());
            i++;
        }
        else if (args[i] == "--set-default" || args[i] == "-S")
        {
            SET_DEFAULT_PARAMS = true;
        }
        else
        {
            std::cerr << "unkown option '" << args[i] << "'" << std::endl;
            std::cerr <<"usage: easytikz <Image_Path>"<<std::endl; 
            std::cerr<<"[--tikz | t]"<<std::endl;
            std::cerr<<"[--tex | -T]"<<std::endl;
            std::cerr<<"[(--manual-alignment | -M) <horiz. grid size> <vert. grid size>]"<<std::endl;
            std::cerr<<"[(--corner-merge-threshold | -C) <max dist of line ends>]"<<std::endl;
            std::cerr<<"[(--line-support-threshold | -L) <min votes for line>]"<<std::endl;
            std::cerr<<"[--set-default | -S]"<<std::endl;
            return false;
        }
    }
    return true;
}

void readConfigFile(bool& TIKZ_ENV_FLAG , bool& TEX_DOC_FLAG , bool& COSMETICS_FLAG , bool& LABEL_FLAG , Alignments& ALIGNMENT_MODE , std::pair<float,float>& GRID_SIZE , double& CORNER_MERGE_THRESHOLD , int& LINE_SUPPORT_THRESHOLD)
{
    std::ifstream filestream;
    filestream.open(".easyTikZ_config");
    std::string line;
    while (filestream>>line)
    {
        std::cout<< "read line from config file:\n"<<line<<std::endl;
        int delimiterIndex = line.find(":");
        std::string paramName = line.substr(0,delimiterIndex);
        if(paramName == "TIKZ_ENV_FLAG")
        {
            TIKZ_ENV_FLAG = (bool)atoi(line.substr(delimiterIndex+1,line.length()-1).c_str());
        }
        else if(paramName == "TEX_DOC_FLAG")
        {
            TEX_DOC_FLAG = (bool)atoi(line.substr(delimiterIndex+1,line.length()-1).c_str());
        }
        else if(paramName == "COSMETICS_FLAG")
        {
            COSMETICS_FLAG = (bool)atoi(line.substr(delimiterIndex+1,line.length()-1).c_str());
        }
        else if(paramName == "LABEL_FLAG")
        {
            LABEL_FLAG = (bool)atoi(line.substr(delimiterIndex+1,line.length()-1).c_str());
        }
        else if(paramName == "ALIGNMENT_MODE")
        {
            ALIGNMENT_MODE = Alignments(atoi(line.substr(delimiterIndex+1,line.length()-1).c_str()));
        }
        else if(paramName == "GRID_SIZE_H")
        {
            GRID_SIZE.first = atof(line.substr(delimiterIndex+1,line.length()-1).c_str());
        }
        else if(paramName == "GRID_SIZE_V")
        {
            GRID_SIZE.second = atof(line.substr(delimiterIndex+1,line.length()-1).c_str());
        }
        else if(paramName == "CORNER_MERGE_THRESHOLD")
        {
            CORNER_MERGE_THRESHOLD = atof(line.substr(delimiterIndex+1,line.length()-1).c_str());
        }
        else if(paramName == "LINE_SUPPORT_THRESHOLD")
        {
             LINE_SUPPORT_THRESHOLD = atoi(line.substr(delimiterIndex+1,line.length()-1).c_str());
        }
    }
}