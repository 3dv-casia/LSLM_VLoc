import os, sys
import cv2
import pdb

imname = 'data/indoor.jpg'
img = cv2.imread(imname)

txt_lines = 'data/lines.txt'
with open(txt_lines, 'r') as f:
    lines = f.readlines()

for line in lines:
    dd = line.strip('\n').split(' ')
    dd = [int(float(k)) for k in dd]
    c = (255, 0, 0)
    cv2.line(img, (dd[0], dd[1]), (dd[2], dd[3]), c, 1)

cv2.imwrite('temp.png', img)
pdb.set_trace()

