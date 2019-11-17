#include"Process.h"//引用点云融合处理类的头文件

int main(int argc, char** argv){
	
	Process p;//实例化点云处理类
	p.txt_Reader();//读取包围盒数据
	p.execute();//执行配准
	return 0;
}
