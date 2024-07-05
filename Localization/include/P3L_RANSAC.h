#ifndef P3L_RANSAC_H
#define P3L_RANSAC_H

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include <random>
#include <set>

#include "baseDataStruct.h"

#include "eigen3/Eigen/Eigen"
#include "eigen3/Eigen/Dense"
#include "eigen3/Eigen/Geometry"
#include <unsupported/Eigen/Polynomials>

struct R_and_T
{
    Eigen::Matrix3d R;
    Eigen::Vector3d T;
};

#define MIN_LINES 3

Eigen::MatrixXd kroneckerProduct(const Eigen::MatrixXd& A, const Eigen::MatrixXd& B);

void getProjNorm(Eigen::MatrixXd &p1,Eigen::MatrixXd &p2,Eigen::MatrixXd &nl);

void getVP(Eigen::MatrixXd &P1,Eigen::MatrixXd &P2,
            Eigen::MatrixXd &V,Eigen::MatrixXd &P);

std::vector<int> getSubset(int n, int m);

std::vector<R_and_T> P3L(Eigen::MatrixXd &sp1,Eigen::MatrixXd &sp2,Eigen::MatrixXd &sP1_w,Eigen::MatrixXd &sP2_w);

bool P3L_Ransac(std::vector<Line3d> &_3D_Line_points_inWorld,std::vector<Line2d> &_2D_Line_points_in_images,
            Eigen::MatrixXd &R,Eigen::Vector3d &T,std::vector<int>& inliers);

#endif