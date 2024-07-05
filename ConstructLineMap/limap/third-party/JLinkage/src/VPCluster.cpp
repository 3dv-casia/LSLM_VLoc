/* 
 *  Copyright (c) 2011  Chen Feng (cforrest (at) umich.edu)
 *    and the University of Michigan
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

#include "stdio.h"
#include "updator.h"
#include "RandomSampler.h"
#include "JLinkage.h"
#include "VPPrimitive.h"
#include "VPCluster.h"

namespace VPCluster {
	using namespace Updator;

	struct LabelStat {
		unsigned int cnt;
		unsigned int id;
		bool operator<(const LabelStat& rhs) const
		{
			if(cnt!=rhs.cnt) return cnt>rhs.cnt;
			return id>rhs.id;
		}
	};

	// Function pointers
	std::vector<float>  *(*mGetFunction)(const std::vector<sPt *> &nDataPtXMss, const std::vector<unsigned int>  &nSelectedPts);
	float (*mDistanceFunction)(const std::vector<float>  &nModel, const std::vector<float>  &nDataPt);

	unsigned int run(
		//// OUTPUT
		std::vector<unsigned int>& Labels,
		std::vector<unsigned int>& LabelCount,
		//// Input arguments
		// Arg 0, points
		std::vector<std::vector<float> *> *mDataPoints,
		// Arg 1, Models
		std::vector<std::vector<float> *> *mModels,
		// Arg 2, InliersThreshold
		float mInlierThreshold,
		// Arg 3, type of model: 0 - Planes 1 - 2dLines
		unsigned int /*mModelType*/,
		// ----- facultatives
		// Arg 4, Select the KNN number of neighboards that can be merged. ( = 0 if all the points are used; n^2 complexity)
		int mKDTreeRange,
		// Arg 5, Already existing clusters, Logical Matrix containing Pts X NCluster");
		std::vector<std::vector<unsigned int> > mExistingClusters
		) 
	{
		// arg2 : InlierThreshold
		if (mInlierThreshold <= 0.0f) {
			printf("Invalid InlierThreshold");
			return 0;
		}
		
		mGetFunction = GetFunction_VP;
		mDistanceFunction = DistanceFunction_VP;
	
		// Compute the jLinkage Clusterization
		JLinkage mJLinkage(mDistanceFunction, mInlierThreshold, 
			(unsigned int)mModels->size(), true, 
			(unsigned int)((*mDataPoints)[0])->size(), mKDTreeRange);

		std::vector<const sPtLnk *> mPointMap(mDataPoints->size());	

		std::list<sClLnk *> mClustersList;

		unsigned int counter = 0;
		// InitializeWaitbar("Loading Models ");
		for(unsigned int nModelCount = 0; nModelCount < mModels->size(); nModelCount++){
			mJLinkage.AddModel(((*mModels)[nModelCount]));
			++counter;
            UpdateWaitbar((float)counter/(float)mModels->size());
		}
		// CloseWaitbar();

		counter = 0;
		// InitializeWaitbar("Loading Points ");
		for(std::vector<std::vector<float> *>::iterator iterPts = mDataPoints->begin();
				iterPts != mDataPoints->end(); ++iterPts ){
			mPointMap[counter] = mJLinkage.AddPoint(*iterPts);
			++counter;
			UpdateWaitbar((float)counter/(float)mDataPoints->size());
		}
		// CloseWaitbar();

		if(mExistingClusters.size() > 0){
			printf("\tLoading Existing Models \n");
			for(int i=0; i < (int)mExistingClusters.size(); ++i){
				if(!mJLinkage.ManualClusterMerge(mExistingClusters[i])) {
					printf("Invalid Existing cluster matrix");
					return 0;
				}
			}
		}

		// InitializeWaitbar("J-Clusterizing ");
		mClustersList = mJLinkage.DoJLClusterization(UpdateWaitbar);
		// CloseWaitbar();

		// Write output
		// 	plhs[0] = mxCreateDoubleMatrix(1,mDataPoints->size(), mxREAL);
		// 	double *mTempUintPointer = (double *)mxGetPr(plhs[0]);

		std::vector<LabelStat> stats;

		unsigned int counterCl = 0;
		Labels.clear();
		Labels.resize(mDataPoints->size(),10);
		for(std::list<sClLnk *>::iterator iterCl = mClustersList.begin(); iterCl != mClustersList.end(); ++iterCl){
			unsigned int cnt=0;
			for(std::list<sPtLnk *>::iterator iterPt = (*iterCl)->mBelongingPts.begin(); iterPt != (*iterCl)->mBelongingPts.end(); ++iterPt){
				unsigned int counterPt = 0;
				for(std::vector<const sPtLnk *>::iterator iterPtIdx = mPointMap.begin(); iterPtIdx != mPointMap.end(); ++iterPtIdx){
					if((*iterPt) == (*iterPtIdx)){
						//mTempUintPointer[counterPt] = counterCl;
						Labels[counterPt] = counterCl;
						++cnt;
						break;
					}
					++counterPt;
				}
			}
			LabelStat s;
			s.cnt = cnt;
			s.id = counterCl;
			stats.push_back(s);
			++counterCl;
		}

		std::sort(stats.begin(),stats.end());
		std::vector<unsigned int> Idx(stats.size());
		LabelCount.clear();
		for(int i=0; i<(int)stats.size(); ++i) {
			Idx[ stats[i].id ] = i;
			LabelCount.push_back(stats[i].cnt);
		}

		unsigned int lsize = (int)Labels.size();
		for(int i=0; i<(int)lsize; ++i) {
			unsigned int old = Labels[i];
			Labels[i] = Idx[old];
		}

		return (int)stats.size();

	}
}

