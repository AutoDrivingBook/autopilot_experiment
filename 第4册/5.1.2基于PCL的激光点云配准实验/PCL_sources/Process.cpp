#include "Process.h"


Process::Process()
{

}
Process::~Process()
{

}

void Process::execute(){
	pcl::PCDReader reader;
	//定义两个点云变量用于接收用于融合的两片点云数据
	pcl::PointCloud<PointIoType>::Ptr cloud(new pcl::PointCloud<PointIoType>())
									, cloud2(new pcl::PointCloud<PointIoType>());
	//读取点云文件数据
	reader.read("H:/pro_software_workplace/VisualStudio/Project1/Project1/288.pcd", *cloud);
	reader.read("H:/pro_software_workplace/VisualStudio/Project1/Project1/128.pcd", *cloud2);
	mix(&cloud, &cloud2);//点云融合
}
void Process::mix(PointPtr *sour, PointPtr *dist){
	Eigen::Vector3d D_value_vector = Kdsearch();//传入误差均值向量
	//定义两个点云变量接收传入的点云指针
	pcl::PointCloud<PointIoType>::Ptr p_sour(new pcl::PointCloud<PointIoType>())
									, p_dist(new pcl::PointCloud<PointIoType>());
	p_sour = *sour;
	p_dist = *dist;
	//通过for循环，遍历源点云中的所有点，
	//将每个点x、y坐标都减去两片点云之间的误差均值
	for (size_t i = 0; i < p_sour->points.size(); i++){
		p_sour->points[i].x = p_sour->points[i].x - D_value_vector.x();
		p_sour->points[i].y = p_sour->points[i].y - D_value_vector.y();
	}
	std::string path = "./288_1.pcd";
	//保存减去误差后的源点云
	pcl::io::savePCDFileBinaryCompressed<PointIoType>(path, *p_sour);
}

Eigen::Vector3d Process::Kdsearch(){
	// 定义一个点云变量，用于存储源点云中用于搜索的杆包围盒数据
	pcl::PointCloud<pcl::PointXYZL>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZL>);
	int size_of_sour_pole = sour_pole.size();
	int size_of_dist_pole = dist_pole.size();
	cloud->width = size_of_dist_pole;
	cloud->height = 1;
	cloud->points.resize(cloud->width * cloud->height);
	for (int i = 0; i < size_of_dist_pole; ++i)
	{
		cloud->points[i].x = dist_pole.at(i).x;
		cloud->points[i].y = dist_pole.at(i).y;
		cloud->points[i].z = dist_pole.at(i).z;
		cloud->points[i].label = 0;
	}
	float radius = 1.0f;
	int match_size = 0;
	pcl::KdTreeFLANN<pcl::PointXYZL> kdtree;
	pcl::PointXYZL searchPoint;
	std::vector<int> pointIdxRadiusSearch;
	std::vector<float> pointRadiusSquaredDistance;
	Eigen::Vector3d meanVector(0, 0, 0);
	kdtree.setInputCloud(cloud);
	//遍历源点云杆的中心点，搜索距离dist_pole点云最近的点
	for (int sour_index = 0; sour_index < size_of_sour_pole; sour_index++){
		searchPoint.x = sour_pole.at(sour_index).x;
		searchPoint.y = sour_pole.at(sour_index).y;
		searchPoint.z = sour_pole.at(sour_index).z;
		std::cout << "搜索点为" << searchPoint.x << " " << searchPoint.y << " " << searchPoint.z << std::endl;
		//筛选出符合条件的点云
		if (kdtree.radiusSearch(searchPoint, radius, pointIdxRadiusSearch, pointRadiusSquaredDistance) > 0){
			//筛选出没有匹配过的点
			if (cloud->points[pointIdxRadiusSearch[0]].label == 0){
				//筛选出距离小于1m的点云
				if (pointRadiusSquaredDistance[0]<1){
					std::cout << "匹配点为" << 
					cloud->points[pointIdxRadiusSearch[0]].x << " " <<
					cloud->points[pointIdxRadiusSearch[0]].y << " " <<
					cloud->points[pointIdxRadiusSearch[0]].z << " (距离: " <<
					pointRadiusSquaredDistance[0] << ")" << std::endl;
					//将该点标记为已匹配
					cloud->points[pointIdxRadiusSearch[0]].label = 1;
					//计算误差向量
					meanVector.x() = meanVector.x() + searchPoint.x - cloud->points[pointIdxRadiusSearch[0]].x;
					meanVector.y() = meanVector.y() + searchPoint.y - cloud->points[pointIdxRadiusSearch[0]].y;
					match_size += 1;
				}
				else{
					std::cout << "没有匹配的邻近点" << std::endl;
				}
			}
			else{
				std::cout << "没有匹配的邻近点" << std::endl;
			}

		}
	}
	//返回计算好的误差均值向量
	return meanVector / match_size;
}

void Process::txt_Reader(){
	FILE *fp_1;
	fp_1 = fopen("H:/pro_software_workplace/VisualStudio/Project1/Project1/feature.txt", "r");
	if (fp_1){
		for (int i = 0; i < SIZE_OF_FEATURES; ++i){
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
	for (int i = 0; i < SIZE_OF_FEATURES; i++){
		//筛选出杆的包围盒中心点云数据
		if (features[i].type == 206){
			//利用index索引，区分两片点云包围盒数据，并保存
			if (features[i].index == 1){
				sour_pole.push_back(features[i]);
			}
			else{
				dist_pole.push_back(features[i]);
			}
		}
	}
}



