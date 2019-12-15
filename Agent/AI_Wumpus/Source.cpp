#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
/*
----Outline---
*********Rule Of Reasoning**********
1/If one cell receive potential_pit and partial_wumpus event at the same time, these two event will automatically terminated
leaving this cell become safe cell
2/Cell X will become full_pit cell if all of the surrounding cells of its successor(except X) are trivial("","explored",out
side of the map)
3/
*/
//string file_map[10];
using namespace std;
string environment_map[100][100];
string map_state[100][100];
bool explored[100][100];
bool is_shot[100][100];
int dx[4] = {-1,0,0,1};//horizontal direction
int dy[4] = { 0,-1,1,0};//vertical direction
int ways_to_home[100][100];
const int limited_step = 40;
const int arrow_point = -100;
const int gold_point = 100;
const int out_cave_point = 10;
struct cell {
	int x;
	int y;
};
cell start_state = cell{ 0,0 };
bool inside_map(cell current_cell)
{
	//checking whether current_cell is inside map of 10x10
	if (current_cell.x >= 0 && current_cell.x < 10 && current_cell.y >= 0 && current_cell.y < 10)
		return true;
	return false;
}
bool read_map(string file_path)
{
	//reading map from input file
	ifstream file;
	file.open(file_path);
	if (!file) {
		return false;
	}
	string input, output;
	for (int i = 0; i < 10; i++) {
		getline(file, input);
		stringstream X(input);
		int count = 0;
		while (getline(X, output, '.')) {
			environment_map[i][count] = output;
			if (environment_map[i][count] == "A")
				start_state = cell{ i,count };
			count += 1;
		}
	}
	return true;
}
void shoot_cell(cell current_cell, int& current_point)
{
	if (inside_map(current_cell) && explored[current_cell.x][current_cell.y] == false)
	{
		if (!is_shot[current_cell.x][current_cell.y])
		{
			cout << "Shooting arrow at (" << current_cell.x << "," << current_cell.y << ")" << endl;
			current_point += arrow_point;//shoot arrow;
			cout << "Current point:" << current_point << endl;
			is_shot[current_cell.x][current_cell.y] = true;
			int start_pos = environment_map[current_cell.x][current_cell.y].find("W");
			if (start_pos != string::npos)
			{
				environment_map[current_cell.x][current_cell.y].erase(start_pos, 1);
				if (environment_map[current_cell.x][current_cell.y] == "")
					environment_map[current_cell.x][current_cell.y] = "-";
			}
			if(map_state[current_cell.x][current_cell.y] != "potential_pit"&& map_state[current_cell.x][current_cell.y] != "absolute_pit")
				map_state[current_cell.x][current_cell.y] = "safe";
		}
	}
}
void reasoning_gold(cell current_cell, int& current_point)
{
	current_point += gold_point;
	cout << "Grabbing gold => Current point: " << current_point << endl;
}
void reasoning_safe(cell current_cell)
{
	if (inside_map(current_cell))
	{
		map_state[current_cell.x][current_cell.y] = "safe";
	}
}
void reasoning_ok(cell current_cell)
{
	cell bottom_cell = cell{ current_cell.x,current_cell.y + 1 };
	cell left_cell = cell{ current_cell.x - 1,current_cell.y };
	cell right_cell = cell{ current_cell.x + 1,current_cell.y };
	cell top_cell = cell{ current_cell.x,current_cell.y -1 };
	reasoning_safe(top_cell);
	reasoning_safe(left_cell);
	reasoning_safe(right_cell);
	reasoning_safe(bottom_cell);
}
void casting_full_pit(cell current_cell)
{
	if (inside_map(current_cell) && explored[current_cell.x][current_cell.y] == false)
	{
		if (map_state[current_cell.x][current_cell.y] == "potential_pit")
		{
			map_state[current_cell.x][current_cell.y] = "absolute_pit";
		}
	}
}
void reasoning_pit(cell current_cell, int& count)//potentially containing pit in current_cell
{
	if (inside_map(current_cell) && explored[current_cell.x][current_cell.y] == false)
	{
		if (map_state[current_cell.x][current_cell.y] != "safe")
		{
			if (map_state[current_cell.x][current_cell.y] == "")
				map_state[current_cell.x][current_cell.y] = "potential_pit";
			count += 1;
		}
	}
}
void reasoning_breeze(cell current_cell)
{
	cell bottom_cell = cell{ current_cell.x,current_cell.y + 1 };
	cell left_cell = cell{ current_cell.x - 1,current_cell.y };
	cell right_cell = cell{ current_cell.x + 1,current_cell.y };
	cell top_cell = cell{ current_cell.x,current_cell.y - 1 };
	int count = 0;
	reasoning_pit(top_cell, count);
	reasoning_pit(left_cell, count);
	reasoning_pit(right_cell, count);
	reasoning_pit(bottom_cell, count);
	if (count == 1)//only one partial_pit cell => full_pit cell
	{
		casting_full_pit(top_cell);
		casting_full_pit(left_cell);
		casting_full_pit(right_cell);
		casting_full_pit(bottom_cell);
	}
}
void reasoning_stench(cell current_cell, int& current_point)
{
	cell bottom_cell = cell{ current_cell.x,current_cell.y + 1 };
	cell left_cell = cell{ current_cell.x - 1,current_cell.y };
	cell right_cell = cell{ current_cell.x + 1,current_cell.y };
	cell top_cell = cell{ current_cell.x,current_cell.y - 1 };
	shoot_cell(top_cell, current_point);
	shoot_cell(left_cell, current_point);
	shoot_cell(right_cell, current_point);
	shoot_cell(bottom_cell, current_point);
	int start_pos = environment_map[current_cell.x][current_cell.y].find("S");
	environment_map[current_cell.x][current_cell.y].erase(start_pos, 1);
	if (environment_map[current_cell.x][current_cell.y] == "")
		environment_map[current_cell.x][current_cell.y] = "-";
}
void reasoning_abstract(cell current_cell, int& current_point)
{
	if (environment_map[current_cell.x][current_cell.y].find("B") != string::npos)
	{
		cout << "Detecting breeze at (" << current_cell.x << " " << current_cell.y << endl;
		reasoning_breeze(current_cell);
	}
	if (environment_map[current_cell.x][current_cell.y].find("S") != string::npos)
	{
		cout << "Detecting stench at (" << current_cell.x << " " << current_cell.y << endl;
		reasoning_stench(current_cell, current_point);
	}
	if (environment_map[current_cell.x][current_cell.y].find("G") != string::npos)
	{
		cout << "Detecting gold at (" << current_cell.x << " " << current_cell.y << endl;
		reasoning_gold(current_cell, current_point);
		int start_pos = environment_map[current_cell.x][current_cell.y].find("G");
		environment_map[current_cell.x][current_cell.y].erase(start_pos, 1);
		if (environment_map[current_cell.x][current_cell.y] == "")
			environment_map[current_cell.x][current_cell.y] = "-";
	}
	if (environment_map[current_cell.x][current_cell.y] == "-")
	{
		cout << "Detecting nothing at (" << current_cell.x << " " << current_cell.y << endl;
		reasoning_ok(current_cell);
	}
}
void go_next_cell(cell current_cell, int& current_point,bool &finish)
{
	//exploring adjacent cell
	int key;
	cout << "Enter to go" << endl;
	cin >> key;
	cout << "Go to cell (" << current_cell.x <<","<< current_cell.y << ")" << endl;
	cout << "Ways to back home from current cell:" << ways_to_home[current_cell.x][current_cell.y] << endl;
	reasoning_abstract(current_cell, current_point);
	for (int i = 0; i < 4; i++)
	{
		cell next_cell = cell{ current_cell.x + dx[i],current_cell.y + dy[i] };
		if (inside_map(next_cell))
		{
			if (explored[next_cell.x][next_cell.y] == false && map_state[next_cell.x][next_cell.y] == "safe")
			{
				explored[next_cell.x][next_cell.y] = true;
				ways_to_home[next_cell.x][next_cell.y] = ways_to_home[current_cell.x][current_cell.y] + 1;
				if (ways_to_home[next_cell.x][next_cell.y] == limited_step / 2)
				{
					finish = true;
					return;
				}
				go_next_cell(next_cell, current_point,finish);
				cout<<"Back to cell(" << current_cell.x << "," << current_cell.y << ")" << endl;
				if (finish)
					return;
			}
		}
	}
}
void play_games(cell start_cell)
{
	//playing games
	int point = 0;
	environment_map[start_cell.x][start_cell.y] = "-";
	bool finish = false;
	go_next_cell(start_cell, point,finish);
}
int main()
{
	string path = "map1.txt";
	if (read_map(path)) {
		/*for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				cout << environment_map[i][j] << " ";
			}
			cout << endl;
		}*/
		play_games(start_state);
	}
	else cout << "Fail to read file";
	return 0;
}