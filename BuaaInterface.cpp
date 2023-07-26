#include <iostream>
#include <vector>
#include <string>
#include "BuaaInterface.h"

using namespace std;

void BuaaInterface_jizai::clear_uav_map()
{
	std::cout << "Clear the uav_map!" << std::endl;
	uav_map.clear();
}

void BuaaInterface_jizai::precisionPathPlan(char data_type[4], char task_id[8], int uav_num, vector<iPosition> positions)
{
	
	//std::cout << "precisionPathPlan: uav_map_size:" << uav_map.size() << " positions size:" << positions.size() << " uav_num:" << uav_num << std::endl;
	std::cout << "precisionPathPlan: uav_map_size:" << uav_map.size()<<std::endl;
	std::cout << "positions:" << std::endl;
	for (int i = 0; i < positions.size(); i++)
	{
		std::cout << "uav id = " << positions[i].UAV_ID << ", 10 points:" << std::endl;
		for (int j = 0; j < 10; j++)
			std::cout << positions[i].x1[j] << ' ' << positions[i].y1[j] << ' ' << positions[i].z1[j] << std::endl;
	}
	if (uav_map.size() < uav_num) {
		for (int i = 0; i < uav_num; i++) {
			uavInfo* ua_info = new uavInfo(positions[i].UAV_ID);
			for (int j = 0; j < 10; j++) {
				ua_info->paths.push(_Axis(positions[i].x1[j], positions[i].y1[j], positions[i].z1[j]));
			}
			uav_map.emplace(ua_info->uav_id, ua_info);
		}
	}
	else {
		for (int i = 0; i < uav_num; i++) {
			int uav_id = positions[i].UAV_ID;
			uavInfo* ua_info = uav_map[uav_id];
			for (int j = 0; j < 10; j++) {
				ua_info->paths.push(_Axis(positions[i].x1[j], positions[i].y1[j], positions[i].z1[j]));
			}
		}
	}
}

int BuaaInterface_jizai::get_uav_map_path_len() //07/26
{
	if (uav_map.size() == 0)
		return 0;
	else {
		for (auto iter = uav_map.begin(); iter != uav_map.end(); ++iter) {
			Uavposition pos;
			uavInfo* uav_info = iter->second;
			pos.UAV_ID = iter->first;
			return uav_info->paths.size();
		}
	}
}

void BuaaInterface_jizai::getPathPlanInfo(vector<Uavposition>& poss)
{
	bool flag = false;
	bool verbose = false;
	if (uav_map.size() == 0) {
		cout << "getPathPlanInfo: no uav!" << endl;
	}
	else {
		for (auto iter = uav_map.begin(); iter != uav_map.end(); ++iter) {
			Uavposition pos;
			uavInfo* uav_info = iter->second;
			pos.UAV_ID = iter->first;
			//cout << "pos.id: " << pos.UAV_ID << endl;
			if (uav_info->paths.empty()) {
				flag = true;
				break;
			}
			else {
				_Axis& loc = uav_info->paths.front();
				pos.x1 = loc.x, pos.y1 = loc.y, pos.z1 = loc.z;
				uav_info->paths.pop();
			}
			if (verbose) cout << "路径队列剩余长度：" << uav_info->paths.size() << endl;
			poss.emplace_back(pos);
		}
		if (flag) poss.clear();
	}

}

vector<int> BuaaInterface_jizai::get_uav_id()
{
	vector<int> uav_ids;
	for (auto it = uav_map.begin(); it != uav_map.end(); ++it)
	{
		uav_ids.push_back(it->second->uav_id);
	}
	return uav_ids;
}

int BuaaInterface_jizai::get_uav_num()
{
	return uav_map.size();
}