#include <iostream>
using namespace std;

#include <cstdlib>
#include <windows.h>
#include <conio.h> // keyboard input
#include <fstream>
#include <ctime>
#include <cmath>
#include <string>
#include <vector>


#pragma region definitions
// frame
#define FRAME_TIME 20

// keyboard input
#define ENTER 13
#define ESC 27
#define SPACE 32
// arrow key = 224 &..., use negative sign to represent arrow key input
#define UP -72
#define DOWN -80
#define LEFT -75
#define RIGHT -77

// map
#define MAP_LENGTH 50 
#define MAP_HEIGHT 10 // 0-3 road, 4-8 sky
#define ROAD_SURFACE 4 

#define ROAD_DIRT_RATE 30 // dirt appearing rate is equal to 1/ROAD_DIRT_RATE

// barrier
#define BARRIER_COLD_TIME 20

// dinosaur
#define JUMP_POWER_DECREMENT 1
#define FALLING_SPEED_INCREMENT 1

// default values
// dinosaur's
#define DEFAULT_BARRIER_CHANCE 3
#define DEFAULT_JUMP_POWER 2
#define DEFAULT_POS_X 6
#define DEFAULT_POS_Y 0
#define DEFAULT_FALLING_SPEED 1
#define DEFAULT_FALLING_DELAY 1

// others
#define DEFAULT_BARRIER_RANDOM_MAX_HEIGHT 3
#pragma endregion

// static value
static int frame = 0;


class Barrier
{
private:
	bool active_type;
	int max_height;

	bool flag = false;

public:
	int pos_x = MAP_LENGTH - 1;
	int pos_y;
	int height = 1;

	Barrier(int y, int max_h = 1, bool active_type = false)
	{
		pos_y = y;
		height = max_h;
		max_height = max_h;
		this->active_type = active_type;
	}

	void move()
	{
		--pos_x;
	}

	void deform()
	{
		if (active_type)
		{
			if (flag)
			{
				if (height < max_height)
					++height;
				else
					flag = false;
			}
			else
			{
				if (height > 1)
					--height;
				else
					flag = true;
			}
		}
	}

	bool is_alive()
	{
		return pos_x >= 0 && pos_x < MAP_LENGTH;
	}
};

struct dino 
{
	// from 0(underground) to SKY_HEIGHT
	// when final_game_map_display, print it from the end to the begin
	vector<char> map[MAP_HEIGHT];
	vector<char> barrier_map[MAP_HEIGHT];
	vector<Barrier*> barriers;

	int score = 0;
	int highest_score = 0;

	int barrier_chance = DEFAULT_BARRIER_CHANCE;
	int barrier_cold_time = 0;

	// dinosaur's data
	int falling_delay = DEFAULT_FALLING_DELAY;
	int falling_speed = DEFAULT_FALLING_SPEED;

	int pos_x = DEFAULT_POS_X;
	int pos_y = DEFAULT_POS_Y;

	bool is_jumping = false;
	int jump_power_left = 0;
};
typedef dino Dino;
static Dino* m_dino = new Dino();

struct ui
{
	// start_menu
	const vector<string> start_menu_button = { "  START  ", "  SKILL  ", "  EXIT GAME  " };
	const vector<string> start_menu_selected_button = { "> START <", "> SKILL <", "> EXIT GAME <" };
	// game_over_menu
	const vector<string> game_over_menu_button = { "  BACK TO MENU  " };
	const vector<string> game_over_menu_selected_button = { "> BACK TO MENU <" };


	int cursor_pos_index = 0;
	bool flag = false;
};
typedef ui UI;
static UI* m_ui = new UI();


#pragma region other functions
void sleep_per_frame();

bool execute_per_frame(int frequency);
bool execute_per_frame(int frequency, void (*op)(void));

inline string str_repeat(int count, string str);
inline void print_at(short x, short y, char str, bool reset_cursor_pos = true);
inline void print_at(short x, short y, string str, bool reset_cursor_pos = true);
inline void print_at_center(short x, short y, char str, bool reset_cursor_pos = true);
inline void print_at_center(short x, short y, string str, bool reset_cursor_pos = true);
void console_clear();
#pragma endregion

int key_pressed();


// initialization
void init(void);

// score
void score_reset();
void score_increase();
void score_record_update();
string score_print();
string highest_score_print();

// dinosaur movement
void dino_jump();
void dino_jump_animation();

// be bumped into barrier
bool is_bumped();

// update map
void map_move();
void spawn_barrier();
void barrier_animation();
void barrier_map_move();

// read & save record
void read_previous_record();
void save_record();

//UI
void cursor_pos_index_initialize();
// start_menu
void start_menu_initialize();
int start_menu_button_select();
void start_menu_animation();
// game
void final_game_map_display();
// game_over
void game_over_menu_initialize();
int game_over_menu_button_select();
void game_over_menu_animation();
