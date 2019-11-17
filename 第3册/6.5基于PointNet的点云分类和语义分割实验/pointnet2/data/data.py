import glob
import os
import numpy as np
import random 
pcd_path = '/home1/xuhui/Pointnet2_PyTorch-master/pointnet2/data/3d-sample/pcd_files/'
label_path = '/home1/xuhui/Pointnet2_PyTorch-master/pointnet2/data/3d-sample/label/'

def rotate_point (point, rotation_angle):
    point = np.array(point)
    cos_theta = np.cos(rotation_angle)
    sin_theta = np.sin(rotation_angle)
    rotation_matrix = np.array([[cos_theta, sin_theta, 0],
                                [-sin_theta, cos_theta, 0],
                                [0, 0, 1]])
    rotated_point = np.dot(point.reshape(-1, 3), rotation_matrix)
    return rotated_point
###########

def jitter_point(point, sigma=0.01, clip=0.05):
    assert(clip > 0)
    point = np.array(point)
    point = point.reshape(-1,3)
    Row, Col = point.shape
    jittered_point = np.clip(sigma * np.random.randn(Row, Col), -1*clip, clip)
    jittered_point += point
    return jittered_point

def augment_data(point, rotation_angle, sigma, clip):
    return jitter_point(rotate_point(point, rotation_angle), sigma, clip)


pcd_list,label_list = [],[]
points,labels = [],[]
pcd_list = glob.glob(os.path.join(pcd_path,'*'))
pcd_list.sort()
label_list = glob.glob(os.path.join(label_path,'*'))
label_list.sort()
for i,files in enumerate(pcd_list):
    # print(files)
    f = open(files)
    for n,line in enumerate(f):
        if n >=11 and n<=90122:
            # data = f.readlines()[8:]
            data = line.strip().split()
            data =[float(d) for d in data]
            points.append(data)
        
        else:
        	continue
    # print(np.array(points).shape)
points = np.array(points)
points = np.reshape(points,(100,90112,3))
points = augment_data(points, rotation_angle = random.uniform(-30,30),
    sigma = 0.01,clip = 0.05)
print(points.shape)

# for i,files in enumerate(label_list):
#     f = open (files)
#     for n,line in enumerate(f):
#         if n >=0 and n<=90111:
#             # data = f.readlines()[8:]
#             data = line.strip().split()
#             data =[float(d) for d in data]
#             labels.append(data)
        
#         else:
#             continue

# labels = np.array(labels)
# labels = np.reshape(labels,(100,90112))
# print(labels.shape)


    # print(i,data)
    # idxs = []
    # idxs.append(i)
# points = data_batchlist[idxs,...]
# labels = labels_batchlist[idxs,...]
# print(points)
# fo = open('/home1/xuhui/Pointnet2_PyTorch-master/pointnet2/data/points.txt','w')
# fo.write(str(points)+'\n')
# fo.close()