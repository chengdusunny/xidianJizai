#include <iostream>
#include <string>
#include <WinSock2.h>
#include <Windows.h>
#include "BuaaInterface.h"
#include "common.h"
#include "Timer.h"

#pragma warning(disable:4996)

//void print_iPosition(std::vector<iPosition>& pos) {
//	for (auto& p : pos) {
//		std::cout << p.UAV_ID << std::endl;
//		for (int i = 0; i < 10; ++i)
//			std::cout << i << ": " << p.x1[i] << " " << p.y1[i] << " " << p.z1[i] << std::endl;
//	}
//}

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

	char* buf = new char[MAX_BUF];
	int len = sizeof(recvAddr);

	//接受精确任务下发数据，作为初始化数据，且仅接受一次
	std::cout << "-----态势初始化数据等待接受-----" << std::endl;
	memset(buf, 0, MAX_BUF);
	int ret = recvfrom(serSoc, buf, MAX_BUF, 0, (sockaddr*)&recvAddr, &len);
	if (ret <= 0) {
		std::cout << "recefrom ERROR" << WSAGetLastError() << std::endl;
	}
	else {
		std::cout << "-----态势初始化数据接受中-----" << std::endl;
		ZQTJRJ* data = new ZQTJRJ;
		char* p = buf;
		memcpy(data, p, sizeof(ZQTJRJ));
		uav_num = data->num_uav;
		//03/27修改-----------
		p += sizeof(ZQTJRJ);
		Area_info* area = new Area_info;
		memcpy(area, p, sizeof(Area_info));
		p += sizeof(Area_info);

		platforms* Platforms = new platforms[data->num_uav];
		memcpy(Platforms, p, sizeof(platforms) * data->num_uav);
		p += sizeof(platforms) * data->num_uav;

		obstacles* Obstalces = new obstacles[data->num_obs];
		memcpy(Obstalces, p, sizeof(obstacles) * data->num_obs);
		p += sizeof(obstacles) * data->num_obs;
		
		targets* target = new targets[data->num_target];
		memcpy(target, p, sizeof(targets) * data->num_target);
			p += sizeof(targets) * data->num_target;
		//03/27修改-----------
	}

	//上报通信仿真分系统
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
		std::cout << "通信仿真分系统Send Socket success" << std::endl;

	//上报机载视觉感知分系统
	sockaddr_in sendAddr_sj;
	sendAddr_sj.sin_family = AF_INET;
	sendAddr_sj.sin_port = htons(10014); // 机载视觉感知分系统端口
	sendAddr_sj.sin_addr.S_un.S_addr = inet_addr("192.168.40.14"); // 机载视觉感知分系统ip
	SOCKET sendSoc_sj = socket(AF_INET, SOCK_DGRAM, 0);
	if (sendSoc_sj == SOCKET_ERROR) {
		std::cout << "机载视觉感知分系统Socket Error=" << WSAGetLastError() << std::endl;
		closesocket(sendSoc_sj);
		WSACleanup();
		return 1;
	}
	else
		std::cout << "机载视觉感知分系统Send Socket success" << std::endl;

	//上报机载频谱感知分系统
	sockaddr_in sendAddr_pp;
	sendAddr_pp.sin_family = AF_INET;
	sendAddr_pp.sin_port = htons(10013); // 机载频谱感知分系统端口
	sendAddr_pp.sin_addr.S_un.S_addr = inet_addr("192.168.40.13"); // 机载频谱感知分系统ip
	SOCKET sendSoc_pp = socket(AF_INET, SOCK_DGRAM, 0);
	if (sendSoc_pp == SOCKET_ERROR) {
		std::cout << "机载频谱感知分系统Socket Error=" << WSAGetLastError() << std::endl;
		closesocket(sendSoc_pp);
		WSACleanup();
		return 1;
	}
	else
		std::cout << "机载频谱感知分系统Send Socket success" << std::endl;
	
	Sleep(2000); // 延时2s
	while (true) {
		//接收路径规划信息
		std::cout << "-------路径规划信息接收等待-------" << std::endl;
		int ret = recvfrom(serSoc, buf, MAX_BUF, 0, (sockaddr*)&recvAddr, &len);
		if (ret <= 0) {
			std::cout << "recefrom ERROR" << WSAGetLastError() << std::endl;
		}
		else {
			std::cout << "-----路径规划信息接收中-----" << std::endl;
			ZQTJRD* data = new ZQTJRD;
			char* p = buf;
			memcpy(data, buf, sizeof(ZQTJRD));
			p += sizeof(ZQTJRD);
			iPosition* positions = new iPosition[uav_num];
			memcpy(positions, p, sizeof(iPosition) * uav_num);

			b_interface->precisionPathPlan(data->datatype, data->taskID, uav_num, positions);

			// 每过0.5s，每架无人机上报路径点信息
			char send_buf[MAX_BUF];
			// 开启定时器
			Timer tSendData([&]() {
				std::cout << "上报位置信息" << std::endl;
			for (int i = 0; i < uav_num; i++) {
				memset(send_buf, 0, MAX_BUF);
				char* p_send = send_buf;
				ZQJRTE* data = new ZQJRTE;
				memcpy(data->datatype, "JRTE", 4);
				memcpy(data->taskID, "GZRW0001", 8);
				std::string data_type = "";
				std::string task_id = "";
				Uavposition* uavpos = new Uavposition;
				uavpos->UAV_ID = i;
				b_interface->getPathPlanInfo(data_type, task_id, uavpos);
				data->uavpos = uavpos;
				memcpy(p_send, data, sizeof(ZQJRTE));
				p_send += sizeof(ZQJRTE);
				//上报通信仿真分系统
				int ret = sendto(sendSoc, send_buf, MAX_BUF, 0, (sockaddr*)&sendAddr, sizeof(sendAddr));
				//上报视觉感知分系统
				int ret_sj = sendto(sendSoc_sj, send_buf, MAX_BUF, 0, (sockaddr*)&sendAddr_sj, sizeof(sendAddr_sj));
				//上报频谱感知分系统
				int ret_pp = sendto(sendSoc_pp, send_buf, MAX_BUF, 0, (sockaddr*)&sendAddr_pp, sizeof(sendAddr_pp));
				if (ret > 0)
					std::cout << "路径信息上报成功" << std::endl;
				else
					std::cout << "路径信息上报失败" << std::endl;
			}
			});
			tSendData.setSingleShot(false);
			tSendData.setInterval(Timer::Interval(500)); // 每0.5s上报一次
			tSendData.start(true);
			std::cout << "位置信息上报完成" << std::endl;
			delete[] positions;
		}
	}
	return 0;
}