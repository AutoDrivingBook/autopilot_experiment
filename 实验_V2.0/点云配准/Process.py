#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2020/6/27 上午9:34
# @Author  : LHo

# 本文件用来设置函数们

import numpy as np
import pcl

def txt_reader(fileName):
    """
    :param fileName: 点云文件，格式为pcb
    :return: 返回点云
    """
    cloud_ = pcl.load(fileName)

    return cloud_

def Kdsearch(sour_pole, dist_pole):
    """
    使用KDTree搜索最近
    :param sour_pole: Kdtree搜索的源点云
    :param dist_pole: Kdtree搜索的搜索点云
    :return: 误差均值向量
    """
    cloud_ = pcl.PointCloud()

    size_of_sourPole = sour_pole.width
    size_of_distPole = dist_pole.width


    points = np.zeros((size_of_sourPole, 3), dtype = np.float32)
    for i in range(0, size_of_distPole):
        points[i][0] = dist_pole[i][0]
        points[i][1] = dist_pole[i][1]
        points[i][2] = dist_pole[i][2]
    cloud_.from_array(points)

    radius = 1.0
    match_size = 0

    matched_flags = np.zeros((size_of_sourPole, 1))
    kdtree = cloud_.make_kdtree_flann()
    allMeanVector = np.zeros((1, 3), dtype = np.float32)
    # 为了方便去前100
    for i in range(0, size_of_sourPole):
        # 设置待搜索的点
        searchPointPCL = pcl.PointCloud()
        searchPoint = np.zeros((1, 3), dtype = np.float32)
        searchPoint[0][0] = sour_cloud[i][0]
        searchPoint[0][1] = sour_cloud[i][1]
        searchPoint[0][2] = sour_cloud[i][2]
        searchPointPCL.from_array(searchPoint)
        # print("搜索点为 x: " + str(searchPointPCL[0][0]) + ', y : ' +
        #       str(searchPointPCL[0][1]) + ', z : ' + str(searchPointPCL[0][2]))

        # 使用kdtree搜索K近邻
        [ind, sqdist] = kdtree.nearest_k_search_for_cloud(searchPointPCL, 10)
        # [ind, sqdist] = kdtree.radius_search_for_cloud(searchPointPCL, radius)
        errVector = np.zeros((1, 3), dtype = np.float32)
        for k in range(0, ind.size):
            if(matched_flags[ind[0][k]][0] == 0):
                # print('匹配点为: (' + str(cloud_[ind[0][k]][0]) + ' ' + str(cloud_[ind[0][k]][1]) + ' ' + str(
                #     cloud_[ind[0][k]][2]) + ' (squared distance: ' + str(sqdist[0][k]) + ')')
                # 计算当前的平均误差
                if(sqdist[0][k] < radius):
                    errVector[0][0] = errVector[0][0] + searchPointPCL[0][0] - cloud_[ind[0][k]][0]
                    errVector[0][1] = errVector[0][1] + searchPointPCL[0][1] - cloud_[ind[0][k]][1]
                    errVector[0][2] = errVector[0][2] + searchPointPCL[0][2] - cloud_[ind[0][k]][2]

                    # 将当前flag设为1
                    matched_flags[ind[0][k]][0] = 1
                    match_size += 1

        allMeanVector += errVector

    print("matched size: ", match_size)
    print("mean errVector: ", allMeanVector/match_size)

    return allMeanVector/match_size

def mix(sour_pole, dist_pole, save_file_name):
    """
    混合两片点云
    :param sour_pole: 第一片点云
    :param dist_pole: 第二片点云
    :return: None
    """
    d_value_vector = Kdsearch(sour_pole, dist_pole)

    # 创建新的点云
    p_sour = pcl.PointCloud()
    p_sour_data = np.zeros((sour_pole.width, 3), dtype = np.float32)

    for i in range(0, sour_pole.width):
        # print(sour_pole[i][0])
        p_sour_data[i][0] = sour_pole[i][0] - d_value_vector[0][0]
        p_sour_data[i][1] = sour_pole[i][1] - d_value_vector[0][1]
        p_sour_data[i][2] = sour_pole[i][2] - d_value_vector[0][2]
    p_sour.from_array(p_sour_data)


    pcl.save(p_sour, save_file_name, 'pcd')
    print("successful save new pcd file")

if __name__ == "__main__":
    sour_cloud = txt_reader("data/128.pcd")
    dist_cloud = txt_reader("data/288.pcd")
    print('Loaded ' + str(sour_cloud.width * sour_cloud.height) +
          ' data points from source.pcb with the following fields: ')
    print('Loaded ' + str(dist_cloud.width * dist_cloud.height) +
          ' data points from source.pcb with the following fields: ')

    mix(sour_cloud, dist_cloud, "data/288_1.pcd")

    # Kdsearch(sour_cloud, dist_cloud)
    # for i in range(0, cloud.size):
    #     print('x: ' + str(cloud[i][0]) + ', y : ' +
    #           str(cloud[i][1]) + ', z : ' + str(cloud[i][2]))

