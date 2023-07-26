#pragma once
#include <vector>
#include "iostream"
#include "string.h"

struct Coord
{
	float x;
	float y;
	float z;
};

float distance(Coord start, Coord end);


int is_in(std::vector<int> v, int element); // 2023/07/10


int find_elem(std::vector<int> v, int element); // 2023/07/10


void printVector(std::vector<float> v); // 2023/07/10


void printVector(std::vector<int> v); // 2023/07/10


void print_locs(std::vector <std::vector < float >> locs);


std::vector<float> coord2vec(Coord loc); //2023/07/13 -- wt

Coord vec2coord(std::vector<float> vec); // 为动态战情新增


Coord vec2coord_int(std::vector<int> vec); // 0711 revise int


std::vector<float> cumsum(std::vector<float> vec);

float vectorsum(std::vector<float>vect);

int vectorsum(std::vector<int>vect);


std::vector <std::vector<float>> sample_line(Coord start, Coord end, int seg_len);


std::vector<std::vector<float>>merge(std::vector<std::vector<float>>list1, std::vector<std::vector<float>>list2);

Coord loc_on_line(float lambda0, Coord start, Coord end);


//类

class Line_Segment
{
public:
	Coord start;
	Coord end;
	float length;
	std::vector<Coord> inserted_locs;
	Line_Segment(Coord start, Coord end)
	{
		this->start = start;
		this->end = end;
		this->length = distance(this->start, this->end);
	}
};

class Waypoints
{
public:
	std::vector<std::vector<float>> waypoints;
	std::vector<Line_Segment> segments;
	Coord total_start;
	Coord total_end;
	float total_length;
	Waypoints(std::vector<std::vector<float>> points) // points 采样前的点列
	{	
		if(points.size()<2)
		{
			std::cout <<"tao.h: Waypoints error! input points's size should be at least 2!" << std::endl;
		}
		this->total_start.x = points[0][0];
		this->total_start.y = points[0][1];
		this->total_start.z = points[0][2];
		this->total_end.x = points[points.size() - 1][0];
		this->total_end.y = points[points.size() - 1][1];
		this->total_end.z = points[points.size() - 1][2];
		Coord start;
		Coord end;
		float length = 0;
		for (int i = 0; i < points.size() - 1; i++)
		{
			start.x = points[i][0];
			start.y = points[i][1];
			start.z = points[i][2];
			//std::cout <<"位置1 " << points[i][0] << "  " << points[i][1] << std::endl;
			end.x = points[i + 1][0];
			end.y = points[i + 1][1];
			end.z = points[i + 1][2];
			//std::cout << "位置2 " << points[i+1][0] << "  " << points[i+1][1] << std::endl;
			Line_Segment line = Line_Segment(start, end);

			this->segments.push_back(line);
			length += line.length;
			//std::cout << "距离 " << line.length << std::endl;
		}
		this->total_length = length;
		this->waypoints = points;
	}

	void disp();

	std::vector<float> judge_which_segment(float dist);

	Coord get_loc(float path_len);

	std::vector<Coord> output_inserted_waypoints(); //2023/07/01 -- wt

	std::vector<Coord> sample_traj(std::vector<float> length_series); //2023/07/01 -- wt

};

// 2022/04/06
std::vector < std::vector < std::vector < float >> > trim_result(std::vector < std::vector < std::vector < float >> >aligned_result, float base_x, float base_y); // 最后三个点是基地点

std::vector<std::vector<float>> jizai_Sample(Waypoints waypoints, float sample_dist); //2023/07/01--wt

std::vector<std::vector<std::vector<float>>>Align(std::vector<std::vector<std::vector<float>>> input); // 把不等长的vector整合成等长度的
