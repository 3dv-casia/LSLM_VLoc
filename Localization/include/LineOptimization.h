#ifndef LineOptimization_h
#define LineOptimization_h

#include <ceres/ceres.h>

#include "baseDataStruct.h"
#include "CostFunction.h"

#include <colmap/util/logging.h>
#include <colmap/util/threading.h>
#include <colmap/util/misc.h>
#include <colmap/optim/bundle_adjustment.h>


class LineLocEngine
{
    protected:
    std::vector<Line3d> l3ds;
    std::vector<Line2d> l2ds;

    std::vector<V3D> Point_3Ds;
    std::vector<V2D> Point_2Ds;

    V4D cam_kvec;
    Camera cam;
    CameraPose campose;

    // set up ceres problem
    void ParameterizeCamera();
    virtual void AddResiduals();

    public:
    LineLocEngine() {}

    void Initialize(const std::vector<Line3d>& l3ds, const std::vector<Line2d>& l2ds, M3D K, M3D R, V3D T) {
        this->l3ds = l3ds;
        this->l2ds = l2ds;
        this->cam = Camera(colmap::PinholeCameraModel::model_id, K);
        this->campose = CameraPose(R, T);
    }
    void Initialize(const std::vector<Line3d>& l3ds, const std::vector<Line2d>& l2ds, V4D kvec, V4D qvec, V3D tvec) {
        this->l3ds = l3ds;
        this->l2ds = l2ds;
        M3D K = M3D::Zero();
        K(0, 0) = kvec(0);
        K(1, 1) = kvec(1);
        K(0, 2) = kvec(2);
        K(1, 2) = kvec(3);
        this->cam = Camera(colmap::PinholeCameraModel::model_id, K);
        this->campose = CameraPose(qvec, tvec);
    }
    void SetUp();
    bool Solve();

    // output
    M3D GetFinalR() const { return campose.R(); }
    V4D GetFinalQ() const { return campose.qvec; }
    V3D GetFinalT() const { return campose.T(); }
    double GetInitialCost() const { return std::sqrt(summary_.initial_cost / summary_.num_residuals_reduced); }
    double GetFinalCost() const { return std::sqrt(summary_.final_cost / summary_.num_residuals_reduced); }
    bool IsSolutionUsable() const { return summary_.IsSolutionUsable(); }

    // ceres
    std::unique_ptr<ceres::Problem> problem_;
    ceres::Solver::Summary summary_;
};

#endif /* LineOptimization_h */