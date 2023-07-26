#include <vector>
#include <iostream>
#include "tao.h"

using namespace std;

float distance(Coord start, Coord end)
{
	Coord Delta;
	Delta.x = end.x - start.x;
	Delta.y = end.y - start.y;
	Delta.z = end.z - start.z;
	float result = sqrt(Delta.x * Delta.x + Delta.y * Delta.y + Delta.z * Delta.z);
	return result;
};


int is_in(std::vector<int> v, int element) // 2023/07/10
{
	if (v.size() == 0) return 0;
	int flag = 0;
	for (int i = 0; i < v.size(); i++)
	{
		if (v[i] == element)
		{
			flag = 1;
			break;
		}
	}return flag;
}

int find_elem(std::vector<int> v, int element) // 2023/07/10
{
	for (int i = 0; i < v.size(); i++)
	{
		if (v[i] == element)
		{
			return i;
		}
	} return -1;
}

void printVector(std::vector<float> v) // 2023/07/10
{
	for (int i = 0; i < v.size(); i++)
		std::cout << v[i] << ",";
	std::cout << std::endl;
}

void printVector(std::vector<int> v) // 2023/07/10
{
	for (int i = 0; i < v.size(); i++)
		std::cout << v[i] << ",";
	std::cout << std::endl;
}

void print_locs(std::vector <std::vector < float >> locs)
{
	if (locs.size() == 0)
	{
		std::cout << "print_locs: error: length=0" << std::endl;
	}
	else {
		std::cout << " ";
		for (int i = 0; i < locs.size(); i++)
		{
			printVector(locs[i]);
		}
		std::cout << std::endl;
	}
}

std::vector<float> coord2vec(Coord loc) //2023/07/13 -- wt
{
	std::vector<float> loc_vec = { loc.x, loc.y, loc.z };
	return loc_vec;
}

Coord vec2coord(std::vector<float> vec) // 为动态战情新增
{	
	if (vec.size() != 3)
	{
		std::cout << "vec2coord: vec's len != 3" << std::endl;
	}
	Coord result;
	result.x = vec[0];
	result.y = vec[1];
	result.z = vec[2];
	return result;//07/15修
}

Coord vec2coord_int(std::vector<int> vec) // 0711 revise int
{	
	if (vec.size() != 3)
	{
		std::cout << "vec2coord_int: vec's len != 3" << std::endl;
	}
	printVector(vec);
	Coord result;
	result.x = vec[0];
	result.y = vec[1];
	result.z = vec[2];
	return result;//07/15修
}

std::vector<float> cumsum(std::vector<float> vec)
{
	/*std::cout << "cumsum input is: " <<std::endl;
	for (int i = 0; i < vec.size(); i++)
	{
		std::cout <<vec[i] << std::endl;
	}*/
	std::vector<float> vec1 = vec;
	for (int i = 1; i < vec.size(); i++)
	{
		vec1[i] = vec1[i - 1] + vec1[i];
	}
	/*std::cout << "cumsum result is: " << std::endl;
	for (int i = 0; i < vec1.size(); i++)
	{
		std::cout << vec1[i] << std::endl;
	}*/
	return vec1;

}

float vectorsum(std::vector<float>vect)
{
	float result = 0;
	for (int i = 0; i < vect.size(); i++)
	{
		result += vect[i];
	}
	return result;
}

int vectorsum(std::vector<int>vect)
{
	int result = 0;
	for (int i = 0; i < vect.size(); i++)
	{
		result += vect[i];
	}
	return result;
}

std::vector <std::vector<float>> sample_line(Coord start, Coord end, int seg_len)
{
	Coord Delta;
	Delta.x = end.x - start.x;
	Delta.y = end.y - start.y;
	Delta.z = end.z - start.z;
	float dist = distance(start, end);
	int point_num = int(dist / seg_len) + 1;
	std::vector<float> lambda_list(point_num, 0);
	for (int i = 0; i < point_num - 1; i++)
	{
		lambda_list[i] = seg_len / dist * i;
	}
	lambda_list[point_num - 1] = 1;
	std::vector < std::vector<float>> points(point_num, std::vector<float>(3, 0));
	for (int i = 0; i < point_num; i++)
	{
		points[i][0] = start.x + Delta.x * lambda_list[i];
		points[i][1] = start.y + Delta.y * lambda_list[i];
		points[i][2] = start.z + Delta.z * lambda_list[i];
	}
	return points;
}

