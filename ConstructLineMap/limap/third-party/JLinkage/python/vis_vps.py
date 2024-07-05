import os, sys
import cv2
import pdb
import seaborn as sns
import numpy as np

imname = 'data/indoor.jpg'
img = cv2.imread(imname)

txt_vps = 'data/vps.txt'
with open(txt_vps, 'r') as f:
    lines = f.readlines()
mapp = {}
for line in lines:
    dd = line.strip('\n').split(' ')
    seg = [int(float(k)) for k in dd]
    comp_id = int(dd[4])
    if comp_id not in mapp:
        mapp[comp_id] = [seg]
    else:
        mapp[comp_id].append(seg)

colors = sns.color_palette('husl', n_colors=len(mapp))
colors = (np.array(colors) * 255).astype(np.uint8).tolist()
for comp_id, segs in mapp.items():
    c = colors[comp_id]
    for seg in segs:
        cv2.line(img, (seg[0], seg[1]), (seg[2], seg[3]), c, 2)

cv2.imwrite('temp.png', img)
pdb.set_trace()

