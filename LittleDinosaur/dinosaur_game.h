#include <iostream>
#include <windows.h>
#include <conio.h>
#include <ctime>
#include <vector>

using namespace std;

#define MAP_LENGTH 50 
#define MAP_HEIGHT 9 // 0-3 road, 4-8 sky
#define ROAD_SURFACE 3 

#define ROAD_DIRT_RATE 8 // dirt appearing rate is equal to 1/ROAD_DIRT_RATE

#define BARRIER_COLD_TIME 150

static bool gameover = false;


struct dino 
{
	// from 0(underground) to SKY_HEIGHT
	// when console_display, print it from the end to the begin
	vector<char> map[MAP_HEIGHT];

	int score;

	int per_iteration_time;// time for per frame

	int barrier_chance;
	int barrier_cold_time;

	// dinosaur's data
	int jump_power;

	int pos_x;
	int pos_y;

	bool is_jumping;
	int jump_power_left;
	int falling_speed;
};
typedef dino Dino;


Dino* init(void);

void console_display(Dino* MyDino);
void map_move(Dino* MyDino);

void dino_jump(Dino* MyDino);
bool space_pressed(void);

void spawn_barrier(Dino* MyDino);

bool is_bumped(Dino* MyDino);