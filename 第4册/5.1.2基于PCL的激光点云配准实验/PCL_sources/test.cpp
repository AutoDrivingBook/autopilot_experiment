#include<pcl/visualization/pcl_visualizer.h>
#include<iostream>//标准C++库中的输入输出类相关头文件。
#include<pcl/io/io.h>
#include<pcl/io/pcd_io.h>//pcd 读写类相关的头文件。

using namespace std;
using namespace pcl;
typedef pcl::PointXYZRGB PointIoType;//对激光点云类型进行宏定义

int main(int argc, char** argv){
	pcl::PointCloud<PointIoType>::Ptr cloud(new pcl::PointCloud<PointIoType>());
	if (pcl::io::loadPCDFile("H:/pro_software_workplace/VisualStudio/Project1/Project1/128.pcd", *cloud) == -1)
		return (-1);
	boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer1(new pcl::visualization::PCLVisualizer("3D Viewer"));
	viewer1->addPointCloud<PointIoType>(cloud, "128.pcb");
	viewer1->setBackgroundColor(0, 0, 0);//设置可视化窗口的背景颜色为黑色
	while (!viewer1->wasStopped())//持久化显示激光点云图像
	{
		viewer1->spinOnce(100);
		boost::this_thread::sleep(boost::posix_time::microseconds(100000));
	}

	return 0;
}
