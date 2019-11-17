# -*- coding: utf-8 -*-
import torch
import torch.utils.data as data
import numpy as np
import os, sys, h5py, subprocess, shlex
import glob
import pdb
import random
import math
BASE_DIR = os.path.dirname(os.path.abspath(__file__))


def _get_data_files(list_filename):
    with open(list_filename) as f:
        return [line.rstrip() for line in f]


def _load_data_file(name):
    f = h5py.File(name)
    data = f['data'][:]
    label = f['label'][:]
    return data, label
###########
# 绕Z轴旋转 #
###########
# point: vector(1*3:x,y,z)
# rotation_angle: scaler 0~2*pi
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
# 在XYZ上加高斯噪声 #
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

class Indoor3DSemSeg(data.Dataset):

    def __init__(self, num_points, train=True, download=True, data_precent=1.0):
        super().__init__()
        self.data_precent = data_precent
        self.train, self.num_points = train, num_points

        pcd_path = '/home1/xuhui/Pointnet2_PyTorch-master/pointnet2/data/3d-sample/pcd_files/'
        label_path = '/home1/xuhui/Pointnet2_PyTorch-master/pointnet2/data/3d-sample/label/'
        pcd_list,label_list = [],[]
        points,labels = [],[]
        pcd_list = glob.glob(os.path.join(pcd_path,'*'))
        pcd_list.sort()

        label_list = glob.glob(os.path.join(label_path,'*'))
        label_list.sort()
        # print(pcd_list,label_list)
        # pdb.set_trace()
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
        points = np.array(points)
        points = np.reshape(points,(100,90112,3))
        # points = augment_data(points, rotation_angle = random.uniform(0,2)*math.pi,
        #     sigma = 0.01,clip = 0.05)
        # points = np.reshape(points,(100,90112,3))
        self.points = np.tile(points, (1,1,3))
        # self.points = np.tile(np.reshape(points,(100,90112,3)),(1,1,3))
        print(self.points.shape)
        # pdb.set_trace()

        for i,files in enumerate(label_list):
            f = open (files)
            for n,line in enumerate(f):
                if n >=0 and n<=90111:
                    # data = f.readlines()[8:]
                    data = line.strip().split()
                    data =[float(d) for d in data]
                    labels.append(data)
                
                else:
                    continue

        labels = np.array(labels)
        self.labels = np.reshape(labels,(100,90112))
        print(self.labels.shape)

            # data_batches = np.concatenate(p, 0)




    def __getitem__(self, idx):
        pt_idxs = np.arange(0, self.num_points)
        # np.random.shuffle(pt_idxs)

        current_points = torch.from_numpy(self.points[idx, pt_idxs].copy()
                                         ).type(torch.FloatTensor)
        # pdb.set_trace()
        current_labels = torch.from_numpy(self.labels[idx, pt_idxs].copy()
                                         ).type(torch.LongTensor)

        return current_points, current_labels

    def __len__(self):
        return int(self.points.shape[0] * self.data_precent)

    def set_num_points(self, pts):
        self.num_points = pts

    def randomize(self):
        pass


if __name__ == "__main__":
    dset = Indoor3DSemSeg(16, "./", train=True)
    print(dset[0])
    print(len(dset))
    dloader = torch.utils.data.DataLoader(dset, batch_size=32, shuffle=True)
    for i, data in enumerate(dloader, 0):
        inputs, labels = data
        if i == len(dloader) - 1:
            print(inputs.size())
