#pragma once
#include<string.h>

#define MAX_BUF 1024 * 2
#define PORT 10001
#define IP 192.168.40.1

#define SENDIP "127.0.0.1"
#define SENDPORT 10004


struct iPlatState //表示无人机状态
{
	float speed[3];//X Y Z轴速度分量
	float capacity;//此刻剩余战力值
	float max_speed;//最大速度
	float battery;//剩余电量/油量
	int available;//执行/未在执行
	float comm_power;//发射机功率
	float band[2];// x y通信频段
	float comm_demand[2];//[hearerID,demand] 簇头ID和header通信业务需求
};

struct platforms  //平台信息
{
	int Plat_ID;
	int Plat_Type;
	int x1; int y1; int z1;
	iPlatState PlateState;
};

struct obstacles //障碍物信息
{
	int ob_ID;
	int ob_Type;
	float x1; float y1; float z1;
	float Range;
};

struct target_area //目标区域
{
	int x1; int y1; int z1;
	int x2; int y2; int z2;
	int x3; int y3; int z3;
	int x4; int y4; int z4;
};



struct iPosition //无人机路径坐标
{
	int UAV_ID;//无人机代号
	float x1[10];//同一架无人机的10个点位
	float y1[10];
	float z1[10];
};



struct Uavposition //无人机坐标
{
	int UAV_ID;//无人机代号
	int x1;//无人机x、y、z三个方向上的位置
	int y1;
	int z1;
};



struct UAVState //无人机的状态
{
	int UAVid;
	int State;
};


struct Dynamic_Obstacles  //新增障碍物信息
{
	int ob_ID;
	int ob_Type;
	float x1; float y1; float z1;
	float Range;
	float speed[3];//增加速度属性
};

struct targets
{
	int nTgtID; //任务的id
	int nTgtType; //任务的类型type
	float x1; float y1; float z1; //这个是位置
	int x2; //这个x2y2z2是速度,先不考虑目标的移动了, 统一是0
	int y2;
	int z2;
};


struct iTgState //目标状态信息
{
	float demand;//当前需求战力值
	float speed[3];//X Y Z轴速度分量
};

struct Area_info
{
	// float x; 03/27修改
	// float y; 03/27修改
	// float z; 03/27修改
	int x;
	int y;
	int z;
	int length;
	int width;
};

//精确任务下发中心任务规划分系统接口 接受数据结构
struct ZQKZR3
{
	char datatype[4];//数据类型代号
	char taskID[8];//任务代号
	int num_uav;//无人机数量
	int num_obs;//障碍物数量
	int num_target;//目标数量
	int uav_speed; //无人机速度
};
//挂载 Area_info* area; // 感知区域位置信息
//挂载 platforms* Platforms;//平台信息 和无人机数量一致
//挂载 obstacles* Obstacles;//障碍物信息 和障碍物数量一致
//挂载 targets* Targets;//目标信息 和目标数量一致
//挂载 iTgState* iTargetsl//目标状态信息 和目标数量一致

//精确任务下发中心任务规划分系统接口 接收数据结构
struct ZQTJRJ
{
	char datatype[4];//数据类型代号
	char taskID[8];//任务代号
	int num_uav;//无人机数量
	int num_obs;//障碍物数量
	int num_target;//目标数量
	int uav_speed; // 03/27 修改, 新增uav_speed属性
};
//03/27修改
//挂载 Area_info* area; // 感知区域位置信息
//挂载 platforms* Platforms;//平台信息 和无人机数量一致
//挂载 obstacles* Obstacles;//障碍物信息 和障碍物数量一致
//挂载 targets* Targets;//目标信息 和目标数量一致
//挂载 iTgState* iTargetsl//目标状态信息 和目标数量一致


//路径规划信息接口 接收数据结构
struct ZQTJRD
{
	char datatype[4];//数据类型代号
	char taskID[8];//任务代号
	int num_uav; // 无人机数量
};
//挂载 iPosition* positions;//无人机路径规划信息 和无人机数量一致

//上报路径点信息 数据结构
struct ZQJRTE
{
	char datatype[4];
	char taskID[8];
	Uavposition uavpos;
};
//取消挂载 Uavposotion* uavpos;

struct RECEIVEACK
{
	char datatype[4]; // JSQR
	char R_datatype[4]; // KZR3
	char ackcontent[3]; // ACK
};


//下发动态战情接口 接受数据结构
struct ZQKALD
{
	char datatype[4];	// 数据类型代号
	char taskID[8];		// 任务代号
	int num_obs;		// 新增障碍物数量
	int num_uav;		// 状态变化无人机数量
	int num_target;		// 目标数量
};
//挂载 Dynamic_Obstacles* dObstacles;		// 新增障碍物信息 和新增障碍物数量一致
//挂载 UAVState* uavStates;					// 状态变化无人机信息 和状态变化无人机数量一致
//挂载 targets* Targets;						// 目标编号类型位置信息 和目标数量一致
//挂载 iTgState* iTargets;					// 目标状态信息 和目标数量一致

//预规划路径信息下发通信仿真分系统接口 发送数据结构
//每5秒发送每个无人机规划后的十个路径点
struct ZQZRT4
{
	char datatype[4];//数据类型代号
	char taskID[8];//任务代号
	int num_uav;//无人机数量
};
//挂载 iPosition* positions;//无人机路径规划信息 和无人机数量一致
