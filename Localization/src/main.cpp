#include <Python.h>

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

#include "eigen3/Eigen/Eigen"
#include "boost/filesystem.hpp"
#include "boost/thread/mutex.hpp"

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "utils.h"
#include "P3L_RANSAC.h"
#include "LineOptimization.h"

using namespace std;

namespace FilePath
{
    //Cambridge - ShopFacade !!!Change the path to your own//
    std::string nvm_dir=".../Cambridge/ShopFacade/reconstruction.nvm";
    string Retri_dir=".../Cambridge/ShopFacade/output_hloc/pairs-query-netvlad5.txt";
    std::string Linemap_track_txt=".../Cambridge/ShopFacade/alltracks.txt";
    std::string Linemap_2Dline_dir=".../Cambridge/ShopFacade/line_detections/deeplsd/segments/";
    std::string Linemap_imglist_txt=".../Cambridge/ShopFacade/undistorted_images/original_image_list.txt";
    std::string dbimg_dir=".../Cambridge/ShopFacade/RetriOutput/train_imgfile.txt";
    std::string queryimg_dir=".../Cambridge/ShopFacade/RetriOutput/query_imgfile.txt";
    std::string lines_match_dir=".../Cambridge/ShopFacade/output_hloc/GlueStick/";
    std::string undistorted_img_dir=".../Cambridge/ShopFacade/undistorted_images/";
}

