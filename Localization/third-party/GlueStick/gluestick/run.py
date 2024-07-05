import argparse
import os
from os.path import join

import cv2
import torch
import numpy as np
from matplotlib import pyplot as plt

from gluestick import batch_to_np, numpy_image_to_torch, GLUESTICK_ROOT
from gluestick.drawing import plot_images, plot_lines, plot_color_line_matches, plot_keypoints, plot_matches
from gluestick.models.two_view_pipeline import TwoViewPipeline


def main():
    # Parse input parameters
    parser = argparse.ArgumentParser(
        prog='GlueStick',
        description='Line matches obtained by GlueStick')
    parser.add_argument('--max_pts', type=int, default=1000)
    parser.add_argument('--max_lines', type=int, default=3000)
    parser.add_argument('--skip-imshow', default=False, action='store_true')
    args = parser.parse_args()

    # Evaluation config
    conf = {
        'name': 'two_view_pipeline',
        'use_lines': True,
        'extractor': {
            'name': 'wireframe',
            'sp_params': {
                'force_num_keypoints': False,
                'max_num_keypoints': args.max_pts,
            },
            'wireframe_params': {
                'merge_points': True,
                'merge_line_endpoints': True,
            },
            'max_n_lines': args.max_lines,
        },
        'matcher': {
            'name': 'gluestick',
            'weights': str(GLUESTICK_ROOT / 'resources' / 'weights' / 'checkpoint_GlueStick_MD.tar'),
            'trainable': False,
        },
        'ground_truth': {
            'from_pose_depth': False,
        }
    }

    #!!!Change the path to your own!!!
    retrieval_txt = '.../Cambridge/ShopFacade/output_hloc/pairs-query-netvlad5.txt'
    query_image_name = []
    db_image_name = []
    with open(retrieval_txt, 'r') as f:
        for line in f.readlines():
            line = line.strip().split()
            query_image_name.append(line[0].split('/')[-1])
            db_image_name.append(line[1].split('/')[-1])
    
    success_num=0
    for i in range(len(query_image_name)):
        db_lineSegment_txt = '.../Cambridge/ShopFacade/output_hloc/GlueStick/'+db_image_name[i]+'.txt'
        query_image = '.../Cambridge/ShopFacade/undistorted_images/'+query_image_name[i]
        db_image = '.../Cambridge/ShopFacade/undistorted_images/'+db_image_name[i]
        query_image_name_refule=query_image_name[i].replace('.jpg','')
        db_image_name_refule=db_image_name[i].replace('.jpg','')
        match_result_txt='.../Cambridge/ShopFacade/output_hloc/GlueStick/'+query_image_name_refule+"_"+db_image_name_refule+"_matchline.txt"

        with open(match_result_txt, 'w') as f:
            pass
        db_images_lines=[]
        db_images_lines_scores=[]
        db_images_lines_valid=[]
        if not os.path.exists(db_lineSegment_txt):
            continue
        DBSegmentID=[]
        with open(db_lineSegment_txt, 'r') as f:
            for line in f.readlines():
                line = line.strip().split()
                line = [float(i) for i in line]
                DBSegmentID.append(line[0])
                db_images_lines.append(line[1:])
        if len(db_images_lines)==0:
            continue

        for i in range(len(db_images_lines)):
            db_images_lines_scores.append(((db_images_lines[i][0]-db_images_lines[i][2])**2+(db_images_lines[i][1]-db_images_lines[i][3])**2)**0.5)
        db_images_lines_scores=[i/max(db_images_lines_scores) for i in db_images_lines_scores]

        sorted_db_images_lines_scores,sorted_db_images_lines=zip(*sorted(zip(db_images_lines_scores,db_images_lines),reverse=True))
        sorted_db_images_lines_index=[]
        for i in range(len(sorted_db_images_lines)):
            sorted_db_images_lines_index.append(db_images_lines.index(sorted_db_images_lines[i]))

        db_images_lines_tensor=torch.tensor(db_images_lines,dtype=torch.float32)
        db_images_lines_tensor=db_images_lines_tensor.view(-1,2,2)

        db_images_lines_valid=[True for i in range(len(db_images_lines))]
        sorted_db_images_lines = torch.tensor(sorted_db_images_lines, dtype=torch.float32)
        sorted_db_images_lines=sorted_db_images_lines.view(-1,2,2)
        sorted_db_images_lines_scores=torch.tensor(sorted_db_images_lines_scores,dtype=torch.float32)
        sorted_db_images_lines_scores=sorted_db_images_lines_scores.view(-1)
        db_images_lines_valid=torch.tensor(db_images_lines_valid,dtype=torch.bool)
        db_images_lines_valid=db_images_lines_valid.view(-1)

        device = 'cuda' if torch.cuda.is_available() else 'cpu'
        pipeline_model = TwoViewPipeline(conf).to(device).eval()

        query_gray0 = cv2.imread(query_image, 0)
        db_gray1 = cv2.imread(db_image, 0)

        max_resolution=1600
        if query_gray0.shape[0]>max_resolution or query_gray0.shape[1]>max_resolution:
            if query_gray0.shape[0]>query_gray0.shape[1]:
                scale=max_resolution/query_gray0.shape[0]
            else:
                scale=max_resolution/query_gray0.shape[1]
            query_gray0=cv2.resize(query_gray0,(int(query_gray0.shape[1]*scale),int(query_gray0.shape[0]*scale)))
            db_gray1=cv2.resize(db_gray1,(int(db_gray1.shape[1]*scale),int(db_gray1.shape[0]*scale)))

        if query_gray0 is None or db_gray1 is None:
            continue
    
        torch_query_gray0, torch_db_gray1 = numpy_image_to_torch(query_gray0), numpy_image_to_torch(db_gray1)
        torch_query_gray0, torch_db_gray1 = torch_query_gray0.to(device)[None], torch_db_gray1.to(device)[None]

        sorted_db_images_lines=sorted_db_images_lines.to(device)[None]
        sorted_db_images_lines_scores=sorted_db_images_lines_scores.to(device)[None]
        db_images_lines_valid=db_images_lines_valid.to(device)[None]

        x = {'image0': torch_query_gray0, 'image1': torch_db_gray1,'lines1':sorted_db_images_lines,
            'line_scores1': sorted_db_images_lines_scores,
            'valid_lines1': db_images_lines_valid}
        pred = pipeline_model(x)
        pred = batch_to_np(pred)
        m0 = pred["matches0"]
        line_seg0, line_seg1 = pred["lines0"], pred["lines1"]
        line_matches = pred["line_matches0"]
        valid_matches = m0 != -1
        match_indices = m0[valid_matches]
        valid_matches = line_matches != -1
        match_indices = line_matches[valid_matches]
        matched_lines0 = line_seg0[valid_matches]
        matched_lines1 = line_seg1[match_indices]

        valid_matches_index=[i for i in range(len(valid_matches)) if valid_matches[i]==True]

        db_matched_lines_index=[]
        sorted_db_images_lines_index=np.array(sorted_db_images_lines_index)
        db_matched_lines_index=sorted_db_images_lines_index[match_indices]

        db_matched_lines_ID=[]
        for i in range(len(db_matched_lines_index)):
            db_matched_lines_ID.append(int(DBSegmentID[db_matched_lines_index[i]]))

        query_lines=np.array(matched_lines0)
        query_lines=query_lines.reshape(-1,4)
        
        with open(match_result_txt,'w') as f:
            for i in range(len(matched_lines0)):
                f.write(str(db_matched_lines_ID[i])+' '+str(query_lines[i][0])+' '+str(query_lines[i][1])+' '+str(query_lines[i][2])+' '+str(query_lines[i][3])+'\n')
        success_num+=1
        print('success_num:',success_num,'/',len(query_image_name))








if __name__ == '__main__':
    main()
