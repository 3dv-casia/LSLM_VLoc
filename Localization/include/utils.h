#ifndef RETRI_IMGS_H
#define RETRI_IMGS_H

// std
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "eigen3/Eigen/Eigen"
#include "boost/filesystem.hpp"
#include "boost/thread/mutex.hpp"

#include <opencv2/opencv.hpp>

#include "baseDataStruct.h"


using namespace std;

#define MIN_LINE_LENGTH_FACTOR 0.005f
#define Max_line_segments_ 3000

//////////////////////////////////////////////////////////////////////////////////////////////////
bool readNvmFile(const std::string &nvm_file, std::vector<CameraData> &all_image_data);
//////////////////////////////////////////////////////////////////////////////////////////////////
bool readGTPose(vector<CameraData> &db_image_data_vec, vector<CameraData> &query_image_data_vec, string &db_imgname_file, string &query_imgname_file, 
                    std::vector<CameraData> &all_image_data);
//////////////////////////////////////////////////////////////////////////////////////////////////
bool ReadLimap(const std::string &LineMapFile,const std::string &Line2DSegmentFile,std::vector<ImageData> &ImageDatas);
//////////////////////////////////////////////////////////////////////////////////////////////////
bool readRetriFile(const std::string &retri_file, std::vector<ImageData> &QueryImageData,int &query_img_num,int &top_num);
//////////////////////////////////////////////////////////////////////////////////////////////////
bool BuildMappingLineSegmentsTxt(std::vector<CameraData> &DBImageData,std::vector<ImageData> &ImageDatas,std::string &StoreMappingImg2DSegmentDir);
//////////////////////////////////////////////////////////////////////////////////////////////////
bool Build2D3DMatches(ImageData &OneDbImageData,std::string &lines_match_dir,string &QueryImgName_,string &DbImgName_,vector<Line3d> &Lines3D_match,vector<Line2d> &Lines2D_match);
//////////////////////////////////////////////////////////////////////////////////////////////////
bool LineGroup(const std::string &LineMatchTxt , std::vector<Line2d> &Lines2D_match);
//////////////////////////////////////////////////////////////////////////////////////////////////
void crossCheckMatching( cv::Ptr<cv::DescriptorMatcher>& descriptorMatcher,
                         const cv::Mat& descriptors1, const cv::Mat& descriptors2,
                         vector<cv::DMatch>& filteredMatches12, int knn );
//////////////////////////////////////////////////////////////////////////////////////////////////
void build2D3DLineCorrespondences(std::string &queryImgName,std::string &dbImgName,std::string &dbSegID_txt,
                                    std::string &queryseg_txt,std::string &_2D2Dmatchtxt,std::string &_2D3Dmatchtxt);
//////////////////////////////////////////////////////////////////////////////////////////////////
bool SiftMatch(const std::string &query_image_name, const std::string &db_image_name,std::string &siftMatchPath);
//////////////////////////////////////////////////////////////////////////////////////////////////
bool LSDLineDetect(const std::string &image_name,std::string &LSDLinesRestorePath);
//////////////////////////////////////////////////////////////////////////////////////////////////
bool readRetriFile(const std::string &retri_file, std::vector<std::string> &query_image_names, std::vector<std::vector<std::string>> &db_image_names,int &query_img_num,int &top_num);
//////////////////////////////////////////////////////////////////////////////////////////////////
bool read_DB_SegAndIndex_FromLineMap(
                    std::vector<pair<int,string>> &DBImgNameAndId,string &DBImageNamesFromRetri,
                    std::vector<std::vector<Eigen::Vector3d>> &LineMap_3DLineStartPoint,
                    std::vector<std::vector<Eigen::Vector3d>> &LineMap_3DLineEndPoint,
                    std::vector<std::vector<int>> &LineMap_camsIDs,
                    std::vector<std::vector<int>> &LineMap_segsIDs,
                    std::vector<std::vector<Eigen::Vector2d>> &LineMap_ps,
                    std::vector<std::vector<Eigen::Vector2d>> &LineMap_qs,
                    std::string &segID_restore_txt,
                    std::string &seg_restore_txt);
//////////////////////////////////////////////////////////////////////////////////////////////////
typedef Eigen::Matrix<double,3,3> Matrix3x3d;
bool Projection3DLineTo2DLine(string &QueryImgName,
                            vector<Line3d> &Lines3D_match,vector<Line2d> &Lines2D_match,
                            Eigen::Matrix3d &intrinsic_matrix,Eigen::MatrixXd &rotation_matrix,Eigen::Vector3d &translation_vector);

//////////////////////////////////////////////////////////////////////////////////////////////////

#endif // RETRI_IMGS_H