std::vector<std::vector<float>>merge(std::vector<std::vector<float>>list1, std::vector<std::vector<float>>list2)
{
	int m, n;
	m = list1.size();
	n = list2.size();
	for (size_t i = 0; i < n; i++)
		list1.push_back(list2[i]);
	return list1;
}

Coord loc_on_line(float lambda0, Coord start, Coord end)
{
	float delta_x = end.x - start.x;
	float delta_y = end.y - start.y;
	float delta_z = end.z - start.z;
	Coord result;
	result.x = start.x + lambda0 * delta_x;
	result.y = start.y + lambda0 * delta_y;
	result.z = start.z + lambda0 * delta_z;
	return result;
}

void Waypoints::disp()
{
	std::cout << "打印长度:" << std::endl;
	for (int i = 0; i < this->segments.size() - 1; i++)
	{
		std::cout << "长度:" << this->segments[i].length << std::endl;
	}
}

std::vector<float> Waypoints::judge_which_segment(float dist)
{
	std::vector<float> result(2, 0);
	if (dist >= this->total_length)
	{
		result[0] = this->segments.size();
		result[1] = 0;
		return result;
	}
	else
	{
		std::vector<float>distances(this->segments.size(), 0);
		for (int i = 0; i < this->segments.size(); i++)
		{
			distances[i] = this->segments[i].length;
		}
		distances = cumsum(distances);
		for (int iter = 0; iter < this->segments.size(); iter++)
		{
			if (dist <= distances[iter])
			{
				if (iter == 0)
				{
					result[0] = 0;
					result[1] = dist;
				}
				else {
					result[0] = iter;
					result[1] = dist - distances[iter - 1];
				}break;
			}
		}return result;
	}
}

Coord Waypoints::get_loc(float path_len)
{
	if (path_len >= this->total_length)
	{
		return this->total_end;
	}
	else {
		std::vector<float> judge = this->judge_which_segment(path_len);
		float i = int(judge[0]);
		float dist = judge[1];
		//std::cout << "get_loc: path_len= " << path_len<<" i= "<<i<<" dist= "<<dist << std::endl;
		if (dist == 0 || this->segments[i].length == 0)
		{
			Coord loc = loc_on_line(0, this->segments[i].start, this->segments[i].end);
			return loc;
		}
		else {
			Coord loc = loc_on_line(dist / this->segments[i].length, this->segments[i].start, this->segments[i].end);
			return loc;
		}
	}
}

std::vector<Coord> Waypoints::output_inserted_waypoints() //2023/07/01 -- wt
{
	std::vector<Coord> waypoints_list;
	for (int i = 0; i < this->segments.size(); i++)
	{
		waypoints_list.push_back(this->segments[i].start);
		if (this->segments[i].inserted_locs.size() > 0)
		{
			for (int j = 0; j < this->segments[i].inserted_locs.size(); j++)
				waypoints_list.push_back(this->segments[i].inserted_locs[j]);
		}
	}
	int n = this->segments.size();
	waypoints_list.push_back(this->segments[n - 1].end);
	return waypoints_list;
}

std::vector<Coord> Waypoints::sample_traj(std::vector<float> length_series) //2023/07/01 -- wt
{
	for (int i = 0; i < length_series.size(); i++)
	{
		std::vector<float> result = this->judge_which_segment(length_series[i]);
		float id = result[0];
		float j = result[1];
		if (id != this->segments.size())
		{
			this->segments[id].inserted_locs.push_back(this->get_loc(length_series[i]));
		}
	}
	return this->output_inserted_waypoints();
}