int main()
{
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    vector<CameraData> AllCameraData;
    bool flag1=readNvmFile(FilePath::nvm_dir,AllCameraData);
    if(flag1)
    {
        std::cout<<"read nvm file success!"<<std::endl;
    }
    else
    {
        std::cout<<"read nvm file failed!"<<std::endl;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    vector<CameraData> QueryCameraData;
    vector<CameraData> DBCameraData;
    bool flag2=readGTPose(DBCameraData,QueryCameraData,FilePath::dbimg_dir,FilePath::queryimg_dir,AllCameraData);
    if(flag2)
    {
        std::cout<<"read GT pose success!"<<std::endl;
    }
    else
    {
        std::cout<<"read GT pose failed!"<<std::endl;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<ImageData> LineMapData;
    LineMapData.resize(AllCameraData.size());
    bool flag3=ReadLimap(FilePath::Linemap_track_txt,FilePath::Linemap_2Dline_dir,LineMapData);
    if(flag3)
    {
        std::cout<<"read line map success!"<<std::endl;
    }
    else
    {
        std::cout<<"read line map failed!"<<std::endl;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<ImageData> QueryImageData;
    QueryImageData.resize(QueryCameraData.size());
    int query_num=QueryCameraData.size();
    int top_num=5;
    bool flag4=readRetriFile(FilePath::Retri_dir,QueryImageData,query_num,top_num);
    if(flag4)
    {
        std::cout<<"read retrieval file success!"<<std::endl;
    }
    else
    {
        std::cout<<"read retrieval file failed!"<<std::endl;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool flag5=BuildMappingLineSegmentsTxt(DBCameraData,LineMapData,FilePath::lines_match_dir);
    if(flag5)
    {
        std::cout<<"build mapping line segments txt success!"<<std::endl;
    }
    else
    {
        std::cout<<"build mapping line segments txt failed!"<<std::endl;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///!!!Change the path to your own///
    std::string command = "conda activate GlueStick && cd .../LSLM_VLoc/Localization/third-party/GlueStick/gluestick/ && python run.py";
    int result = system(command.c_str());
    if (result == 0) {
        std::cout << "Script executed successfully" << std::endl;
    } else {
        std::cout << "Script execution failed" << std::endl;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    vector<float>R_error_total;
    vector<float>T_error_total;
    vector<float>center_error_total;

    int failnum=0;
    int successnum=0;
    for(int i=0;i<query_num;i++)
    {
        vector<vector<Line3d>> Lines3D_match;
        vector<vector<Line2d>> Lines2D_match;
        for(int j=0;j<top_num;j++)
        {
            vector<Line3d> Lines3D_match_temp;
            vector<Line2d> Lines2D_match_temp;
            vector<int> ImgId_temp;
            vector<int> LineId_temp;
            string QueryImgName=QueryImageData[i].image_name;
            string DbImgName=QueryImageData[i].retrieved_image_names[j];
            int DBimgID=0;

            for(int k=0;k<DBCameraData.size();k++)
            {
                if(DBCameraData[k].image_name==DbImgName)
                {
                    DBimgID=DBCameraData[k].image_id;
                    break;
                }
            }
            ImageData OneDbImageData;
            for(int k=0;k<LineMapData.size();k++)
            {
                if(LineMapData[k].image_id==DBimgID)
                {
                    OneDbImageData=LineMapData[k];
                    break;
                }
            }
            Build2D3DMatches(OneDbImageData,FilePath::lines_match_dir,QueryImgName,DbImgName,
                    Lines3D_match_temp,Lines2D_match_temp);
            Lines3D_match.push_back(Lines3D_match_temp);
            Lines2D_match.push_back(Lines2D_match_temp);
        }

        int max_match_num=0;
        int max_match_index=0;
        for(int j=0;j<Lines3D_match.size();j++)
        {
            if(Lines3D_match[j].size()>max_match_num)
            {
                max_match_num=Lines3D_match[j].size();
                max_match_index=j;
            }
        }
        std::cout<<"----------------------------------------------------------------------------"<< std::endl;
        std::cout<<"query_image_names:"<<QueryImageData[i].image_name<<std::endl;
        std::cout<<"db_image_names:"<<QueryImageData[i].retrieved_image_names[max_match_index]<<std::endl;
        std::cout<<"max_match_num:"<<max_match_num<<std::endl;
        std::cout<<"max_match_index:"<<max_match_index<<std::endl;

        string img_path=FilePath::undistorted_img_dir+QueryImageData[i].image_name;
        string QImgName= QueryImageData[i].image_name.substr(0,QueryImageData[i].image_name.find_last_of("."));
        replace(QImgName.begin(),QImgName.end(),'/','_');
        string DImgName= QueryImageData[i].retrieved_image_names[max_match_index].substr(0,QueryImageData[i].retrieved_image_names[max_match_index].find_last_of("."));
        replace(DImgName.begin(),DImgName.end(),'/','_');
        string LineMatchtxt=FilePath::lines_match_dir+QImgName+DImgName+"_matchline.txt";
        string line_txt=LineMatchtxt;
        ///!!!Change the path to your own///
        std::string command = "conda activate VP && cd ../LSLM_VLoc/Localization/third-party/VP-Estimation-with-Prior-Gravity && python test.py "+img_path+" "+line_txt;
        int result = system(command.c_str());
        if (result == 0) {
            std::cout << "Script executed successfully" << std::endl;
        } else {
            std::cout << "Script execution failed" << std::endl;
        }

        Eigen::Matrix3d intrinsic_matrix_=Eigen::Matrix3d::Zero();
        cv::Mat query_image_temp=cv::imread(img_path,0);
        int query_image_width=query_image_temp.cols;
        int query_image_height=query_image_temp.rows;
        for(int j=0;j<QueryCameraData.size();j++)
        {
            if(QueryCameraData[j].image_name==QueryImageData[i].image_name)
            {
                intrinsic_matrix_<<QueryCameraData[j].focal,0,query_image_width/2,0,QueryCameraData[j].focal,query_image_height/2,0,0,1;
                break;
            }
        }
        int max_size=1600;
        float scale=1.0;
        if(query_image_width>max_size||query_image_height>max_size)
        {
            if(query_image_width>query_image_height)
            {
                scale=float(max_size)/float(query_image_width);
            }
            else
            {
                scale=float(max_size)/float(query_image_height);
            }
        }
        vector<Line3d> Lines3D_match_max=Lines3D_match[max_match_index];
        vector<Line2d> Lines2D_match_max=Lines2D_match[max_match_index];
       
        for(int j=0;j<Lines2D_match_max.size();j++)
        {
            Lines2D_match_max[j].start=Lines2D_match_max[j].start*scale;
            Lines2D_match_max[j].end=Lines2D_match_max[j].end *scale;
        }
        vector<Line2d> Lines2D_match_max_inNormalCamera;
        for(int j=0;j<Lines2D_match_max.size();j++)
        {
            Eigen::Vector3d start_point_inNormalCamera=Eigen::Vector3d::Zero();
            Eigen::Vector3d end_point_inNormalCamera=Eigen::Vector3d::Zero();
            start_point_inNormalCamera<<Lines2D_match_max[j].start(0),Lines2D_match_max[j].start(1),1;
            end_point_inNormalCamera<<Lines2D_match_max[j].end(0),Lines2D_match_max[j].end(1),1;

            start_point_inNormalCamera=intrinsic_matrix_.inverse()*start_point_inNormalCamera;
            end_point_inNormalCamera=intrinsic_matrix_.inverse()*end_point_inNormalCamera;

            Eigen::Vector2d start_point_inNormalCamera_temp=Eigen::Vector2d::Zero();
            Eigen::Vector2d end_point_inNormalCamera_temp=Eigen::Vector2d::Zero();
            start_point_inNormalCamera_temp<<start_point_inNormalCamera(0),start_point_inNormalCamera(1);
            end_point_inNormalCamera_temp<<end_point_inNormalCamera(0),end_point_inNormalCamera(1);

            Line2d line2d_temp;
            line2d_temp.start=start_point_inNormalCamera_temp;
            line2d_temp.end=end_point_inNormalCamera_temp;

            Lines2D_match_max_inNormalCamera.push_back(line2d_temp);
        }

        LineGroup(LineMatchtxt,Lines2D_match_max_inNormalCamera);

        Eigen::MatrixXd Pre_R_P3Lransac(3,3);
        Eigen::Vector3d Pre_t_P3Lransac;
        vector<int>inliers;
        bool flag6=P3L_Ransac(Lines3D_match_max,Lines2D_match_max_inNormalCamera,Pre_R_P3Lransac,Pre_t_P3Lransac,inliers);
        if(flag6==false)
        {
            failnum++;
            continue;
        }

        LineLocEngine lineLocEngine;
        vector<Line2d> lines2d_inliers_filtered;
        vector<Line3d> lines3d_inliers_filtered;
        double length_torlerance=0.7;
        for(int j=0;j<Lines2D_match_max.size();j++)
        {
            Eigen::Vector3d start_3d=Lines3D_match_max[j].start;
            Eigen::Vector3d end_3d=Lines3D_match_max[j].end;
            Eigen::Vector3d start_3d_in_camera=Pre_R_P3Lransac*start_3d+Pre_t_P3Lransac;
            Eigen::Vector3d end_3d_in_camera=Pre_R_P3Lransac*end_3d+Pre_t_P3Lransac;

            start_3d_in_camera=start_3d_in_camera/start_3d_in_camera(2);
            end_3d_in_camera=end_3d_in_camera/end_3d_in_camera(2);

            cv::Mat imagePoint1;
            cv::Mat imagePoint2;

            cv::Mat start_3d_in_camera_mat=(cv::Mat_<double>(3,1)<<start_3d_in_camera[0],start_3d_in_camera[1],start_3d_in_camera[2]);
            cv::Mat end_3d_in_camera_mat=(cv::Mat_<double>(3,1)<<end_3d_in_camera[0],end_3d_in_camera[1],end_3d_in_camera[2]);
            
            cv::Mat intrinsic_matrix_temp(3,3,CV_64FC1);
            intrinsic_matrix_temp.at<double>(0,0)=intrinsic_matrix_(0,0);
            intrinsic_matrix_temp.at<double>(0,1)=intrinsic_matrix_(0,1);
            intrinsic_matrix_temp.at<double>(0,2)=intrinsic_matrix_(0,2);
            intrinsic_matrix_temp.at<double>(1,0)=intrinsic_matrix_(1,0);
            intrinsic_matrix_temp.at<double>(1,1)=intrinsic_matrix_(1,1);
            intrinsic_matrix_temp.at<double>(1,2)=intrinsic_matrix_(1,2);
            intrinsic_matrix_temp.at<double>(2,0)=intrinsic_matrix_(2,0);
            intrinsic_matrix_temp.at<double>(2,1)=intrinsic_matrix_(2,1);
            intrinsic_matrix_temp.at<double>(2,2)=intrinsic_matrix_(2,2);

            cv::projectPoints(start_3d_in_camera_mat,cv::Mat::zeros(3,1,CV_64FC1),cv::Mat::zeros(3,1,CV_64FC1),intrinsic_matrix_temp,cv::Mat::zeros(4,1,CV_64FC1),imagePoint1);
            cv::projectPoints(end_3d_in_camera_mat,cv::Mat::zeros(3,1,CV_64FC1),cv::Mat::zeros(3,1,CV_64FC1),intrinsic_matrix_temp,cv::Mat::zeros(4,1,CV_64FC1),imagePoint2);

            Eigen::Vector2d start_2d_in_camera;
            Eigen::Vector2d end_2d_in_camera;
            start_2d_in_camera[0]=imagePoint1.at<double>(0,0);
            start_2d_in_camera[1]=imagePoint1.at<double>(0,1);
            end_2d_in_camera[0]=imagePoint2.at<double>(0,0);
            end_2d_in_camera[1]=imagePoint2.at<double>(0,1);

            if(start_2d_in_camera[0]>0&&start_2d_in_camera[0]<query_image_width&&start_2d_in_camera[1]>0&&start_2d_in_camera[1]<query_image_height
            &&end_2d_in_camera[0]>0&&end_2d_in_camera[0]<query_image_width&&end_2d_in_camera[1]>0&&end_2d_in_camera[1]<query_image_height)
            {
                if((start_2d_in_camera-end_2d_in_camera).norm()>2 )
                {
                    lines3d_inliers_filtered.push_back(Lines3D_match_max[j]);
                    lines2d_inliers_filtered.push_back(Lines2D_match_max[j]);
                }
            }
        }
        if(lines3d_inliers_filtered.size()<3)
        {
            std::cout<<"line num< 3, not work!"<<std::endl;
            continue;
        }
        lineLocEngine.Initialize(lines3d_inliers_filtered,lines2d_inliers_filtered,intrinsic_matrix_,Pre_R_P3Lransac,Pre_t_P3Lransac);
        lineLocEngine.SetUp();
        lineLocEngine.Solve();
         
        ////////////evaluate the result/////////////
        Eigen::MatrixXd GT_R=Eigen::Matrix3d::Zero();
        Eigen::Vector3d GT_T=Eigen::Vector3d::Zero();
        Eigen::Vector3d GT_center=Eigen::Vector3d::Zero();
        for(int j=0;j<QueryCameraData.size();j++)
        {
            if(QueryCameraData[j].image_name==QueryImageData[i].image_name)
            {
                GT_R=QueryCameraData[j].rotation;
                GT_T=QueryCameraData[j].translation;
                GT_center=QueryCameraData[j].center;
                break;
            }
        }
        float trace=(GT_R.transpose()*lineLocEngine.GetFinalR()).trace();
        float cos_theta2 =std::max(-1.0f,std::min(1.0f,float((trace-1)/2)));
        float e_R=std::abs(acos(cos_theta2))*180.0/3.1415926;
        std::cout<<"R_error:"<<e_R<<endl;

        Eigen::Vector3d T_e = GT_T - lineLocEngine.GetFinalT();
        float t_error = T_e.norm();
        std::cout<<"t_error:"<<t_error<<endl;

        Eigen::Vector3d camera_center=-(lineLocEngine.GetFinalR().transpose())*lineLocEngine.GetFinalT();
        Eigen::Vector3d camera_center_error=GT_center-camera_center;
        float camera_center_error_norm=camera_center_error.norm();
        std::cout<<"position error:"<<camera_center_error_norm<<endl;
    }
    return 0;
}