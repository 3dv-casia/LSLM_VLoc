#ifndef baseDataStruct_h
#define baseDataStruct_h

#include <cmath>
#include <set>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <colmap/base/camera.h>
#include <colmap/base/camera_models.h>
#include <colmap/base/pose.h>

#include "eigen3/Eigen/Eigen"

using V2D = Eigen::Vector2d;
using V3D = Eigen::Vector3d;
using V4D = Eigen::Vector4d;
using M3D = Eigen::Matrix3d;

//inline V3D homogeneous(const V2D& v2d) { return V3D(v2d(0), v2d(1), 1.0); }

const double EPS = 1e-12;

class Line2d {
public:
    Line2d() {}

    Line2d(V2D start_, V2D end_)
    {
        start = start_; 
        end = end_;
    }
    V2D start, end;

    double length() const {return (start - end).norm();}
    V2D midpoint() const {return 0.5 * (start + end);}
    V2D direction() const {return (end - start).normalized();}
    V2D perp_direction() const {V2D dir = direction(); return V2D(dir[1], -dir[0]); }

    int LineType; //0-Horizontal,1-Vertical,2-Oblique

     // 0,1,2
};

class Line3d {
public:
    Line3d() {}
    Line3d(V3D start_, V3D end_) 
    {
        start = start_; 
        end = end_;
    }
    V3D start, end;

    double length() const {return (start - end).norm();}
    V3D midpoint() const {return 0.5 * (start + end);}
    V3D direction() const {return (end - start).normalized();}

};

class CameraData
{
    public:
    std::string image_name;
    int image_id;
    float focal;
    Eigen::Vector4d rotation_qwxyz;
    Eigen::Matrix3d rotation;
    Eigen::Vector3d translation;
    Eigen::Vector3d center;
    float distortion;
};

struct Matched2D3DLine 
{
    Line3d Line3d_;
    int Line2d_id;
    Line2d Line2d_;
};

class ImageData
{
    public:
    std::string image_name;
    std::vector<std::string> retrieved_image_names;
    int image_id;
    std::vector<Matched2D3DLine> Matched2D3DLinesVec;
};

class Camera: public colmap::Camera {
public:
    Camera() {}
    
    Camera(int model_id, M3D K, int cam_id=-1, std::pair<int, int> hw=std::make_pair<int, int>(-1, -1))
    {    
        std::vector<double> params;
        if (model_id == 0) {
            // SIMPLE_PINHOLE
            SetModelIdFromName("SIMPLE_PINHOLE");
            params.push_back(K(0, 0));
            params.push_back(K(0, 2));
            params.push_back(K(1, 2));
        }
        else if (model_id == 1) {
            // PINHOLE
            SetModelIdFromName("PINHOLE");
            params.push_back(K(0, 0));
            params.push_back(K(1, 1));
            params.push_back(K(0, 2));
            params.push_back(K(1, 2));
        }
        else
            throw std::runtime_error("model initialized with K should be either SIMPLE_PINHOLE or PINHOLE");
        SetParams(params);
        if (cam_id != -1)
            SetCameraId(cam_id);
        if (hw.first != -1 && hw.second != -1) {
            SetHeight(hw.first);
            SetWidth(hw.second);
        }
    }

    bool operator ==(const Camera&cam)
    {
        if (CameraId() != cam.CameraId())
            return false;
        if (ModelId() != cam.ModelId())
            return false;
        if (h() != cam.h())
            return false;
        if (w() != cam.w())
            return false;
        std::vector<double> params = Params();
        std::vector<double> params_cam = cam.Params();
        for (int i = 0; i < params.size(); ++i) {
            if (params[i] != params_cam[i])
                return false;
        }
        return true;
    }

    void resize(const size_t width, const size_t height) { Rescale(width, height); }
    M3D K() const { return CalibrationMatrix(); }
    M3D K_inv() const { return K().inverse(); }
    int h() const { return Height(); }
    int w() const { return Width(); }
    std::vector<double> params() const { return Params(); }
};

class CameraPose {
public:
    CameraPose() {}
    CameraPose(V4D qqvec, V3D ttvec): qvec(qqvec.normalized()), tvec(ttvec) {}
    CameraPose(M3D R, V3D T): tvec(T) { qvec = colmap::RotationMatrixToQuaternion(R); }
    CameraPose(const CameraPose& campose): qvec(campose.qvec), tvec(campose.tvec) {}

    V4D qvec = V4D(1., 0., 0., 0.);
    V3D tvec = V3D::Zero();

    M3D R() const { return colmap::QuaternionToRotationMatrix(qvec); }
    V3D T() const { return tvec; }

    V3D center() const { return -R().transpose() * T(); }
};





#endif /* baseDataStruct_h */