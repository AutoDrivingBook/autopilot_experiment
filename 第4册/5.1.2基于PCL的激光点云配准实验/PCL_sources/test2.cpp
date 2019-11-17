#include<pcl/visualization/pcl_visualizer.h>
#include<iostream>//标准C++库中的输入输出类相关头文件。
#include<pcl/io/io.h>
#include<pcl/io/pcd_io.h>//pcd 读写类相关的头文件。

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

void BoundingBox(PointPtr *c_sour){
	FILE *fp_1;//定义一个文件类型指针
	feature features[20];//定义一个含有20个结构体变量的数组
	fp_1 = fopen("H:/pro_software_workplace/VisualStudio/Project1/Project1/feature_128.txt", "r");//打开文件feature_128.txt
	if (fp_1){
		for (int i = 0; i < 20; ++i){
			//循环将20行数据写入feature结构体数组中
			fscanf(fp_1, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%ld,%ld\n",
				&features[i].x, &features[i].y, &features[i].z,
				&features[i].length, &features[i].width, &features[i].height,
				&features[i].dir_x, &features[i].dir_y, &features[i].dir_z,
				&features[i].type, &features[i].index);
		}
	}
	else
		std::cout << "数据加载失败" << std::endl;
	fclose(fp_1);

	PointPtr sour(new pcl::PointCloud<PointIoType>());
	sour = *c_sour;//接收传入的点云
	boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer(new pcl::visualization::PCLVisualizer("3D Viewer"));//实例化一个点云可视化对象
	viewer->setBackgroundColor(0, 0, 0);//设置背景为黑色
	viewer->addPointCloud<PointIoType>(sour, "sample sour");//添加需要显示的点云
	for (int i = 0; i<20; i++){
		//循环遍历feature结构体数据，生成包围盒
		Eigen::Vector3d noml(features[i].dir_x, features[i].dir_y, features[i].dir_z);//用Vector3d类型存储法向量
		//noml.normalize();//法向量单位化
		Eigen::Matrix3f mt;//定义旋转矩阵
		mt << noml.x(), -noml.y(), noml.z(),
			noml.y(), noml.x(), noml.z(),
			0, 0, 1.0;
		Eigen::Quaternionf q(mt);//定义四元数
		noml.normalize();
		Eigen::Vector3f translation(features[i].x, features[i].y, features[i].z);//定义一个浮点类型包围盒中心坐标向量
		viewer->addCube(translation, q, features[i].length, features[i].width, features[i].height, "bbox" + to_string(i));//在viewer中添加包围盒
	}
	while (!viewer->wasStopped())//点云图像持久化
	{
		viewer->spinOnce(100);
		boost::this_thread::sleep(boost::posix_time::microseconds(100000));
	}
}

int main(int argc, char** argv){
	pcl::PointCloud<PointIoType>::Ptr cloud(new pcl::PointCloud<PointIoType>());
	//读取要显示的点云文件
	if (pcl::io::loadPCDFile("H:/pro_software_workplace/VisualStudio/Project1/Project1/128.pcd", *cloud) == -1)
		return (-1);
	BoundingBox(&cloud);//调用包围盒添加函数
	return 0;
}
