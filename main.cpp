#include <iostream>
#include <string>
#include<vector>
#include <WinSock2.h>
#include <Windows.h>
#include "BuaaInterface.h"
#include "common.h"
#include "Timer.h"

#pragma warning(disable:4996)

struct Coord
{
	float x;
	float y;
	float z;
};

float distance(Coord start, Coord end)
{
	Coord Delta;
	Delta.x = end.x - start.x;
	Delta.y = end.y - start.y;
	Delta.z = end.z - start.z;
	float result = sqrt(Delta.x * Delta.x + Delta.y * Delta.y + Delta.z * Delta.z);
	return result;
};

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
	int uav_num = 0;

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
	bool verbose_timer = FALSE;
	// 开启定时器, 开始发送信息
	Timer tSendData([&]() {
		//std::cout << "上报位置信息..." << std::endl;
		std::vector<Uavposition> poss;
		b_interface->getPathPlanInfo(poss);
		uav_map_pathsize=b_interface->get_uav_map_path_len();

		std::cout << "uav_map剩余长度:"<< uav_map_pathsize << std::endl;

		if (uav_map_pathsize == 0)//2023/04/18 根据剩余点的个数判断是否退出系统
		{	
			exit_system = true;
		}
		else { exit_system = false; }

		if (verbose_timer) std::cout << "poss size: " << poss.size() << std::endl;

		if (poss.size() == uav_num) {
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
					//if (verbose_timer)
						std::cout << "路径信息上报成功" << std::endl;
				}
				else
					std::cout << "路径信息上报失败" << std::endl;

				delete data;
			}
		}
		});
	tSendData.setSingleShot(false);
	tSendData.setInterval(Timer::Interval(2000)); // 每2s上报一次
	tSendData.start(true);


	int n = 2;
	while(n)
	{
		std::cout << std::endl << "----------第 " << 3 - n << " 次战情----------" << std::endl << std::endl;
		bool task_success = false; //2023/04/17 是否接收完成
		char* buf = new char[1024*10];
		int len = sizeof(recvAddr);
		float base_x; //2023/04/17 接收起始点位置
		float base_y; //2023/04/17 接收起始点位置

		//接受精确任务下发数据，作为初始化数据，且仅接受一次
		std::cout << "-----态势初始化数据等待接收-----" << std::endl;
		memset(buf, 0, MAX_BUF);
		int ret = recvfrom(serSoc, buf, MAX_BUF, 0, (sockaddr*)&recvAddr, &len);
		if (ret <= 0)
		{
			std::cout << "recevfrom ERROR" << WSAGetLastError() << std::endl;
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
				}
			else
				{
				std::cout << "机载解析到非战情信息，丢弃... " << std::endl;
				continue;
				}
		
			//03/27修改-----------
			p += sizeof(ZQTJRJ);
			Area_info* area = new Area_info;
			memcpy(area, p, sizeof(Area_info));
			p += sizeof(Area_info);

			platforms* Platforms = new platforms[data->num_uav];
			memcpy(Platforms, p, sizeof(platforms) * data->num_uav);
			base_x = Platforms[0].x1; //2023/04/17 接收起始点位置
			base_y = Platforms[0].y1; //2023/04/17 接收起始点位置
			p += sizeof(platforms) * data->num_uav;

			obstacles* Obstalces = new obstacles[data->num_obs];
			memcpy(Obstalces, p, sizeof(obstacles) * data->num_obs);
			p += sizeof(obstacles) * data->num_obs;	
		
			targets* target = new targets[data->num_target];
			memcpy(target, p, sizeof(targets) * data->num_target);
			p += sizeof(targets) * data->num_target;

			iTgState* iTargets = new iTgState[data->num_target];
			memcpy(iTargets, p, sizeof(iTgState) * data->num_target);
			//03/27修改-----------
			std::cout << std::endl << "----------态势数据接收成功------------" << std::endl << std::endl;
		}
	
		Sleep(2000); // 延时2s

		// 循环等待接收路径规划信息
		// 先清空uav_map再接收战情
		b_interface->clear_uav_map(); //2023/04/18
		std::cout << std::endl << "----------开始进行路径规划------------" << std::endl << std::endl;
		std::cout << "清空uav_map等待接收位置信息..." << std::endl;
		bool verbose_path = FALSE;
		while (true)
		{
			//接收路径规划信息
			std::cout << "-------路径规划信息接收等待-------" << std::endl;
			memset(buf, 0, 1024*10);
			int ret = recvfrom(serSoc, buf, 1024*10, 0, (sockaddr*)&recvAddr, &len);
			if (ret <= 0)
			{
				std::cout << "recefrom ERROR" << WSAGetLastError() << std::endl;
			}
			else
			{
				std::cout << "-------路径规划信息解析-------" << std::endl;
				char* p = buf;
				char data_type[5];
				memcpy(data_type, p, 4);
				data_type[4] = '\0';
				if (strcmp(data_type, "KALD") == 0)
				{
					b_interface->clear_uav_map();
				}
				if (strcmp(data_type, "TJR9") == 0) {
					continue;
				}
				if (strcmp(data_type, "TJR5") != 0 && strcmp(data_type, "TJRD") != 0)
				{
					continue;
				}
				ZQTJRD* data = new ZQTJRD;
				memcpy(data, p, sizeof(ZQTJRD));
				int uav_num1 = data->num_uav;
				if (uav_num1 == 0) continue;
				std::cout << "路径规划接收 uav_num: " << uav_num1 << std::endl;
				p += sizeof(ZQTJRD);
				iPosition* positions = new iPosition[uav_num1];
				memcpy(positions, p, sizeof(iPosition) * uav_num1);
				std::cout << "接收路径规划信息..." << std::endl;
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
				std::cout << "到达= "<< task_success << std::endl;
				if (task_success == true)
				{
					std::cout <<"接收到最后一个包..." << std::endl;
					std::cout << "--------------" << std::endl;
					break;
				}
				else
				{
					std::cout << "继续接收..." << std::endl;
					std::cout << "--------------" << std::endl;
				}
				delete[] positions;
			}
		}
    

		// n值减1
		n = n - 1;
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