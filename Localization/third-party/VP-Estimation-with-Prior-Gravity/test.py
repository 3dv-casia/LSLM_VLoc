import os
import numpy as np
import cv2
import sys
import random
import argparse

sys.path.append(os.path.join(os.path.dirname(os.path.abspath("")), "build/vp_estimation_with_prior_gravity"))
from vp_estimation_with_prior_gravity.solvers import run_hybrid_uncalibrated
from vp_estimation_with_prior_gravity.evaluation import project_vp_to_image, get_labels_from_vp
from vp_estimation_with_prior_gravity.visualization import plot_images, plot_vp, plot_lines

def main():
    parser = argparse.ArgumentParser(description='Line grouping by estimating vanishing points')
    parser.add_argument('arg1', type=str, help='image path')
    parser.add_argument('arg2', type=str, help='lines path')
    args = parser.parse_args()

    # Set the parameters
    SOLVER_FLAGS = [True, True, True, True, True]
    th_pixels = 3  
    ls_refinement = 2  
    nms = 1  
    magsac_scoring = True

    img_path = args.arg1
    img = cv2.imread(img_path, 0)
    height,width=img.shape
    scale=1
    if height>1600 or width>1600:
        if height>width:
            scale=1600/height
        else:
            scale=1600/width

    if (SOLVER_FLAGS == [True, False, False, False, False]) or (SOLVER_FLAGS == [False, False, True, False, False]):
        vertical = np.array([random.random() / 1e12, 1, random.random() / 1e12])
        vertical /= np.linalg.norm(vertical)
    else:
        vertical = np.array([0., 1, 0.])
        
    # Intrinsics matrix 
    K = np.array([[1683.36755371, 0., 960],
                [0., 1683.36755371, 540],
                [0., 0., 1.]])
    
    # Load the lines
    lines_txt = args.arg2
    lines_data=open(lines_txt,'r')

    lines=[]
    for line in lines_data.readlines():
        line=line.strip('\n')
        line=line.split(' ')
        line=line[1:]
        for i in range(len(line)):
            line[i]=float(line[i])/scale
        line=np.array(line)
        line=line.astype(np.float32)
        line=line.reshape(2,2)
        lines.append(line)

    lines=np.array(lines)
    lines=lines.reshape(-1,2,2)

    principle_point = np.array([img.shape[1] / 2.0, img.shape[0] / 2.0])
    f, vp = run_hybrid_uncalibrated(
        lines - principle_point[None, None, :],
        vertical, th_pixels=th_pixels, ls_refinement=ls_refinement,
        nms=nms, magsac_scoring=magsac_scoring, sprt=True, solver_flags=SOLVER_FLAGS)
    vp[:, 1] *= -1
    vp_labels = get_labels_from_vp(lines[:, :, [1, 0]], project_vp_to_image(vp, K), threshold=th_pixels)[0]

    lines_data=open(lines_txt,'r')
    lines_data=lines_data.readlines()
    for i in range(len(lines)):
        lines_data[i]=lines_data[i].strip('\n')
        lines_data[i]+=' '+str(vp_labels[i])+'\n'
    with open(lines_txt,'w') as f:
        for line in lines_data:
            f.write(line)









if __name__ == '__main__':
    main()

