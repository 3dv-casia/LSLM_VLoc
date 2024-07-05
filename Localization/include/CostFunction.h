#ifndef CostFunction_h
#define CostFunction_h

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

#include <ceres/ceres.h>
#include <ceres/rotation.h>

#include "baseDataStruct.h"

//////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void WorldToImage(const T* kvec, const T u, const T v, T* x, T* y) {
    const T f1 = kvec[0];
    const T f2 = kvec[1];
    const T c1 = kvec[2];
    const T c2 = kvec[3];

    *x = f1 * u + c1;
    *y = f2 * v + c2;
}
template <typename T>
inline void WorldToPixel(const T* kvec, const T* qvec, const T* tvec, const T* xyz, T* xy) {
    T projection[3];
    ceres::QuaternionRotatePoint(qvec, xyz, projection);
    projection[0] += tvec[0];
    projection[1] += tvec[1];
    projection[2] += tvec[2];

    projection[0] /= projection[2]; // u
    projection[1] /= projection[2]; // v
    WorldToImage(kvec, projection[0], projection[1], &xy[0], &xy[1]);
}
//////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
void Ceres_2DVerticalAndHorizonDistance(const T s2d1[2], const T e2d1[2], const T s2d2[2], const T e2d2[2], T* res) 
{
    T a = e2d2[1] - s2d2[1];
    T b = s2d2[0] - e2d2[0];
    T c = e2d2[0] * s2d2[1] - s2d2[0] * e2d2[1];
    T d = ceres::sqrt(a * a + b * b);
    T dist1=ceres::abs(a * s2d1[0] + b * s2d1[1] + c) / d;
    T dist2=ceres::abs(a * e2d1[0] + b * e2d1[1] + c) / d;
    res[0] = (dist1 + dist2) / T(2.0);

    T mid1[2] = {(s2d1[0] + e2d1[0]) / T(2.0), (s2d1[1] + e2d1[1]) / T(2.0)};
    T mid2[2] = {(s2d2[0] + e2d2[0]) / T(2.0), (s2d2[1] + e2d2[1]) / T(2.0)};
    T k_2_T = (e2d2[0] - s2d2[0]) / (s2d2[1] - e2d2[1]);
    T b_2_T = mid2[1]-k_2_T*mid2[0];
    res[1] = ceres::abs(k_2_T*mid1[0]-mid1[1]+b_2_T)/ceres::sqrt(k_2_T*k_2_T+T(1));   
}

struct ReprojectionVerticalAndHorizonDistanceResidual
{
    public:
    ReprojectionVerticalAndHorizonDistanceResidual(const Line3d& line3d, const Line2d& line2d ):line3d_(line3d), line2d_(line2d){}

    static ceres::CostFunction* Create(const Line3d& line3d, const Line2d& line2d) 
    {
        return new ceres::AutoDiffCostFunction<ReprojectionVerticalAndHorizonDistanceResidual, 2, 4, 4, 3>(new ReprojectionVerticalAndHorizonDistanceResidual(line3d, line2d));
    }


    template <typename T>
    bool operator()(const T* const kvec, const T* const qvec, const T* const tvec, T* residuals) const
    {
        // reproject to 2d
        T s3d[3] = {T(line3d_.start(0)), T(line3d_.start(1)), T(line3d_.start(2))};
        T e3d[3] = {T(line3d_.end(0)), T(line3d_.end(1)), T(line3d_.end(2))};

        T s2d_reproj[2], e2d_reproj[2];
        WorldToPixel<T>(kvec, qvec, tvec, s3d, s2d_reproj);
        WorldToPixel<T>(kvec, qvec, tvec, e3d, e2d_reproj);

        T s2d[2] = {T(line2d_.start(0)), T(line2d_.start(1))};
        T e2d[2] = {T(line2d_.end(0)), T(line2d_.end(1))};

        Ceres_2DVerticalAndHorizonDistance(s2d_reproj, e2d_reproj, s2d, e2d,residuals);

        return true;

    }
    protected:
    Line3d line3d_;
    Line2d line2d_;
};
/////////////////////////////////////////////////////////////////////////////////////////



#endif /* LineOptimize_h */
