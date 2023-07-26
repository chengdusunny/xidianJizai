#include <iostream>
#include <string>
#include<vector>
#include <WinSock2.h>
#include <Windows.h>
#include "common.h"
#include "Timer.h"
#include "iterator"
#include "BuaaInterface.h"
#include "tao.h"

#pragma warning(disable:4996)

bool hasArrived(std::vector<iPosition> positions, float base_x, float base_y) //判断10个位置点最后3个是否都到达
{
	bool result = false;
	std::vector<int> judge(10, 1);
	for(int t=0; t<10; t++)
	{ 
	for (int i=0;i<positions.size();i++)
	{
		Coord start;
		start.x = positions[i].x1[t];
		start.y = positions[i].y1[t];
		start.z = positions[i].z1[t];
		Coord end;
		end.x = base_x;
		end.y = base_y;
		end.z = positions[i].z1[t];
		if (distance(start, end) > 1)
		{
			judge[t] = 0;
			break;
		}
	}
	}
	//最后三个位置到达
	if (judge[9] == 1 && judge[8] == 1 && judge[7] == 1)
	{
		result = true;
	}
	return result;
}



int main() {
	std::cout << "-----------------机载任务规划分系统-------------------" << std::endl;
	WSAData  wsd;
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		std::cout << "WSAStart Error" << WSAGetLastError() << std::endl;
		return 0;
	}
	else
	{
		std::cout << "start Success" << std::endl;
	}

	BuaaInterface_jizai* b_interface = new BuaaInterface_jizai();
	int uav_num = 0; // 全局，所有未戰損的uav數量
	std::vector<int> uav_ids;
	sockaddr_in serAddr, recvAddr;

	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(10012); //接收路径规划信息
	serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	serAddr.sin_addr.S_un.S_addr = INADDR_ANY;

	SOCKET serSoc = socket(AF_INET, SOCK_DGRAM, 0);

	if (serSoc == SOCKET_ERROR) {
		std::cout << "Socket Error=" << WSAGetLastError() << std::endl;
		closesocket(serSoc);
		WSACleanup();
		return 1;
	}
	else {
		std::cout << "Socket success" << std::endl;
	}

	char opt = 1;
	setsockopt(serSoc, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	if (bind(serSoc, (sockaddr*)&serAddr, sizeof(serAddr)) == SOCKET_ERROR) {
		std::cout << "bind Error" << WSAGetLastError() << std::endl;
		closesocket(serSoc);
		WSACleanup();
		return 1;
	}
	else {
		std::cout << "bind success" << std::endl;
	}

	//上报通信仿真分系统(配置)
	sockaddr_in sendAddr;
	sendAddr.sin_family = AF_INET;
	sendAddr.sin_port = htons(SENDPORT); // 通信仿真分系统端口
	sendAddr.sin_addr.S_un.S_addr = inet_addr(SENDIP); // 通信仿真分系统ip
	SOCKET sendSoc = socket(AF_INET, SOCK_DGRAM, 0);
	if (sendSoc == SOCKET_ERROR) {
		std::cout << "通信仿真分系统Socket Error=" << WSAGetLastError() << std::endl;
		closesocket(sendSoc);
		WSACleanup();
		return 1;
	}
	else
		std::cout << "通信仿真分系统Socket success" << std::endl;

	//上报机载视觉感知分系统(配置)
	sockaddr_in sendAddr_sj;
	sendAddr_sj.sin_family = AF_INET;
	sendAddr_sj.sin_port = htons(10014); // 机载视觉感知分系统端口
	sendAddr_sj.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); // 机载视觉感知分系统ip

	SOCKET sendSoc_sj = socket(AF_INET, SOCK_DGRAM, 0);
	if (sendSoc_sj == SOCKET_ERROR) {
		std::cout << "机载视觉感知分系统Socket Error=" << WSAGetLastError() << std::endl;
		closesocket(sendSoc_sj);
		WSACleanup();
		return 1;
	}
	else
		std::cout << "机载视觉感知分系统 Socket success" << std::endl;

	//上报机载频谱感知分系统(配置)
	sockaddr_in sendAddr_pp;
	sendAddr_pp.sin_family = AF_INET;
	sendAddr_pp.sin_port = htons(10013); // 机载频谱感知分系统端口
	sendAddr_pp.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); // 机载频谱感知分系统ip
	SOCKET sendSoc_pp = socket(AF_INET, SOCK_DGRAM, 0);
	if (sendSoc_pp == SOCKET_ERROR) {
		std::cout << "机载频谱感知分系统Socket Error=" << WSAGetLastError() << std::endl;
		closesocket(sendSoc_pp);
		WSACleanup();
		return 1;
	}
	else
		std::cout << "机载频谱感知分系统 Socket success" << std::endl;

	// 每过2s, 每架无人机上报路径点信息
	char* send_buf = new char[MAX_BUF];
	bool exit_system = false; //2023/04/18 是否退出系统
	int uav_map_pathsize = 0;
	std::string task_id;
	task_id = "NULL0000"; //默认的8位task_id
	bool verbose_timer = true;
	// 开启定时器, 开始发送信息
	Timer tSendData([&]() {
		//std::cout << "上报位置信息..." << std::endl;
		std::vector<Uavposition> poss;
		b_interface->getPathPlanInfo(poss);
		std::cout << "main: timer: poss.size= " << poss.size() << std::endl;
		if (poss.size()!=0)
		{
			uav_map_pathsize = b_interface->get_uav_map_path_len();
			if (verbose_timer) std::cout << "uav_map 剩余长度:" << uav_map_pathsize << std::endl;
			if (uav_map_pathsize == 0)//2023/04/18 根据剩余点的个数判断是否退出系统
			{
				exit_system = true;
			}
			else { exit_system = false; }

			if (verbose_timer) std::cout << "poss size: " << poss.size() << std::endl;
			std::cout << poss.size() << std::endl;
			std::cout << uav_num << std::endl;
			//if (poss.size() == uav_num)
			if (uav_num > 0)			// 0726 revised
			{
				for (int i = 0; i < uav_num; i++) {
					memset(send_buf, 0, MAX_BUF); // 清空send buffer
					char* p_send = send_buf;
					ZQJRTE* data = new ZQJRTE;
					memcpy(data->datatype, "JRTE", 4);
					memcpy(data->taskID, task_id.c_str(), 8);
					Uavposition uavpos;
					uavpos.UAV_ID = poss[i].UAV_ID, uavpos.x1 = poss[i].x1, uavpos.y1 = poss[i].y1, uavpos.z1 = poss[i].z1;
					data->uavpos = uavpos;
					memcpy(p_send, data, sizeof(ZQJRTE));
					p_send += sizeof(ZQJRTE);

					if (verbose_timer) std::cout << "uav pos: " << uavpos.UAV_ID << ' ' << uavpos.x1 << ' ' << uavpos.y1 << ' ' << uavpos.z1 << std::endl;

					//上报通信仿真分系统
					int ret = sendto(sendSoc, send_buf, MAX_BUF, 0, (sockaddr*)&sendAddr, sizeof(sendAddr));
					//上报视觉感知分系统
					int ret_sj = sendto(sendSoc_sj, send_buf, MAX_BUF, 0, (sockaddr*)&sendAddr_sj, sizeof(sendAddr_sj));
					//上报频谱感知分系统
					int ret_pp = sendto(sendSoc_pp, send_buf, MAX_BUF, 0, (sockaddr*)&sendAddr_pp, sizeof(sendAddr_pp));
					Sleep(1);
					if (ret > 0)
					{
						if (verbose_timer)
							std::cout << "路径信息上报成功" << std::endl;
					}
					else
						std::cout << "路径信息上报失败" << std::endl;

					delete data;
				}
			}
		}
		});
	tSendData.setSingleShot(false);
	tSendData.setInterval(Timer::Interval(2000)); // 每2s上报一次
	tSendData.start(true);


	int n = 1;
	float base_x; //2023/04/17 接收起始点位置 2023/07/26 move here
	float base_y; //2023/04/17 接收起始点位置
	while(n)
	{
		std::cout << std::endl << "----------第 " << n << " 次战情----------" << std::endl << std::endl;
		bool task_success = false; //2023/04/17 是否接收完成
		char* buf = new char[1024*10];
		int len = sizeof(recvAddr);

		//接受精确任务下发数据，作为初始化数据，且仅接受一次
		std::cout << "-----态势初始化数据等待接收-----" << std::endl;
		memset(buf, 0, MAX_BUF);
		int ret = recvfrom(serSoc, buf, MAX_BUF, 0, (sockaddr*)&recvAddr, &len);
		if (ret <= 0)
		{
			std::cout << "recevfrom ERROR" << WSAGetLastError() << std::endl;
			continue;
		}
		else
		{
			std::cout << "-----态势初始化数据接收中-----" << std::endl;
			ZQTJRJ* data = new ZQTJRJ;
			char* p = buf;
			memcpy(data, p, sizeof(ZQTJRJ));
			std::cout << "无人机数量: " << data->num_uav << std::endl;
			uav_num = data->num_uav;
			task_id = data->taskID;
			//2023/04/18 判断收到信息的datatype
			if (strncmp(data->datatype, "TJRJ", 4) == 0)
				{
					std::cout << "机载解析到战情信息..." << std::endl;

					//03/27修改-----------
					p += sizeof(ZQTJRJ);
					Area_info* area = new Area_info;
					memcpy(area, p, sizeof(Area_info));
					p += sizeof(Area_info);

					platforms* Platforms = new platforms[data->num_uav];
					memcpy(Platforms, p, sizeof(platforms)* data->num_uav);
					uav_num = data->num_uav;//07/26 -- wt
					base_x = Platforms[0].x1; //2023/04/17 接收起始点位置
					base_y = Platforms[0].y1; //2023/04/17 接收起始点位置
					p += sizeof(platforms) * data->num_uav;
					for (int i = 0; i < data->num_uav; i++)
					{
						uav_ids.push_back(Platforms[i].Plat_ID);
					}
					obstacles* Obstalces = new obstacles[data->num_obs];
					memcpy(Obstalces, p, sizeof(obstacles)* data->num_obs);
					p += sizeof(obstacles) * data->num_obs;

					targets* target = new targets[data->num_target];
					memcpy(target, p, sizeof(targets)* data->num_target);
					p += sizeof(targets) * data->num_target;

					iTgState* iTargets = new iTgState[data->num_target];
					memcpy(iTargets, p, sizeof(iTgState)* data->num_target);
					//03/27修改-----------
					std::cout << std::endl << "----------态势数据接收成功------------" << std::endl << std::endl;
				}
			else if (strncmp(data->datatype, "KALD", 4) == 0)
			{
				std::cout << "接收到动态ZQ信息" << std::endl; //  新增解析 2023/07/26
				ZQKALD* data = new ZQKALD();
				char* p = buf;
				memcpy(data, p, sizeof(ZQKALD));
				p += sizeof(ZQKALD);

				std::cout << "jizai_main: DTZQ: ZQKALD: " << "datatype:" << data->datatype << " taskID:" << data->taskID << " obs: " << data->num_obs << " uav:" << data->num_uav << " target:" << data->num_target << std::endl;

				Dynamic_Obstacles* dObstacles = new Dynamic_Obstacles[data->num_obs];
				memcpy(dObstacles, p, sizeof(Dynamic_Obstacles) * data->num_obs);
				p += sizeof(Dynamic_Obstacles) * data->num_obs;

				for (int j = 0; j < data->num_obs; j++) // 新来的障碍物append进列表 2023/07/10 
				{
					obstacles obs_temp;
					obs_temp.ob_ID = dObstacles[j].ob_ID;
					obs_temp.ob_Type = dObstacles[j].ob_Type;
					obs_temp.x1 = dObstacles[j].x1;
					obs_temp.y1 = dObstacles[j].y1;
					obs_temp.z1 = dObstacles[j].z1;
				}

				for (int i = 0; i < data->num_obs; i++)
				{
					std::cout << "obs id: " << dObstacles[i].ob_ID << std::endl;
					std::cout << "obs Type: " << dObstacles[i].ob_Type << std::endl;
					std::cout << "loc: " << dObstacles[i].x1 << ' ' << dObstacles[i].y1 << ' ' << dObstacles[i].z1 << std::endl;
					std::cout << "range: " << dObstacles[i].Range << std::endl;
					std::cout << "speed: " << dObstacles[i].speed[0] << ' ' << dObstacles[i].speed[1] << ' ' << dObstacles[i].speed[2] << std::endl;
				}

				UAVState* uavStates = new UAVState[data->num_uav];
				memcpy(uavStates, p, sizeof(UAVState) * data->num_uav);
				p += sizeof(UAVState) * data->num_uav;
				for (int i = 0; i < data->num_uav; i++)
				{
					std::cout << "uav id: " << uavStates[i].UAVid << std::endl;
					std::cout << "uav state: " << uavStates[i].State << std::endl;
				}
				
				printVector(uav_ids);
				std::cout <<"減去戰損前uav_num="<< uav_num << std::endl;
				{
					std::vector<int> new_uav_ids;
					for (int i = 0; i < data->num_uav; i++)
					{
						if (is_in(uav_ids, uavStates[i].UAVid) && uavStates[i].State == 1)
						{
							uav_num = uav_num - 1;

						}
						else { new_uav_ids.push_back(uavStates[i].UAVid); }
					}
					uav_ids = new_uav_ids;
				}
				std::cout << "減去戰損后還剩uav_num="<< uav_num << std::endl;
				// 減去戰損最後得到的uav_num
				targets* Targets = new targets[data->num_target];
				memcpy(Targets, p, sizeof(targets) * data->num_target);
				p += sizeof(targets) * data->num_target;

				iTgState* iTargets = new iTgState[data->num_target];
				memcpy(iTargets, p, sizeof(iTgState) * data->num_target);
				p += sizeof(iTgState) * data->num_target;

				for (int i = 0; i < data->num_target; i++)
				{
					std::cout << "tgt id: " << Targets[i].nTgtID << std::endl;
					std::cout << "demand: " << iTargets[i].demand << std::endl;
					std::cout << "speed: " << iTargets[i].speed[0] << ' ' << iTargets[i].speed[1] << ' ' << iTargets[i].speed[2] << std::endl;
				} //07/26解析完成
			}
			else
			{
				std::cout << "机载解析到非战情信息，丢弃... " << std::endl;
				continue;
			}
		}
	
		Sleep(2000); // 延时2s

		// 循环等待接收路径规划信息
		// 先清空uav_map再接收dt战情
		b_interface->clear_uav_map(); //2023/04/18
		std::cout << std::endl << "----------开始进行路径规划------------" << std::endl << std::endl;
		std::cout << "清空uav_map等待接收位置信息..." << std::endl;
		bool verbose_path = FALSE;
		bool verbose_all_rec = false;
		while (true)
		{
			//接收路径规划信息
			if (verbose_all_rec) std::cout << "-------路径规划信息接收等待-------" << std::endl;
			memset(buf, 0, 1024*10);
			int ret = recvfrom(serSoc, buf, 1024*10, 0, (sockaddr*)&recvAddr, &len);
			if (ret <= 0)
			{
				std::cout << "recefrom ERROR" << WSAGetLastError() << std::endl;
			}
			else
			{
				if (verbose_all_rec) std::cout << "-------路径规划信息解析-------" << std::endl;
				char* p = buf;
				char data_type[5];
				memcpy(data_type, p, 4);
				data_type[4] = '\0';

				// 2023/07/26
				/*ZQTJRD* temp_data = new ZQTJRD;
				memcpy(temp_data, buf, sizeof(ZQTJRD));
				std::cout << temp_data->datatype << std::endl;*/

				if (strcmp(data_type, "KALD") == 0)
				{
					std::cout << "-------接收到动态ZQ信息，清空原规划路径-------" << std::endl; //  新增解析 2023/07/26
					b_interface->clear_uav_map();
					//以下為解析
					ZQKALD* data = new ZQKALD();
					char* p = buf;
					memcpy(data, p, sizeof(ZQKALD));
					p += sizeof(ZQKALD);

					std::cout << "jizai_main: DTZQ: ZQKALD: " << "datatype:" << data->datatype << " taskID:" << data->taskID << " obs: " << data->num_obs << " uav:" << data->num_uav << " target:" << data->num_target << std::endl;

					Dynamic_Obstacles* dObstacles = new Dynamic_Obstacles[data->num_obs];
					memcpy(dObstacles, p, sizeof(Dynamic_Obstacles)* data->num_obs);
					p += sizeof(Dynamic_Obstacles) * data->num_obs;

					for (int j = 0; j < data->num_obs; j++) // 新来的障碍物append进列表 2023/07/10 
					{
						obstacles obs_temp;
						obs_temp.ob_ID = dObstacles[j].ob_ID;
						obs_temp.ob_Type = dObstacles[j].ob_Type;
						obs_temp.x1 = dObstacles[j].x1;
						obs_temp.y1 = dObstacles[j].y1;
						obs_temp.z1 = dObstacles[j].z1;
					}

					for (int i = 0; i < data->num_obs; i++)
					{
						std::cout << "obs id: " << dObstacles[i].ob_ID << std::endl;
						std::cout << "obs Type: " << dObstacles[i].ob_Type << std::endl;
						std::cout << "loc: " << dObstacles[i].x1 << ' ' << dObstacles[i].y1 << ' ' << dObstacles[i].z1 << std::endl;
						std::cout << "range: " << dObstacles[i].Range << std::endl;
						std::cout << "speed: " << dObstacles[i].speed << std::endl;
					}

					UAVState* uavStates = new UAVState[data->num_uav];
					memcpy(uavStates, p, sizeof(UAVState) * data->num_uav);
					p += sizeof(UAVState) * data->num_uav;
					for (int i = 0; i < data->num_uav; i++)
					{
						std::cout << "uav id: " << uavStates[i].UAVid << std::endl;
						std::cout << "uav state: " << uavStates[i].State << std::endl;
					}
					
					{
						std::vector<int> new_uav_ids;
						for (int i = 0; i < data->num_uav; i++)
						{
							if (is_in(uav_ids, uavStates[i].UAVid) && uavStates[i].State == 1)
							{
								uav_num = uav_num - 1;

							}
							else { new_uav_ids.push_back(uavStates[i].UAVid); }
						}
						uav_ids = new_uav_ids;
					}
					std::cout << "減去戰損后還剩uav_num=" << uav_num << std::endl;
					// 減去戰損最後得到的uav_num
				}
				if (strcmp(data_type, "TJR9") == 0) {
					std::cout << "接收到接口9的開始命令" << std::endl;
					continue;
				}
				if (strcmp(data_type, "TJR5") != 0 && strcmp(data_type, "TJRD") != 0)
				{
					std::cout << "接收到其他信息" << std::endl;
					continue;
				}
				else
				{
					std::cout << "接收到路径规划信息" << std::endl;
				}
				ZQTJRD* data = new ZQTJRD;
				memcpy(data, p, sizeof(ZQTJRD));
				int uav_num1 = data->num_uav;
				if (uav_num1 == 0) continue;
				if (verbose_path) std::cout << "路径规划接收 uav_num: " << uav_num1 << std::endl;
				p += sizeof(ZQTJRD);
				iPosition* positions = new iPosition[uav_num1];
				memcpy(positions, p, sizeof(iPosition) * uav_num1);
				if (verbose_all_rec) std::cout << "接收路径规划信息..." << std::endl;
				for (int i = 0; i < data->num_uav; ++i) {
					if (verbose_path) std::cout << positions[i].UAV_ID << ":" << std::endl;
					for (int j = 0; j < 10; ++j) {
						if (verbose_path) std::cout << positions[i].x1[j] << " " << positions[i].y1[j] << " " << positions[i].z1[j] << std::endl;
					}
				}
				std::vector<iPosition> ipositions; // 加一个是否到达的判断 2023/04/17
				for (int aa = 0; aa < uav_num1; aa++)
				{
					ipositions.push_back(positions[aa]);
				}
				b_interface->precisionPathPlan(data->datatype, data->taskID, uav_num1, ipositions); //存储新收到的uav路径点
				task_success = hasArrived(ipositions, base_x, base_y); //加一个逻辑判断路径规划信息是否接收完毕
				if (verbose_all_rec) std::cout << "到达= "<< task_success << std::endl;
				if (task_success == true)
				{
					std::cout <<"接收到最后一个包..." << std::endl;
					std::cout << "--------------" << std::endl;
					break;
				}
				else
				{
					if (verbose_all_rec) std::cout << "继续接收..." << std::endl;
					if (verbose_all_rec) std::cout << "--------------" << std::endl;
				}
				delete[] positions;
			}
		}
    

		// n值减1
		n = n + 1;
	}

	//2023/04/18
	while (true)
	{
		Sleep(3000);
		std::cout << "点未报完，不退出系统..." << std::endl;
		if (exit_system == true)
		{
			std::cout << "点已报完，退出系统..." << std::endl;
			break;
		}
	}
	return 0;

}