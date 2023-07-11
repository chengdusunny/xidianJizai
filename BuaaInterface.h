#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>

#include "common.h"


class BuaaInterface_jizai
{
public:
	BuaaInterface_jizai() {}

	//路径规划信息下发机载任务规划分系统接口
	void precisionPathPlan(char data_type[4], char task_id[8], int uav_num, std::vector<iPosition> positions);
	//下发动态战情接口
	//void dynamicInfoAssign(char data_type[4], char task_id[8], int new_obs_num, Dynamic_Obstacles* new_obs, int uav_num, UAVState* uav_info);
	//预规划路径信息下发通信仿真分系统接口
	void getPathPlanInfo(std::vector<Uavposition>& poss);
	//清空uav_map
	void clear_uav_map();//2023/04/17
	int get_uav_map_path_len();//2023/04/18
	//中心任务规划分系统上报返航完成接口
	//void taskCompleted(std::string& data_type, std::string& task_id, int& order);
	struct _Axis {
	public:
		float x;
		float y;
		float z;

		_Axis(float _x, float _y, float _z) :x(_x), y(_y), z(_z) {}
	};

private:
	
	struct uavInfo {
	public:
		int uav_id;

		std::queue<_Axis> paths;

	public:
		uavInfo(int _uav_id)
			: uav_id(_uav_id){}
	};

	struct targetInfo {
	public:
		int target_id;
		int target_type;
		float demand;
		float x;
		float y;
		float z;

	public:
		targetInfo(int _target_id, int _target_type, float _demand, float _x, float _y, float _z)
			:target_id(_target_id), target_type(_target_type), demand(_demand), x(_x), y(_y), z(_z) {}
	};

	struct obsInfo {
	public:
		int obs_id;
		float x;
		float y;
		float z;
		float radius;

	public:
		obsInfo(int _obs_id, float _x, float _y, float _z, float _radius)
			:obs_id(_obs_id), x(_x), y(_y), z(_z), radius(_radius) {}
	};

	struct cmp {
		bool operator() (std::pair<int, int>& a, std::pair<int, int>& b) {
			return a.first < b.first;
		}
	};

private:
	std::unordered_map<int, uavInfo*> uav_map;
	std::unordered_map<int, targetInfo*> target_map;
	std::unordered_map<int, obsInfo*> obs_map;
	std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, cmp> heap;
	int index = 0;
};