// 2022/04/06
std::vector < std::vector < std::vector < float >> > trim_result(std::vector < std::vector < std::vector < float >> >aligned_result, float base_x, float base_y) // 最后三个点是基地点
{
	if (aligned_result.size() == 0)
	{
		std::cout << "trim_result: input vector size=0, error!!!" << std::endl;
		return aligned_result;
	}
	else{
	float base_H = aligned_result[0][0][2]; // 基地高度
	int total_length = aligned_result[0].size() + 3;
	std::vector < float > base(3, 0);
	base[0] = base_x;
	base[1] = base_y;
	base[2] = base_H;
	int tail_length = 0; // 结尾长度
	for (int i = 0; i < 3; i++)
	{
		for (int id = 0; id < aligned_result.size(); id++)
		{
			aligned_result[id].push_back(base);
		}
	}

	for (int i = 0; i < total_length; i++)
	{
		//std::cout << "判断结尾倒数: " << i << "位" << std::endl;
		int temp = 1; // 是否全到了
		for (int id = 0; id < aligned_result.size(); id++) // 判断是否全部到达
		{
			Coord start;
			Coord end;
			start.x = aligned_result[id][total_length - 1 - i][0];
			start.y = aligned_result[id][total_length - 1 - i][1];
			start.z = aligned_result[id][total_length - 1 - i][2];
			end.x = base[0];
			end.y = base[1];
			end.z = base[2];
			if (distance(start, end) > 0.5)
			{
				//std::cout << "uav : " << i << ": " << distance(start, end) << std::endl;
				temp = 0;
				break;
			}
		}
		if (temp == 0)
		{
			break;
		}
		else
		{
			tail_length = tail_length + 1;
		}
	}
	//std::cout << "应当裁剪 " << tail_length - 3 << "个点" << std::endl;
	if (tail_length > 3)
	{
		int pop_num = tail_length - 3;
		for (int i = 0; i < pop_num; i++)
		{
			for (int id = 0; id < aligned_result.size(); id++) //遍历无人机
			{
				aligned_result[id].pop_back();
			}
		}
	}
	}
	return aligned_result;
}

// vector<vector<float>> jizai_Sample(Waypoints waypoints, float sample_dist)
// {
// 	float total_length = waypoints.total_length;
// 	int num_points = total_length / sample_dist + 5;//采样多少个点
// 	//  std::cout << "采样点数: " << num_points << std::endl;
// 	vector<vector<float>> result(num_points, vector<float>(3, 0));
// 	for (int i = 0; i < num_points; i += 1)
// 	{
// 		Coord loc = waypoints.get_loc(sample_dist * i);
// 		// std::cout <<"采样距离:"<< sample_dist * i << " 打印loc: " << loc.x << " " << loc.y << " " << loc.z << std::endl;
// 		result[i][0] = loc.x;
// 		result[i][1] = loc.y;
// 		result[i][2] = loc.z;
// 	}
// 	return result;
// }

std::vector<std::vector<float>> jizai_Sample(Waypoints waypoints, float sample_dist) //2023/07/01--wt
{
	float total_length = waypoints.total_length;
	int num_points = total_length / sample_dist + 5;//采样多少个点
	std::vector<float> len_series;
	for (int i = 0; i < num_points; i++)
	{
		len_series.push_back(sample_dist * i);
	}
	//std::cout << "jizai_sample: num_of_points" << num_points << std::endl;
	std::vector<Coord> sampled_traj = waypoints.sample_traj(len_series);
	//vector<vector<float>> result(num_points, vector<float>(3, 0));
	std::vector<std::vector<float>> result;
	for (int i = 0; i < sampled_traj.size(); i++)
	{
		Coord loc = sampled_traj[i];
		//std::cout <<"sample dist:"<< sample_dist * i << " loc: " << loc.x << " " << loc.y << " " << loc.z << std::endl;
		result.push_back(coord2vec(loc));
	}
	return result;
}

std::vector<std::vector<std::vector<float>>>Align(std::vector<std::vector<std::vector<float>>> input) // 把不等长的vector整合成等长度的
{
	if (input.size() == 0)
	{
		std::cout << "Align: input vector size=0, error!" << std::endl;
	}
	int max_size = 0;
	for (int i = 0; i < input.size(); i++)
	{
		if (input[i].size() > max_size)
		{
			max_size = input[i].size();
		}
	}
	// std::cout <<max_size << std::endl;
	std::vector<std::vector<std::vector<float>>>result;
	for (int i = 0; i < input.size(); i++)
	{
		if (input[i].size() < max_size)
		{
			int current_size = input[i].size();
			int append_num = max_size - input[i].size();
			for (int x = 1; x <= append_num; x++)
			{
				input[i].push_back(input[i][current_size - 1]);
			}
		}
	}
	////check
	//for (int i = 0; i < input.size(); i++)
	//{
	//	if (input[i].size() != max_size)
	//	{
	//		std::cout << input[i].size() << "align报错: 大小不一致" << std::endl;
	//	}
	//}
	return input;
}

