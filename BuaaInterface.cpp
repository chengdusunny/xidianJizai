#include "BuaaInterface.h"

using namespace std;

void BuaaInterface_jizai::precisionPathPlan(char data_type[4], char task_id[8], int uav_num, iPosition* positions)
{
	for (int i = 0; i < uav_num; i++) {
		uavInfo* ua_info = new uavInfo(positions[i].UAV_ID);
		for (int j = 0; j < 10; j++) {
			ua_info->paths.push(_Axis(positions[i].x1[j], positions[i].y1[j], positions[i].z1[j]));
		}
		uav_map.emplace(ua_info->uav_id, ua_info);
	}
}


void BuaaInterface_jizai::getPathPlanInfo(string& data_type, string& task_id, Uavposition* uavpos)
{
	uavInfo* uav_info = uav_map[uavpos->UAV_ID];
	if (!uav_info->paths.empty()) {
		_Axis& location = uav_info->paths.front();
		uavpos->x1 = location.x;
		uavpos->y1 = location.y;
		uavpos->z1 = location.z;
		uav_info->paths.pop();
	}
}