#include "LineOptimization.h"




void LineLocEngine::ParameterizeCamera() {
    // Parameterize the camera.
    double* kvec_data = cam.Params().data();
    double* qvec_data = campose.qvec.data();
    double* tvec_data = campose.tvec.data();

    // We do not optimize for intrinsics
    problem_->SetParameterBlockConstant(kvec_data);

    ceres::LocalParameterization* quaternion_parameterization = new ceres::QuaternionParameterization;
    problem_->AddParameterBlock(qvec_data, 4, quaternion_parameterization);
    problem_->AddParameterBlock(tvec_data, 3);
}
void LineLocEngine::AddResiduals()
{
    for (int j = 0; j < l3ds.size(); j++) 
    {
        const Line3d &l3d = l3ds[j];
        const Line2d &l2d = l2ds[j];

        problem_=std::make_unique<ceres::Problem>();

        ceres::CostFunction* cost_function =ReprojectionVerticalAndHorizonDistanceResidual::Create(l3d, l2d); 
        
        problem_->AddResidualBlock(cost_function, nullptr,
                                    cam.Params().data(), campose.qvec.data(), campose.tvec.data());
    }
}
void LineLocEngine::SetUp() {

    AddResiduals();
    
    ParameterizeCamera();
}

bool LineLocEngine::Solve() {
    if (problem_->NumResiduals() == 0)
        return false;
    ceres::Solver::Options solver_options;
    solver_options.minimizer_progress_to_stdout = true;
    // solver_options.max_num_iterations = 100;
    // solver_options.max_linear_solver_iterations=200;
    // solver_options.max_num_consecutive_invalid_steps = 20;
    // solver_options.max_consecutive_nonmonotonic_steps = 20;
    // solver_options.num_threads = -1;
    solver_options.linear_solver_type = ceres::DENSE_QR;
    ceres::Solve(solver_options, problem_.get(), &summary_);
    if (solver_options.minimizer_progress_to_stdout) {
        std::cout << std::endl;
    }
    std::cout << summary_.BriefReport() << std::endl;
    std::cout << "Initial cost: " << GetInitialCost() << std::endl;
    std::cout << "Final cost: " << GetFinalCost() << std::endl;
    // std::cout << "R: " << std::endl << GetFinalR() << std::endl;
    // std::cout << "Q: " << std::endl << GetFinalQ() << std::endl;
    // std::cout << "T: " << std::endl << GetFinalT() << std::endl;
    return true;
}