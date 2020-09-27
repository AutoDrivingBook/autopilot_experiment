# -*- coding: utf-8 -*-
import torch
import torch.utils.data as data
import numpy as np
import os, sys, h5py, subprocess, shlex
import glob
import pdb
import random
import math

import yaml
# from kitti_api.auxiliary.laserscan import *
from pointnet2.data.kitti_api.auxiliary.laserscan import *

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


class Kitti3DSemSeg(data.Dataset):
    def __init__(self, num_points, train = True, download = True, data_precent = 1.0):
        super().__init__()
        self.data_precent = data_precent
        self.train, self.num_points = train, num_points

        pcd_path = '/kitti_semantic/dataset/sequences/00/velodyne/'
        label_path = '/kitti_semantic/dataset/sequences/00/labels/'
        pcd_list, label_list = [], []
        points, labels = [], []
        # 写入pcd_path的路径
        pcd_list = glob.glob(os.path.join(pcd_path, '*.bin'))
        # 写入label_path的路径
        label_list = glob.glob(os.path.join(label_path, '*.label'))
        # 将文件list重排序
        pcd_list.sort()
        label_list.sort()

        # 验证pcd_list与label_list的长度是否相同
        assert(len(pcd_list) == len(label_list))
        num_of_file = len(pcd_list)

        # 读取点云
        config_path = '/kitti_semantic/Pointnet2_PyTorch-master/pointnet2/data/kitti_api/config/semantic-kitti.yaml'
        CFG = yaml.safe_load(open(config_path, 'r'))
        color_dict = CFG["color_map"]
        class_remap = CFG["learning_map"]

        nclasses = len(color_dict)
        print(nclasses)
        sem_ls = SemLaserScan(nclasses, color_dict, project = False)

        k = 0
        for i in range(0, num_of_file):
            scan_file = pcd_list[i]
            label_file = label_list[i]
            
            sem_ls.open_scan(scan_file)
            sem_ls.open_label(label_file)
            scans = sem_ls.points.tolist()
            sems = sem_ls.sem_label.astype(np.float32).tolist()

            if len(scans) >= 100592 and len(sems) >= 100592:
                points.extend(scans[0 : 100592])
                labels.extend(sems[0 : 100592])
                k += 1
        
        points = np.array(points)
        print(points.shape)
        points = np.reshape(points, (k, 100592, 3))
        self.points = np.tile(points, (1, 1, 3))
        labels_remap = []
        for x in labels:
            labels_remap.append(class_remap[x])
        labels = np.array(labels_remap)
        print(len(labels.nonzero()[0]))
        self.labels = np.reshape(labels, (num_of_file, 100592))

        print(self.points.shape)
        print(self.labels.shape)
        print(max(labels))

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
    dset = Kitti3DSemSeg(16, train = True)
    print(dset[0])
    print(len(dset))
    dloader = torch.utils.data.DataLoader(dset, batch_size=32, shuffle=True)
    for i, data in enumerate(dloader, 0):
        inputs, labels = data
        if i == len(dloader) - 1:
            print(inputs.size())
        

            