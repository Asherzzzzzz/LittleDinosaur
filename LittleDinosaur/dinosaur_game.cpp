#include "dinosaur_game.h"

Dino* init(void)// default value
{
	Dino* MyDino = new Dino();

	if (MyDino != NULL)
	{
		srand(time(NULL));

		for (int i = 0; i < MAP_HEIGHT; i++)
		{
			MyDino->map[i].reserve(MAP_LENGTH);

			if (i >= 0 && i < ROAD_SURFACE)// underground
			{
				for (int j = 0; j < MAP_LENGTH; j++)
				{
					if (rand() % ROAD_DIRT_RATE == 0)
					{
						MyDino->map[i].push_back('`');
					}
					else
					{
						MyDino->map[i].push_back(' ');
					}
				}
			}
			else if (i == ROAD_SURFACE)// road surface
			{
				for (int j = 0; j < MAP_LENGTH; j++)
				{
					MyDino->map[i].push_back('_');
				}
			}
			else// sky
			{
				for (int j = 0; j < MAP_LENGTH; j++)
				{
					MyDino->map[i].push_back(' ');
				}
			}
		}

		MyDino->score = 0;

		MyDino->per_iteration_time = 10;

		MyDino->barrier_chance = 3;
		MyDino->barrier_cold_time = 0;

		MyDino->jump_power = 3;

		MyDino->pos_x = 6;
		MyDino->pos_y = 0;

		MyDino->is_jumping = false;
		MyDino->jump_power_left = 0;
		MyDino->falling_speed = 0;
	}


	return MyDino;
}

void console_display(Dino* MyDino)
{
	if (is_bumped(MyDino))
	{
		system("pause");
		return;
	}

	system("cls");// clear

	cout << "\n\n\t";
	cout << "Score: " << MyDino->score 
		<< " Pos_x: " << MyDino->pos_x
		<< " Pos_y: " << MyDino->pos_y
		<< " Is_jumping: " << MyDino->is_jumping;
	cout << "\n\n\n\n\t";

	for (int i = MAP_HEIGHT - 1; i >= 0; i--)
	{
		for (int j = 0; j < MAP_LENGTH; j++)
		{
			if (MyDino->pos_y == i - ROAD_SURFACE && MyDino->pos_x == j) // if y = -depth
			{
				cout << "R";
			}
			else
			{
				cout << MyDino->map[i][j];
			}
		}
		cout << "\n\t";
	}
}

void map_move(Dino* MyDino)
{
	for (int i = MAP_HEIGHT - 1; i >= 0; i--)
	{
		MyDino->map[i].erase(MyDino->map[i].begin());

		// move sky and road
		if (i > ROAD_SURFACE)
		{
			MyDino->map[i].push_back(' ');
		}
		else if (i == ROAD_SURFACE)
		{
			MyDino->map[i].push_back('_');
		}
		else
		{
			if (rand() % ROAD_DIRT_RATE == 0)
			{
				MyDino->map[i].push_back('`');
			}
			else
			{
				MyDino->map[i].push_back(' ');
			}
		}

		// spawn barrier
		spawn_barrier(MyDino);
	}
}

void dino_jump(Dino* MyDino)
{
	if (space_pressed() && !MyDino->is_jumping)
	{
		MyDino->is_jumping = true;
		MyDino->jump_power_left = MyDino->jump_power;
		MyDino->falling_speed = 0;
	}
	else if (MyDino->is_jumping)
	{
		if (MyDino->jump_power_left > 0)
		{
			MyDino->pos_y += MyDino->jump_power_left;
			MyDino->jump_power_left /= 2;
		}
		else if (MyDino->pos_y != 0)
		{
			if (MyDino->pos_y - MyDino->falling_speed >= 0)
			{
				MyDino->pos_y -= MyDino->falling_speed;
			}
			else
			{
				MyDino->pos_y = 0;
			}
			
			if (MyDino->falling_speed == 0)
			{
				MyDino->falling_speed = 1;
			}
			else
			{
				MyDino->falling_speed *= 2;
			}
		}
		else
		{
			MyDino->is_jumping = false;
		}
	}
}

bool space_pressed(void)
{
	return _kbhit() && _getch() == 32;
}

void spawn_barrier(Dino* MyDino)
{
	if (MyDino->barrier_cold_time != 0)
	{
		--MyDino->barrier_cold_time;
	}
	else if (rand() % MyDino->barrier_chance == 0)
	{
		MyDino->barrier_cold_time = BARRIER_COLD_TIME;
		MyDino->map[ROAD_SURFACE][MAP_LENGTH - 1] = 'T';
	}
}

bool is_bumped(Dino* MyDino)
{
	//if ()
	{
		//gameover = true;
		//return true;
	}
	
	return false;
}