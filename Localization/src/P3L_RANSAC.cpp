#include "P3L_RANSAC.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Eigen::MatrixXd kroneckerProduct(const Eigen::MatrixXd& A, const Eigen::MatrixXd& B)
{
    int m = A.rows(), n = A.cols(), p = B.rows(), q = B.cols();
    Eigen::MatrixXd C(m*p, n*q);
    for (int i = 0; i < m; i++)
    {
        for (int k = 0; k < n; k++)
        {
            C.block(i*p, k*q, p, q) = A(i, k)*B;
        }
    }
    return C;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void getProjNorm(Eigen::MatrixXd &p1,Eigen::MatrixXd &p2,Eigen::MatrixXd &nl)
{
    int n=p1.cols();
    Eigen::MatrixXd d1(3,n);
    Eigen::MatrixXd d2(3,n);
    
    d1.block(0,0,2,n)=p1;
    d1.row(2)=Eigen::MatrixXd::Ones(1,n);
    d2.block(0,0,2,n)=p2;
    d2.row(2)=Eigen::MatrixXd::Ones(1,n);

    Eigen::MatrixXd c(3,n);
    for(int i=0;i<n;i++)
    {
        Eigen::Vector3d d1_col_i=d1.col(i);
        Eigen::Vector3d d2_col_i=d2.col(i);
        Eigen::Vector3d c_col_i=d1_col_i.cross(d2_col_i);
        c.col(i)=c_col_i;
    }

    Eigen::MatrixXd D_norm(1,n);
    D_norm=(c.cwiseProduct(c)).colwise().sum().cwiseSqrt().cwiseInverse();
    Eigen::MatrixXd one=Eigen::MatrixXd::Ones(3,1);
    Eigen::MatrixXd D_norm_one=kroneckerProduct(one,D_norm);
    nl=c.cwiseProduct(D_norm_one);
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<int> getSubset(int n, int m)
{
    std::vector<int> index;
    std::set<int> index_set;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, n - 1);
    while (index_set.size() < m)
    {
        index_set.insert(dis(gen));
    }
    for (auto it = index_set.begin(); it != index_set.end(); it++)
    {
        index.push_back(*it);
    }
    return index;
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void getVP(Eigen::MatrixXd &P1,Eigen::MatrixXd &P2,
            Eigen::MatrixXd &V,Eigen::MatrixXd &P)
{
    V=P2-P1;
    int n=P1.cols();

    Eigen::MatrixXd D_norm(1,n);
    D_norm=(V.cwiseProduct(V)).colwise().sum().cwiseSqrt().cwiseInverse();

    Eigen::MatrixXd K=kroneckerProduct(Eigen::MatrixXd::Ones(3,1),D_norm);

    V=V.cwiseProduct(K);
    P=(P1+P2)*0.5;
}
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
int sign(T val) {
    return (T(0) < val) - (val < T(0));
}
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<R_and_T> P3L(Eigen::MatrixXd &sp1,Eigen::MatrixXd &sp2,Eigen::MatrixXd &sP1_w,Eigen::MatrixXd &sP2_w)
{
    int n=sp1.cols();
    Eigen::MatrixXd nl(3,n);
    getProjNorm(sp1,sp2,nl);
    Eigen::MatrixXd V(3,n);
    Eigen::MatrixXd P(3,n);
    getVP(sP1_w,sP2_w,V,P);

    Eigen::Matrix3d xs(3,3);
    Eigen::Matrix3d xe(3,3);
    xs.block(0,0,2,sp1.cols())=sp1;
    xs.row(2)=Eigen::MatrixXd::Ones(1,sp1.cols());
    xe.block(0,0,2,sp2.cols())=sp2;
    xe.row(2)=Eigen::MatrixXd::Ones(1,sp2.cols());

    Eigen::MatrixXd nl_nl(3,2*n);
    nl_nl.block(0,0,3,n)=nl;
    nl_nl.block(0,n,3,n)=nl;
    Eigen::MatrixXd M1=kroneckerProduct(Eigen::MatrixXd::Ones(1,3),nl_nl.transpose());
    Eigen::MatrixXd P1_W_P2_W(3,2*n);
    P1_W_P2_W.block(0,0,3,n)=sP1_w;
    P1_W_P2_W.block(0,n,3,n)=sP2_w;
    Eigen::MatrixXd M2=kroneckerProduct(P1_W_P2_W.transpose(),Eigen::MatrixXd::Ones(1,3));
    Eigen::MatrixXd A=M1.cwiseProduct(M2);
    Eigen::MatrixXd B=nl_nl.transpose();
    Eigen::MatrixXd BTB=B.transpose()*B;

    Eigen::FullPivLU<Eigen::MatrixXd> lu(BTB);
    if(lu.rank()<3)
    {
        std::cout<<"rank(BTB)<3"<<std::endl;
        return std::vector<R_and_T>();
    }
    Eigen::MatrixXd BA=-BTB.inverse()*B.transpose()*A;

    Eigen::Vector3d nc1=xs.col(0).cross(xe.col(0));
    nc1=nc1/nc1.norm();
    Eigen::Vector3d nc2=xs.col(1).cross(xe.col(1));
    nc2=nc2/nc2.norm();
    Eigen::Vector3d nc3=xs.col(2).cross(xe.col(2));
    nc3=nc3/nc3.norm();
    
    Eigen::Vector3d Vw1=V.col(0);
    Vw1=Vw1/Vw1.norm();
    Eigen::Vector3d Vw2=V.col(1);
    Vw2=Vw2/Vw2.norm();
    Eigen::Vector3d Vw3=V.col(2);
    Vw3=Vw3/Vw3.norm();

    Eigen::Vector3d Xm=nc1.cross(Vw1);
    Xm=Xm/Xm.norm();
    Eigen::Vector3d Ym=nc1;
    Eigen::Vector3d Vm1=Xm.cross(Ym);
    Vm1=Vm1/Vm1.norm();

    Eigen::MatrixXd Rot(3,3);
    Rot.col(0)=Xm;
    Rot.col(1)=Ym;
    Rot.col(2)=Vm1;
    //Rot=Rot.transpose();

    Eigen::MatrixXd Vw1new=Rot.transpose()*Vw1;
    double cospsi=0;
    cospsi=Vw1new(2);

    double sinpsi=0;
    sinpsi=sqrt(1-cospsi*cospsi);

    Eigen::MatrixXd Rmw(3,3);
    Rmw<<1,0,0,
        0,cospsi,-sinpsi,
        0,sinpsi,cospsi;

    nc1=Rot.transpose()*nc1;
    nc2=Rot.transpose()*nc2;
    nc3=Rot.transpose()*nc3;

    Vm1=Rmw*Rot.transpose()*Vw1;
    if(1-abs(Vm1(2))>1e-5)
    {
        Rmw.transposeInPlace();
    }
    Eigen::MatrixXd Vm2=Rmw*(Rot.transpose()*Vw2);
    Eigen::MatrixXd Vm3=Rmw*(Rot.transpose()*Vw3);

    double A2=Vm2(0);
    double B2=Vm2(1);
    double C2=Vm2(2);
    double A3=Vm3(0);
    double B3=Vm3(1);
    double C3=Vm3(2);

    double x2=nc2(0);
    double y2=nc2(1);
    double z2=nc2(2);
    double x3=nc3(0);
    double y3=nc3(1);
    double z3=nc3(2);

    double u11 = -z2*A2*y3*B3 +y2*B2*z3*A3;
    double u12 = -y2*A2*z3*B3 +z2*B2*y3*A3;
    double u13 = -y2*B2*z3*B3 +z2*B2*y3*B3 +y2*A2*z3*A3 -z2*A2*y3*A3;
    double u14 = -y2*B2*x3*C3 +x2*C2*y3*B3;
	double u15 = x2*C2*y3*A3 -y2*A2*x3*C3;
	double u21 = -x2*A2*y3*B3 +y2*B2*x3*A3;
	double u22 = -y2*A2*x3*B3 +x2*B2*y3*A3;
	double u23 = x2*B2*y3*B3 -y2*B2*x3*B3 -x2*A2*y3*A3 +y2*A2*x3*A3;
	double u24 = y2*B2*z3*C3 -z2*C2*y3*B3;
	double u25 = y2*A2*z3*C3 -z2*C2*y3*A3;
	double u31 = -x2*A2*z3*A3 +z2*A2*x3*A3;
	double u32 = -x2*B2*z3*B3 +z2*B2*x3*B3;
	double u33 = x2*A2*z3*B3 -z2*A2*x3*B3 +x2*B2*z3*A3 -z2*B2*x3*A3;
	double u34 = z2*A2*z3*C3 +x2*A2*x3*C3 -z2*C2*z3*A3 -x2*C2*x3*A3;
	double u35 = -z2*B2*z3*C3 -x2*B2*x3*C3 +z2*C2*z3*B3 +x2*C2*x3*B3;
	double u36 = -x2*C2*z3*C3 +z2*C2*x3*C3;

    double a4= u11*u11+u12*u12-u13*u13-2*u11*u12+u21*u21+u22*u22-u23*u23-2*u21*u22-u31*u31-u32*u32+u33*u33+2*u31*u32;
	double a3= 2*(u11*u14-u13*u15-u12*u14+u21*u24-u23*u25-u22*u24-u31*u34+u33*u35+u32*u34);
	double a2= -2*u12*u12+u13*u13+u14*u14-u15*u15+2*u11*u12-2*u22*u22+u23*u23+u24*u24-u25*u25+2*u21*u22+2*u32*u32-u33*u33-u34*u34+u35*u35-2*u31*u32-2*u31*u36+2*u32*u36;
	double a1= 2*(u12*u14+u13*u15+u22*u24+u23*u25-u32*u34-u33*u35-u34*u36);
	double a0= u12*u12+u15*u15+u22*u22+u25*u25-u32*u32-u35*u35-u36*u36-2*u32*u36;
	double b3= 2*(u11*u13-u12*u13+u21*u23-u22*u23-u31*u33+u32*u33);
	double b2= 2*(u11*u15-u12*u15+u13*u14+u21*u25-u22*u25+u23*u24-u31*u35+u32*u35-u33*u34);
	double b1= 2*(u12*u13+u14*u15+u22*u23+u24*u25-u32*u33-u34*u35-u33*u36);
	double b0= 2*(u12*u15+u22*u25-u32*u35-u35*u36);

    double d0= a0*a0-b0*b0;
	double d1= 2*(a0*a1-b0*b1);
	double d2= a1*a1+2*a0*a2+b0*b0-b1*b1-2*b0*b2;
	double d3= 2*(a0*a3+a1*a2+b0*b1-b1*b2-b0*b3);
	double d4= a2*a2+2*a0*a4+2*a1*a3+b1*b1+2*b0*b2-b2*b2-2*b1*b3;
	double d5= 2*(a1*a4+a2*a3+b1*b2+b0*b3-b2*b3);
	double d6= a3*a3+2*a2*a4+b2*b2-b3*b3+2*b1*b3;
	double d7= 2*(a3*a4+b2*b3);
	double d8= a4*a4+b3*b3;

    Eigen::VectorXd coeffs(9);
    coeffs << d8, d7, d6, d5, d4, d3, d2, d1, d0;
    Eigen::PolynomialSolver<double,Eigen::Dynamic> solver;
    solver.compute(coeffs);
    Eigen::PolynomialSolver<double,Eigen::Dynamic>::RootsType roots = solver.roots();
    std::vector<double> real_roots;
    for (int i = 0; i < roots.rows(); i++)
    {
        if (std::abs(roots(i).imag()) < 0.01)
        {
            real_roots.push_back(roots(i).real());
        }
    }

    if (real_roots.size() == 0)
    {
        std::cout << "no solution" << std::endl;
        return std::vector<R_and_T>();
    }

    std::vector<R_and_T> sol;
    for (int i = 0; i < real_roots.size(); i++)
    {
        double cosphi = real_roots[i];
        int sign1 = sign(a4 * cosphi * cosphi * cosphi * cosphi + a3 * cosphi * cosphi * cosphi + a2 * cosphi * cosphi + a1 * cosphi + a0);
        int sign2 = sign(b3 * cosphi * cosphi * cosphi + b2 * cosphi * cosphi + b1 * cosphi + b0);
        double sinphi = -sign1 * sign2 * sqrt(std::abs(1 - cosphi * cosphi));

        double N1 = u11 * cosphi * cosphi + u12 * sinphi * sinphi + u13 * cosphi * sinphi + u14 * cosphi + u15 * sinphi;
        double N2 = u21 * cosphi * cosphi + u22 * sinphi * sinphi + u23 * cosphi * sinphi + u24 * cosphi + u25 * sinphi;
        double N3 = u31 * cosphi * cosphi + u32 * sinphi * sinphi + u33 * cosphi * sinphi + u34 * cosphi + u35 * sinphi + u36;

        double costheta = N1 / N3;
        double sintheta = N2 / N3;
        double normlize = 1 / sqrt(costheta * costheta + sintheta * sintheta);
        costheta = costheta * normlize;
        sintheta = sintheta * normlize;

        Eigen::Matrix3d R1;
        R1 << costheta, 0, sintheta, 0, 1, 0, -sintheta, 0, costheta;
        Eigen::Matrix3d R2;
        R2 << cosphi, -sinphi, 0, sinphi, cosphi, 0, 0, 0, 1;

        Eigen::Matrix3d R = (Rot.transpose().transpose()) * R1 * R2 * Rmw * Rot.transpose();
        Eigen::VectorXd R_vec(9);
        R_vec << R(0, 0), R(1, 0), R(2, 0), R(0, 1), R(1, 1), R(2, 1), R(0, 2), R(1, 2), R(2, 2);
        Eigen::Vector3d T = BA * R_vec;
        R_and_T temp;
        temp.R = R;
        temp.T = T;
        sol.push_back(temp);
    }
    return sol;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool P3L_Ransac(std::vector<Line3d> &_3D_Line_points_inWorld,std::vector<Line2d> &_2D_Line_points_in_images,
            Eigen::MatrixXd &R,Eigen::Vector3d &T,std::vector<int>& inliers)
{
    if (_3D_Line_points_inWorld.size() < MIN_LINES)
    {
        std::cout << "Not enough lines for P3L_Ransac" << std::endl;
        return false;
    }

    std::vector<V3D> X1_W;
    std::vector<V3D> X2_W;
    for (int i = 0; i < _3D_Line_points_inWorld.size(); i++)
    {
        X1_W.push_back(_3D_Line_points_inWorld[i].start);
        X2_W.push_back(_3D_Line_points_inWorld[i].end);
    }
    std::vector<V2D>x1_c;
    std::vector<V2D>x2_c;
    for (int i = 0; i < _2D_Line_points_in_images.size(); i++)
    {
        x1_c.push_back(_2D_Line_points_in_images[i].start);
        x2_c.push_back(_2D_Line_points_in_images[i].end);
    }
    std::vector<V2D>x1_c_Horizontal;
    std::vector<V2D>x2_c_Horizontal;
    std::vector<V2D>x1_c_Vertical;
    std::vector<V2D>x2_c_Vertical;
    std::vector<V2D>x1_c_Oblique;
    std::vector<V2D>x2_c_Oblique;
    std::vector<V3D>X1_W_Horizontal;
    std::vector<V3D>X2_W_Horizontal;
    std::vector<V3D>X1_W_Vertical;
    std::vector<V3D>X2_W_Vertical;
    std::vector<V3D>X1_W_Oblique;
    std::vector<V3D>X2_W_Oblique;
    for (int i = 0; i < _2D_Line_points_in_images.size(); i++)
    {
        if (_2D_Line_points_in_images[i].LineType == 0)
        {
            x1_c_Horizontal.push_back(_2D_Line_points_in_images[i].start);
            x2_c_Horizontal.push_back(_2D_Line_points_in_images[i].end);
            X1_W_Horizontal.push_back(X1_W[i]);
            X2_W_Horizontal.push_back(X2_W[i]);
        }
        else if (_2D_Line_points_in_images[i].LineType == 1)
        {
            x1_c_Vertical.push_back(_2D_Line_points_in_images[i].start);
            x2_c_Vertical.push_back(_2D_Line_points_in_images[i].end);
            X1_W_Vertical.push_back(X1_W[i]);
            X2_W_Vertical.push_back(X2_W[i]);
        }
        else
        {
            x1_c_Oblique.push_back(_2D_Line_points_in_images[i].start);
            x2_c_Oblique.push_back(_2D_Line_points_in_images[i].end);
            X1_W_Oblique.push_back(X1_W[i]);
            X2_W_Oblique.push_back(X2_W[i]);
        }
    }
    int n=x1_c.size();
    Eigen::MatrixXd nl(3,n);
    Eigen::MatrixXd x1_c_mat(2,n);
    Eigen::MatrixXd x2_c_mat(2,n);
    for (int i = 0; i < n; i++)
    {
        x1_c_mat(0,i)=x1_c[i][0];
        x1_c_mat(1,i)=x1_c[i][1];
        x2_c_mat(0,i)=x2_c[i][0];
        x2_c_mat(1,i)=x2_c[i][1];
    }

    getProjNorm(x1_c_mat,x2_c_mat,nl);

    Eigen::MatrixXd nl_nl(3,2*n);
    nl_nl.block(0,0,3,n)=nl;
    nl_nl.block(0,n,3,n)=nl;
    Eigen::MatrixXd nl_nl_T=nl_nl.transpose();
    Eigen::MatrixXd one=Eigen::MatrixXd::Ones(1,3);
    Eigen::MatrixXd M1=kroneckerProduct(one,nl_nl_T);
    Eigen::MatrixXd X1_W_mat(3,n);
    Eigen::MatrixXd X2_W_mat(3,n);
    for (int i = 0; i < n; i++)
    {
        X1_W_mat(0,i)=X1_W[i][0];
        X1_W_mat(1,i)=X1_W[i][1];
        X1_W_mat(2,i)=X1_W[i][2];
        X2_W_mat(0,i)=X2_W[i][0];
        X2_W_mat(1,i)=X2_W[i][1];
        X2_W_mat(2,i)=X2_W[i][2];
    }
    Eigen::MatrixXd X1_W_X2_W(3,2*n);
    X1_W_X2_W.block(0,0,3,n)=X1_W_mat;
    X1_W_X2_W.block(0,n,3,n)=X2_W_mat;
    Eigen::MatrixXd M2=kroneckerProduct(X1_W_X2_W.transpose(),one);
    Eigen::MatrixXd M=M1.cwiseProduct(M2);
    Eigen::MatrixXd M_nl_nl_T(2*n,12);
    M_nl_nl_T.block(0,0,2*n,9)=M;
    M_nl_nl_T.block(0,9,2*n,3)=nl_nl_T;

    double thErr=0.04;
    //thN = max(0.4 * n, 3);
    double thN=std::max(0.9*n,3.0);
    double maxN=-1;
    int nSAC=3;
    int nIter=0;
    while(1)
    {
        nIter = nIter + 1;
        if (nIter > 750)
        {
            if(thN<3)
            {
                break;
            }
            thN = thN * 0.9;
			nIter = 1;
        }
        //std::vector<int> index=getSubset(n,3);
        Eigen::MatrixXd sp1(2,3);
        Eigen::MatrixXd sp2(2,3);
        Eigen::MatrixXd sP1_w(3,3);
        Eigen::MatrixXd sP2_w(3,3);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, x1_c_Horizontal.size() - 1);
        int index=dis(gen);
        sp1.block(0,0,2,1)=x1_c_Horizontal[index];
        sp2.block(0,0,2,1)=x2_c_Horizontal[index];
        sP1_w.block(0,0,3,1)=X1_W_Horizontal[index];
        sP2_w.block(0,0,3,1)=X2_W_Horizontal[index];

        std::uniform_int_distribution<> dis2(0, x1_c_Vertical.size() - 1);
        int index2=dis2(gen);
        sp1.block(0,1,2,1)=x1_c_Vertical[index2];
        sp2.block(0,1,2,1)=x2_c_Vertical[index2];
        sP1_w.block(0,1,3,1)=X1_W_Vertical[index2];
        sP2_w.block(0,1,3,1)=X2_W_Vertical[index2];

        std::uniform_int_distribution<> dis3(0, x1_c_Oblique.size() - 1);
        int index3=dis3(gen);
        sp1.block(0,2,2,1)=x1_c_Oblique[index3];
        sp2.block(0,2,2,1)=x2_c_Oblique[index3];
        sP1_w.block(0,2,3,1)=X1_W_Oblique[index3];
        sP2_w.block(0,2,3,1)=X2_W_Oblique[index3];

        std::vector<R_and_T> sol=P3L(sp1,sp2,sP1_w,sP2_w);

        for (int j = 0; j < sol.size(); j++)
        {
            Eigen::MatrixXd x(12,1);
            x<<sol[j].R(0,0),sol[j].R(1,0),sol[j].R(2,0),
                sol[j].R(0,1),sol[j].R(1,1),sol[j].R(2,1),
                sol[j].R(0,2),sol[j].R(1,2),sol[j].R(2,2),
                sol[j].T(0,0),sol[j].T(1,0),sol[j].T(2,0);
            Eigen::VectorXd Err=(M_nl_nl_T*x).array().abs();
            double minErr=Err.minCoeff();
            double maxErr=Err.maxCoeff();
            
            std::vector<bool> idx;
            for (int i = 0; i < Err.size(); i++)
            {
                if(Err[i]<thErr)
                {
                    idx.push_back(true);
                }
                else
                {
                    idx.push_back(false);
                }
            }
            Eigen::MatrixXd idx_mat(n,2);
            for (int i = 0; i < n; i++)
            {
                idx_mat(i,0)=idx[i];
                idx_mat(i,1)=idx[i+n];
            }
            std::vector<bool> idx2;
            for (int i = 0; i < n; i++)
            {
                if(idx_mat(i,0)&&idx_mat(i,1))
                {
                    idx2.push_back(true);
                }
                else
                {
                    idx2.push_back(false);
                }
            }
            int resN=0;
            for (int i = 0; i < n; i++)
            {
                if(idx2[i])
                {
                    resN++;
                }
            }
            if(resN>maxN)
            {
                R=sol[j].R;
                T=sol[j].T;
                inliers.clear();
                for (int i = 0; i < n; i++)
                {
                    if(idx2[i])
                    {
                        inliers.push_back(i);
                    }
                }
                maxN=resN;
            }
            if(maxN>thN)
            {
                return true;
            }
        }
    }
}