#include "utils.h"
#include "eigen3/Eigen/Eigen"

//////////////////////////////////////////////////////////////////////////////////////////////////
bool readNvmFile(const std::string &nvm_file, std::vector<CameraData> &all_image_data)
{
    std::ifstream file(nvm_file.c_str());
    if (!file.is_open())
    {
        std::cerr << "Could not open file " << nvm_file << std::endl;
        return false;
    }

    std::string nvm_line;
    std::getline(file,nvm_line); // ignore first line...
    std::getline(file,nvm_line); // ignore second line...

    std::getline(file,nvm_line);
    std::stringstream nvm_stream(nvm_line);
    int num_cameras;
    nvm_stream >> num_cameras;

    if(num_cameras == 0)
    {
        std::cerr << "No aligned cameras in NVM file!" << std::endl;
        return false;
    }

    // Read cameras
    all_image_data.resize(num_cameras);
    for (int i = 0; i < num_cameras; ++i)
    {
        std::getline(file, nvm_line);

        // image filename
        std::string filename;

        double focal_length,qx,qy,qz,qw;
        double Cx,Cy,Cz,dist;

        nvm_stream.str("");
        nvm_stream.clear();
        nvm_stream.str(nvm_line);
        nvm_stream >> filename >> focal_length >> qw >> qx >> qy >> qz;
        nvm_stream >> Cx >> Cy >> Cz >> dist;

        Eigen::Vector4d R_qwxyz(qw,qx,qy,qz);
        all_image_data[i].rotation_qwxyz = R_qwxyz;
        all_image_data[i].focal = focal_length;
        all_image_data[i].image_name = filename;
        all_image_data[i].image_id = i;
        all_image_data[i].center = Eigen::Vector3d(Cx,Cy,Cz);
        all_image_data[i].distortion = dist;
        Eigen::Matrix3d R;
        Eigen::Quaterniond q(qw,qx,qy,qz);
        R = q.normalized().toRotationMatrix();
        Eigen::Vector3d C(Cx,Cy,Cz);
        Eigen::Vector3d t = -R*C;
        all_image_data[i].rotation = R;
        all_image_data[i].translation = t;
    }
    file.close();
    return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
bool readGTPose(vector<CameraData> &db_image_data_vec, vector<CameraData> &query_image_data_vec, string &db_imgname_file, string &query_imgname_file, 
                    std::vector<CameraData> &all_image_data)
{
    CameraData db_image_data;
    CameraData query_image_data;

    std::ifstream db_imgfile(db_imgname_file);
    if(!db_imgfile.is_open())
    {
        std::cout<<"open db_imgfile failed!"<<std::endl;
        return -1;
    }
    std::string db_imgname;
    vector<string> db_imgnames;
    while(getline(db_imgfile,db_imgname))
    {
        db_imgnames.push_back(db_imgname);
    }
    db_imgfile.close();

    std::ifstream query_imgfile(query_imgname_file);
    if(!query_imgfile.is_open())
    {
        std::cout<<"open query_imgfile failed!"<<std::endl;
        return -1;
    }
    std::string query_imgname;
    vector<string> query_imgnames;
    while(getline(query_imgfile,query_imgname))
    {
        query_imgnames.push_back(query_imgname);
    }
    query_imgfile.close();

    for(int i=0;i<db_imgnames.size();i++)
    {
        for(int j=0;j<all_image_data.size();j++)
        {
            if(db_imgnames[i]==all_image_data[j].image_name)
            {
                db_image_data_vec.push_back(all_image_data[j]);
            }
        }
    }
    for(int i=0;i<query_imgnames.size();i++)
    {
        for(int j=0;j<all_image_data.size();j++)
        {
            if(query_imgnames[i]==all_image_data[j].image_name)
            {
                query_image_data_vec.push_back(all_image_data[j]);
            }
        }
    }
    return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
bool ReadLimap(const std::string &LineMapFile,const std::string &Line2DSegmentFile,std::vector<ImageData> &ImageDatas)
{
    ifstream fin(LineMapFile.c_str());
    if(!fin)
    {
        cerr<<"Cannot open file "<<LineMapFile<<endl;
        return false;
    }
    int num_lines;
    fin>>num_lines;

    int Line3D_ID,Num2DLines,NumImages;
    Eigen::Vector3d Line3DStartPoint,Line3DEndPoint;
    vector<int> ImageIDs,SegmentIDs;

    for(int i=0;i<num_lines;i++)
    {
        fin>>Line3D_ID>>Num2DLines>>NumImages;
        fin>>Line3DStartPoint(0)>>Line3DStartPoint(1)>>Line3DStartPoint(2);
        fin>>Line3DEndPoint(0)>>Line3DEndPoint(1)>>Line3DEndPoint(2);

        Line3d line3d;
        line3d.start=Line3DStartPoint;
        line3d.end=Line3DEndPoint;

        ImageIDs.resize(Num2DLines);
        SegmentIDs.resize(Num2DLines);

        for(int j=0;j<Num2DLines;j++)
        {
            fin>>ImageIDs[j];
        }
        for(int j=0;j<Num2DLines;j++)
        {
            fin>>SegmentIDs[j];
        }
        for(int j=0;j<Num2DLines;j++)
        {
            int ImageID=ImageIDs[j];
            int SegmentID=SegmentIDs[j];

            Matched2D3DLine Matched2D3DLine_temp;
            Matched2D3DLine_temp.Line2d_id=SegmentID;
            Matched2D3DLine_temp.Line3d_=line3d;

            ImageDatas[ImageID].Matched2D3DLinesVec.push_back(Matched2D3DLine_temp);
        }     
    }
    fin.close();

    for(int i=0;i<ImageDatas.size();i++)
    {
        std::string imageName=Line2DSegmentFile+"segments_"+to_string(i)+".txt";
        ifstream Line2DSegmentFileTxt(imageName.c_str());
        if(!Line2DSegmentFileTxt)
        {
            cerr<<"Cannot open file "<<imageName<<endl;
            return false;
        }
        int num_segments;
        Line2DSegmentFileTxt>>num_segments;
        vector<Line2d> Line2d_vec(num_segments);
        for(int j=0;j<num_segments;j++)
        {
            Line2DSegmentFileTxt>>Line2d_vec[j].start(0)>>Line2d_vec[j].start(1)>>Line2d_vec[j].end(0)>>Line2d_vec[j].end(1);
        }
        for(int j=0;j<ImageDatas[i].Matched2D3DLinesVec.size();j++)
        {
            int SegmentID=ImageDatas[i].Matched2D3DLinesVec[j].Line2d_id;
            ImageDatas[i].Matched2D3DLinesVec[j].Line2d_=Line2d_vec[SegmentID];
        }
        Line2DSegmentFileTxt.close();
    }
    cout<<"ReadLimap success!"<<endl;
    return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
bool readRetriFile(const std::string &retri_file, std::vector<ImageData> &QueryImageData,int &query_img_num,int &top_num)
{
    std::ifstream fin(retri_file.c_str());
    if(!fin)
    {
        std::cerr<<"Cannot open file "<<retri_file<<std::endl;
        return false;
    }
    std::string line;

    for(int i=0;i<query_img_num;i++)
    {
        string query_image_name;
        string db_image_name;
        vector<string> one_db_image_names;
        for(int j=0;j<top_num;j++)
        {
            getline(fin,line);
            std::stringstream ss(line);
            ss>>query_image_name>>db_image_name;
            one_db_image_names.push_back(db_image_name);
        }
        QueryImageData[i].image_id=i;
        QueryImageData[i].image_name=query_image_name;
        QueryImageData[i].retrieved_image_names=one_db_image_names;
    }
    cout<<"readRetriFile success!"<<endl;
    return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
bool BuildMappingLineSegmentsTxt(std::vector<CameraData> &DBImageData,std::vector<ImageData> &ImageDatas,std::string &StoreMappingImg2DSegmentDir)
{
    for(int i=0;i<DBImageData.size();i++)
    {
        string img_name=DBImageData[i].image_name;
        img_name=img_name.substr(img_name.find_last_of("/")+1);
        string MappingLineSegmentsTxt=StoreMappingImg2DSegmentDir+img_name+".txt";
        ofstream MappingLineSegmentsTxtFile(MappingLineSegmentsTxt.c_str());
        if(!MappingLineSegmentsTxtFile)
        {
            cerr<<"Cannot open file "<<MappingLineSegmentsTxt<<endl;
            return false;
        }
        for(int j=0;j<ImageDatas.size();j++)
        {
            if(ImageDatas[j].image_id==DBImageData[i].image_id)
            {
                for(int k=0;k<ImageDatas[j].Matched2D3DLinesVec.size();k++)
                {
                    MappingLineSegmentsTxtFile<<ImageDatas[j].Matched2D3DLinesVec[k].Line2d_id<<ImageDatas[j].Matched2D3DLinesVec[k].Line2d_.start(0)<<" "<<ImageDatas[j].Matched2D3DLinesVec[k].Line2d_.start(1)<<" "<<ImageDatas[j].Matched2D3DLinesVec[k].Line2d_.end(0)<<" "<<ImageDatas[j].Matched2D3DLinesVec[k].Line2d_.end(1)<<endl;
                }
            }
        }
        MappingLineSegmentsTxtFile.close();
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////
bool Build2D3DMatches(ImageData &OneDbImageData,
                    std::string &lines_match_dir,string &QueryImgName_,string &DbImgName_,
                    vector<Line3d> &Lines3D_match,vector<Line2d> &Lines2D_match)
{
    string QImgName= QueryImgName_.substr(0,QueryImgName_.find_last_of("."));
    replace(QImgName.begin(),QImgName.end(),'/','_');
    string DImgName= DbImgName_.substr(0,DbImgName_.find_last_of("."));
    replace(DImgName.begin(),DImgName.end(),'/','_');
    string LineMatchtxt=lines_match_dir+QImgName+DImgName+"_matchline.txt";

    vector<int>SegmentID;
    vector<Line2d> Line2d_inquery;

    ifstream LineMatchtxtFile(LineMatchtxt.c_str());
    if(!LineMatchtxtFile)
    {
        cerr<<"Cannot open file "<<LineMatchtxt<<endl;
        return false;
    }
    while(!LineMatchtxtFile.eof())
    {
        int SegmentID_;
        Line2d Line2d_inquery_;
        LineMatchtxtFile>>SegmentID_>>Line2d_inquery_.start(0)>>Line2d_inquery_.start(1)>>Line2d_inquery_.end(0)>>Line2d_inquery_.end(1);
        SegmentID.push_back(SegmentID_);
        Line2d_inquery.push_back(Line2d_inquery_);
    }
    LineMatchtxtFile.close();
    SegmentID.pop_back();
    Line2d_inquery.pop_back();

    for(int i=0;i<SegmentID.size();i++)
    {
        int SegmentID_=SegmentID[i];
        for(int j=0;j<OneDbImageData.Matched2D3DLinesVec.size();j++)
        {
            if(OneDbImageData.Matched2D3DLinesVec[j].Line2d_id==SegmentID_)
            {
                Lines3D_match.push_back(OneDbImageData.Matched2D3DLinesVec[j].Line3d_);
                Lines2D_match.push_back(Line2d_inquery[i]);      
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////
bool LineGroup(const std::string &LineMatchTxt , std::vector<Line2d> &Lines2D_match)
{
    ifstream LineMatchTxtFile(LineMatchTxt.c_str());
    if(!LineMatchTxtFile)
    {
        cerr<<"Cannot open file "<<LineMatchTxt<<endl;
        return false;
    }
    vector<int> GroupID;
    while(!LineMatchTxtFile.eof())
    {
        int SegmentID_;
        int GroupID_;
        Line2d Line2d_inquery_;
        LineMatchTxtFile>>SegmentID_>>Line2d_inquery_.start(0)>>Line2d_inquery_.start(1)>>Line2d_inquery_.end(0)>>Line2d_inquery_.end(1)>>GroupID_;
        GroupID.push_back(GroupID_);
    }

    for(int i=0;i<Lines2D_match.size();i++)
    {
        Lines2D_match[i].LineType=GroupID[i];
    }

    LineMatchTxtFile.close();
    return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
void crossCheckMatching( cv::Ptr<cv::DescriptorMatcher>& descriptorMatcher,
                         const cv::Mat& descriptors1, const cv::Mat& descriptors2,
                         vector<cv::DMatch>& filteredMatches12, int knn)
{
    filteredMatches12.clear();
    vector<vector<cv::DMatch> > matches12, matches21;
    descriptorMatcher->knnMatch( descriptors1, descriptors2, matches12, knn );
    descriptorMatcher->knnMatch( descriptors2, descriptors1, matches21, knn );
    for( size_t m = 0; m < matches12.size(); m++ )
    {
        bool findCrossCheck = false;
        for( size_t fk = 0; fk < matches12[m].size(); fk++ )
        {
            cv::DMatch forward = matches12[m][fk];

            for( size_t bk = 0; bk < matches21[forward.trainIdx].size(); bk++ )
            {
                cv::DMatch backward = matches21[forward.trainIdx][bk];
                if( backward.trainIdx == forward.queryIdx )
                {
                    filteredMatches12.push_back(forward);
                    findCrossCheck = true;
                    break;
                }
            }
            if( findCrossCheck ) break;
        }
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////
bool SiftMatch(const std::string &query_image_name, const std::string &db_image_name,std::string &siftMatchPath)
{
    // cv::Mat img1=cv::imread( query_image_name, 0 );
    // cv::Mat img2=cv::imread( db_image_name, 0 );

    // vector<cv::KeyPoint> keypoints1,keypoints2;
    // cv::Mat descriptors1,descriptors2;
    // //cv::Ptr<cv::FeatureDetector> detector=cv::SiftFeatureDetector::create();
    // //cv::Ptr<cv::DescriptorExtractor> descriptor=cv::SiftDescriptorExtractor::create();
    // cv::Ptr<cv::DescriptorMatcher> matcher=cv::DescriptorMatcher::create("BruteForce");
    // detector->detect(img1,keypoints1);
    // detector->detect(img2,keypoints2);

    // cout<<"keypoints1:"<<keypoints1.size()<<endl;
    // cout<<"keypoints2:"<<keypoints2.size()<<endl;
    // descriptor->compute(img1,keypoints1,descriptors1);
    // descriptor->compute(img2,keypoints2,descriptors2);
    // vector<cv::DMatch> filteredMatches;
    // crossCheckMatching( matcher, descriptors1, descriptors2, filteredMatches,1);
    // cout<<"good matches:"<<filteredMatches.size()<<endl;

    // std::vector<Eigen::Vector2d> query_match_points;
    // std::vector<Eigen::Vector2d> db_match_points;
    // for(int i=0;i<filteredMatches.size();i++)
    // {
    //     cv::Point2f p1=keypoints1[filteredMatches[i].queryIdx].pt;
    //     cv::Point2f p2=keypoints2[filteredMatches[i].trainIdx].pt;
    //     Eigen::Vector2d query_match_point(p1.x,p1.y);
    //     Eigen::Vector2d db_match_point(p2.x,p2.y);
    //     query_match_points.push_back(query_match_point);
    //     db_match_points.push_back(db_match_point);
    // }
    // std::ofstream matchPoints(siftMatchPath);
    // if (!matchPoints.is_open())
    // {
    //     std::cerr << "Could not open file " << siftMatchPath << std::endl;
    //     return false;
    // }
    // for(int i=0;i<filteredMatches.size();i++)
    // {
    //     matchPoints<<query_match_points[i][0]<<" "<<query_match_points[i][1]<<" "<<db_match_points[i][0]<<" "<<db_match_points[i][1]<<std::endl;
    // }
    // matchPoints.close();
    // return true;   
}
//////////////////////////////////////////////////////////////////////////////////////////////////

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
                    std::string &seg_restore_txt)
{
    std::vector<Eigen::Vector3d> db_image_3DLineStartPoint;
    std::vector<Eigen::Vector3d> db_image_3DLineEndPoint;
    std::vector<int> db_image_segIDs;
    std::vector<Eigen::Vector2d> db_image_seg_startPoints;
    std::vector<Eigen::Vector2d> db_image_seg_endPoints;

    int db_image_id=0;
    for(int i=0;i<DBImgNameAndId.size();i++)
    {
        if("db/"+DBImgNameAndId[i].second==DBImageNamesFromRetri)
        {
            db_image_id=DBImgNameAndId[i].first;
            break;
        }
    }
    for(int i=0;i<LineMap_camsIDs.size();i++)
    {
        for(int j=0;j<LineMap_camsIDs[i].size();j++)
        {
            if(LineMap_camsIDs[i][j]==db_image_id)
            {
                db_image_3DLineStartPoint.push_back(LineMap_3DLineStartPoint[i][0]);
                db_image_3DLineEndPoint.push_back(LineMap_3DLineEndPoint[i][0]);
                db_image_segIDs.push_back(LineMap_segsIDs[i][j]);
                db_image_seg_startPoints.push_back(LineMap_ps[i][j]);
                db_image_seg_endPoints.push_back(LineMap_qs[i][j]);
            }
        }
    }
    ofstream fout(segID_restore_txt.c_str());
    if(!fout)
    {
        cerr<<"Cannot open file "<<segID_restore_txt<<endl;
        return false;
    }
    for(int i=0;i<db_image_segIDs.size();i++)
    {
        fout<<db_image_segIDs[i]<<endl;
    }
    ofstream fout2(seg_restore_txt.c_str());
    if(!fout2)
    {
        cerr<<"Cannot open file "<<seg_restore_txt<<endl;
        return false;
    }
    for(int i=0;i<db_image_seg_startPoints.size();i++)
    {
        fout2<<db_image_seg_startPoints[i](0)<<" "<<db_image_seg_startPoints[i](1)<<" "<<db_image_seg_endPoints[i](0)<<" "<<db_image_seg_endPoints[i](1)<<endl;
    }
    fout.close();
    fout2.close();

    return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
void build2D3DLineCorrespondences(std::string &queryImgName,std::string &dbImgName,std::string &dbSegID_txt,
                                    std::string &queryseg_txt,std::string &_2D2Dmatchtxt,std::string &_2D3Dmatchtxt)
{
    std::vector<int> queryImg2D2DMatchLines_index;
    std::vector<int> dbImg2D2DMatchLines_index;
    std::vector<int> dbImg2D3DMatchLines_index;
    std::vector<int> dbSegID;
    ifstream fin(dbSegID_txt);
    if(!fin)
    {
        std::cout<<"read dbSegID_txt failed!"<<std::endl;
    }
    else
    {
        int segID;
        while(fin>>segID)
        {
            dbSegID.push_back(segID);
        }
    }
    fin.close();

    ifstream fin2(_2D2Dmatchtxt);
    if(!fin2)
    {
        std::cout<<"read _2D2Dmatchtxt failed!"<<std::endl;
    }
    else
    {
        int queryImg2D2DMatchLines_index_temp;
        int dbImg2D2DMatchLines_index_temp;
        while(fin2>>queryImg2D2DMatchLines_index_temp>>dbImg2D2DMatchLines_index_temp)
        {
            queryImg2D2DMatchLines_index.push_back(queryImg2D2DMatchLines_index_temp);
            dbImg2D2DMatchLines_index.push_back(dbImg2D2DMatchLines_index_temp);
        }
    }
    fin2.close();
    for(int i=0;i<dbImg2D2DMatchLines_index.size();i++)
    {
        int index=dbImg2D2DMatchLines_index[i]-1;
        dbImg2D3DMatchLines_index.push_back(dbSegID[index]);
    }
    std::vector<Eigen::Vector2d> queryImg2D2DMatchLines_startPoints;
    std::vector<Eigen::Vector2d> queryImg2D2DMatchLines_endPoints;
    ifstream fin3(queryseg_txt);
    if(!fin3)
    {
        std::cout<<"read queryseg_txt failed!"<<std::endl;
    }
    else
    {
        double x1,y1,x2,y2;
        while(fin3>>x1>>y1>>x2>>y2)
        {
            Eigen::Vector2d startP(x1,y1);
            Eigen::Vector2d endP(x2,y2);
            queryImg2D2DMatchLines_startPoints.push_back(startP);
            queryImg2D2DMatchLines_endPoints.push_back(endP);
        }
    }

    std::vector<Eigen::Vector2d> queryImg2D3DMatchLines_startPoints;
    std::vector<Eigen::Vector2d> queryImg2D3DMatchLines_endPoints;
    for(int i=0;i<queryImg2D2DMatchLines_index.size();i++)
    {
        int index=queryImg2D2DMatchLines_index[i]-1;
        Eigen::Vector2d start_point=queryImg2D2DMatchLines_startPoints[index];
        Eigen::Vector2d end_point=queryImg2D2DMatchLines_endPoints[index];
        queryImg2D3DMatchLines_startPoints.push_back(start_point);
        queryImg2D3DMatchLines_endPoints.push_back(end_point);
    }

    ofstream fout(_2D3Dmatchtxt.c_str(),ios::app);
    if(!fout)
    {
        cerr<<"Cannot open file "<<_2D3Dmatchtxt<<endl;
    }
    fout<<queryImgName<<" "<<dbImgName<<" ";
    for(int i=0;i<dbImg2D3DMatchLines_index.size();i++)
    {
        fout<<dbImg2D3DMatchLines_index[i]<<" "<<queryImg2D3DMatchLines_startPoints[i](0)<<" "<<queryImg2D3DMatchLines_startPoints[i](1)<<" "<<queryImg2D3DMatchLines_endPoints[i](0)<<" "<<queryImg2D3DMatchLines_endPoints[i](1)<<" ";
    }
    fout<<endl;
    fout.close();
}
//////////////////////////////////////////////////////////////////////////////////////////////////
bool Projection3DLineTo2DLine(string &QueryImgName,
                            vector<Line3d> &Lines3D_match,vector<Line2d> &Lines2D_match,
                            Eigen::Matrix3d &intrinsic_matrix,Eigen::MatrixXd &rotation_matrix,Eigen::Vector3d &translation_vector)
{
    std::cout<<"Projection3DLineTo2DLine"<<std::endl;
    std::cout<<"QueryImgName: "<<QueryImgName<<std::endl;
    cv::Mat image=cv::imread(".../pathway/"+QueryImgName);
    
    if(!image.data)
    {
        std::cout<<"read image failed!"<<std::endl;
        return false;
    }
    std::vector<Eigen::Vector2d> _2DLines_start_point;
    std::vector<Eigen::Vector2d> _2DLines_end_point;
    for(int i=0;i<Lines3D_match.size();i++)
    {
        Eigen::Vector3d start_point_3D=Lines3D_match[i].start;
        Eigen::Vector3d end_point_3D=Lines3D_match[i].end;

        Eigen::Vector3d start_point_3D_temp=rotation_matrix*start_point_3D+translation_vector;
        Eigen::Vector3d end_point_3D_temp=rotation_matrix*end_point_3D+translation_vector;

        cv::Mat imagePoint1;
        cv::Mat imagePoint2;

        cv::Mat start_point_2D_temp(3,1,CV_64FC1);
        start_point_2D_temp.at<double>(0,0)=start_point_3D_temp(0);
        start_point_2D_temp.at<double>(1,0)=start_point_3D_temp(1);
        start_point_2D_temp.at<double>(2,0)=start_point_3D_temp(2);
        cv::Mat end_point_2D_temp(3,1,CV_64FC1);
        end_point_2D_temp.at<double>(0,0)=end_point_3D_temp(0);
        end_point_2D_temp.at<double>(1,0)=end_point_3D_temp(1);
        end_point_2D_temp.at<double>(2,0)=end_point_3D_temp(2);

        cv::Mat intrinsic_matrix_temp(3,3,CV_64FC1);
        intrinsic_matrix_temp.at<double>(0,0)=intrinsic_matrix(0,0);
        intrinsic_matrix_temp.at<double>(0,1)=intrinsic_matrix(0,1);
        intrinsic_matrix_temp.at<double>(0,2)=intrinsic_matrix(0,2);
        intrinsic_matrix_temp.at<double>(1,0)=intrinsic_matrix(1,0);
        intrinsic_matrix_temp.at<double>(1,1)=intrinsic_matrix(1,1);
        intrinsic_matrix_temp.at<double>(1,2)=intrinsic_matrix(1,2);
        intrinsic_matrix_temp.at<double>(2,0)=intrinsic_matrix(2,0);
        intrinsic_matrix_temp.at<double>(2,1)=intrinsic_matrix(2,1);
        intrinsic_matrix_temp.at<double>(2,2)=intrinsic_matrix(2,2);

        cv::projectPoints(start_point_2D_temp,cv::Mat::zeros(3,1,CV_64FC1),cv::Mat::zeros(3,1,CV_64FC1),intrinsic_matrix_temp,cv::Mat::zeros(4,1,CV_64FC1),imagePoint1);
        cv::projectPoints(end_point_2D_temp,cv::Mat::zeros(3,1,CV_64FC1),cv::Mat::zeros(3,1,CV_64FC1),intrinsic_matrix_temp,cv::Mat::zeros(4,1,CV_64FC1),imagePoint2);
        
        Eigen::Vector2d start_point_2D_temp_;
        Eigen::Vector2d end_point_2D_temp_;

        start_point_2D_temp_(0)=imagePoint1.at<double>(0,0);
        start_point_2D_temp_(1)=imagePoint1.at<double>(0,1);
        end_point_2D_temp_(0)=imagePoint2.at<double>(0,0);
        end_point_2D_temp_(1)=imagePoint2.at<double>(0,1);
        _2DLines_start_point.push_back(start_point_2D_temp_);
        _2DLines_end_point.push_back(end_point_2D_temp_);  

    }
    for(int i=0;i<_2DLines_start_point.size();i++)
    {
        Eigen::Vector2d start_point=_2DLines_start_point[i];
        Eigen::Vector2d end_point=_2DLines_end_point[i];
        cv::Point start_point_temp(start_point(0),start_point(1));
        cv::Point end_point_temp(end_point(0),end_point(1));
        cv::line(image,start_point_temp,end_point_temp,cv::Scalar(0,0,255),2);
    }
    cv::imwrite(".../pathway/"+QueryImgName.substr(QueryImgName.find_last_of("/")+1),image);


    for(int i=0;i<Lines2D_match.size();i++)
    {
        Eigen::Vector2d start_point_2D=Lines2D_match[i].start;
        Eigen::Vector2d end_point_2D=Lines2D_match[i].end;

        cv::Point start_point_temp(start_point_2D(0),start_point_2D(1));
        cv::Point end_point_temp(end_point_2D(0),end_point_2D(1));

        cv::line(image,start_point_temp,end_point_temp,cv::Scalar(0,255,0),2);
    }
    cv::imwrite(".../pathway/_"+QueryImgName.substr(QueryImgName.find_last_of("/")+1),image);
    return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
// bool readIntrinsicFile(const std::string &intrinsic_file,std::vector<QueryImage> &query_image)
// {
//     std::ifstream intrinsicFile(intrinsic_file);
//     if (!intrinsicFile.is_open())
//     {
//         std::cerr << "Could not open file " << intrinsic_file << std::endl;
//         return false;
//     }

//     std::string line;
//     while (std::getline(intrinsicFile, line))
//     {
//         std::istringstream iss(line);
//         std::string img_name,camera_model;
//         int width, height;
//         double f, cx, cy,r;
//         iss >> img_name >> camera_model >> width >> height >> f >> cx >> cy >> r;

//         for(int i=0;i<query_image.size();i++)
//         {
//             if(query_image[i].image_name==img_name)
//             {
//                 query_image[i].width=width;
//                 query_image[i].height=height;
//                 query_image[i].K<<f,0,cx,0,f,cy,0,0,1;
//                 break;
//             }
//         }
//     }
//     intrinsicFile.close();
//     cout<<"readIntrinsicFile success!"<<endl;
//     return true;
// }
//////////////////////////////////////////////////////////////////////////////////////////////////
// bool readlimap_originimg_list(const std::string &limap_originimg_list,std::vector<MappingImage> &Mapping_images)
// {
//     std::ifstream limap_originimg_listFile(limap_originimg_list);
//     if (!limap_originimg_listFile.is_open())
//     {
//         std::cerr << "Could not open file " << limap_originimg_list << std::endl;
//         return false;
//     }
//     std::string line;
//     getline(limap_originimg_listFile,line);

//     for(int i=0;i<Mapping_images.size();i++)
//     {
//         getline(limap_originimg_listFile,line);
//         std::istringstream iss(line);
//         std::string img_id,img_name;
//         getline(iss,img_id,',');
//         getline(iss,img_name);
//         int img_id_=atoi(img_id.c_str());
//         Mapping_images[i].image_id=img_id_;
//         Mapping_images[i].image_name=img_name;
//     }
   
//     limap_originimg_listFile.close();
//     cout<<"readlimap_originimg_list success!"<<endl;
//     return true;
// }
//////////////////////////////////////////////////////////////////////////////////////////////////
// bool rename_img(const std::string &img_dir,std::vector<MappingImage> &Mapping_images)
// {
//     for(int i=0;i<Mapping_images.size();i++)
//     {
//         string img_name=Mapping_images[i].image_name;
//         string img_name_new=img_name.substr(img_name.find_last_of("/")+1);
//         img_name_new=img_name_new.substr(0,img_name_new.find_last_of("."))+".png";
//         string img_name_new_=img_dir+img_name_new;
//         string img_id=to_string(Mapping_images[i].image_id);
//         string img_id_new;
//         for(int j=0;j<8-img_id.size();j++)
//         {
//             img_id_new+="0";
//         }
//         img_id_new+=img_id;
//         string img_name_old=img_dir+"image"+img_id_new+".png";

//         rename(img_name_old.c_str(),img_name_new_.c_str());
//     }
//     cout<<"rename_img success!"<<endl;
//     return true;

// }