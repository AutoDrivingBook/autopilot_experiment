#pragma once
#ifndef _PROCESS_
#define _PROCESS_
#include <math.h>
#include<stdio.h>
#include<pcl/io/io.h>
#include<pcl/io/pcd_io.h>//pcd 读写类相关的头文件。
#include<pcl/io/ply_io.h>
#include<pcl/point_types.h> //PCL中支持的点类型头文件。
#include <boost/thread/thread.hpp>
#include <vector>
#include <pcl/kdtree/kdtree_flann.h>
#include<pcl/visualization/pcl_visualizer.h>
#include <Eigen/Core>

#define SIZE_OF_FEATURES 28 //融合的两片点云feature总数
using namespace std;
using namespace pcl;
typedef pcl::PointXYZRGB PointIoType;
typedef pcl::PointCloud<pcl::PointXYZRGB>::Ptr PointPtr;

typedef struct feature_box
{
	double x;  //center_x
	double y;  //center_y
	double z;  //center_z
	double length;
	double width;
	double height;
	double dir_x;
	double dir_y;
	double dir_z;
	int type;
	int index;
}feature;

class Process //声明一个Process点云处理类
{
public:
	Process();
	~Process();
	void execute();//主函数中的调用的融合执行函数
	void mix(PointPtr *sour, PointPtr *dist);//融合函数
	Eigen::Vector3d Kdsearch();//Kdtree搜索函数
	void txt_Reader();//feature.txt文件读取函数
private:
	std::vector<feature> sour_pole;//Kdtree搜索的源点云
	std::vector<feature> dist_pole;//Kdtree搜索的搜索点云
	feature features[SIZE_OF_FEATURES];//包围盒结构体数据数组
};

#endif
