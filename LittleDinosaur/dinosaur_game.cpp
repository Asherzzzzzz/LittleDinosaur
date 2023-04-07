#include "dinosaur_game.h"

#pragma region other_functions
void sleep_per_frame()
{
	Sleep(FRAME_TIME);
	if (frame >= INT_MAX)
		frame = 0;
	else
		++frame;
}

bool execute_per_frame(int frequency)
{
	return frame % frequency == 0;
}

bool execute_per_frame(int frequency, void (*op)(void))
{
	if (frame % frequency == 0)
	{
		(*op)();
		return true;
	}

	return false;
}

inline string str_repeat(int count, string str)
{
	string r = "";

	while (count--)
	{
		r += str;
	}

	return r;
}

inline void print_at(short x, short y, string str, bool reset_cursor_pos)
{
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { x, y });
	cout << str;

	if (reset_cursor_pos)
	{
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 0 });
	}
}

inline void print_at_center(short x, short y, string str, bool reset_cursor_pos)
{
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)(x + (MAP_LENGTH - str.length()) / 2), y });
	cout << str;

	if (reset_cursor_pos)
	{
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 0 });
	}
}
#pragma endregion

int key_pressed()
{
	if (_kbhit())
	{
		int input = _getch();

		if (input != 224)
			return input;
		else
			return -_getch();
	}

	return -1;
}

void init()// set default value
{
	if (m_dino != NULL)
	{
		srand((unsigned int)time(NULL));

		for (int i = 0; i < MAP_HEIGHT; i++)
		{
			m_dino->map[i].reserve(MAP_LENGTH);

			if (i >= 0 && i < ROAD_SURFACE)// underground
			{
				for (int j = 0; j < MAP_LENGTH; j++)
				{
					if (rand() % ROAD_DIRT_RATE == 0)
					{
						m_dino->map[i].push_back('`');
					}
					else if (rand() % ROAD_DIRT_RATE == 1)
					{
						m_dino->map[i].push_back(',');
					}
					else
					{
						m_dino->map[i].push_back(' ');
					}
				}
			}
			else if (i == ROAD_SURFACE)// road surface
			{
				for (int j = 0; j < MAP_LENGTH; j++)
				{
					m_dino->map[i].push_back('_');
				}
			}
			else// sky
			{
				for (int j = 0; j < MAP_LENGTH; j++)
				{
					m_dino->map[i].push_back(' ');
				}
			}
		}

		for (int i = 0; i < MAP_HEIGHT; i++)
		{
			m_dino->barrier_map[i].reserve(MAP_LENGTH);

			for (int j = 0; j < MAP_LENGTH; j++)
			{
				m_dino->barrier_map[i].push_back('\0');
			}
		}
	}
}

void dino_jump()
{
	if (!m_dino->is_jumping && key_pressed() == SPACE)
	{
		m_dino->is_jumping = true;
		m_dino->jump_power_left = DEFAULT_JUMP_POWER;
		m_dino->falling_delay = DEFAULT_FALLING_DELAY;
		m_dino->falling_speed = DEFAULT_FALLING_SPEED;
	}
}

void dino_jump_animation()
{
	if (m_dino->is_jumping)
	{
		if (m_dino->jump_power_left > 0)// jump upward
		{
			m_dino->pos_y += m_dino->jump_power_left;
			m_dino->jump_power_left -= JUMP_POWER_DECREMENT;
		}
		else if (m_dino->falling_delay > 0)// stay in the air
		{
			--m_dino->falling_delay;
		}
		else if (m_dino->pos_y != 0)// fall
		{
			if (m_dino->pos_y - m_dino->falling_speed >= 0)
			{
				m_dino->pos_y -= m_dino->falling_speed;
			}
			else
			{
				m_dino->pos_y = 0;
			}

			m_dino->falling_speed += FALLING_SPEED_INCREMENT;
		}
		else
		{
			m_dino->is_jumping = false;
		}
	}
}

bool is_bumped()
{
	return m_dino->barrier_map[m_dino->pos_y + ROAD_SURFACE][m_dino->pos_x] == 'T';
}

void map_move()
{
	for (int i = MAP_HEIGHT - 1; i >= 0; i--)
	{
		m_dino->map[i].erase(m_dino->map[i].begin());

		// move sky and road
		if (i > ROAD_SURFACE)
		{
			m_dino->map[i].push_back(' ');
		}
		else if (i == ROAD_SURFACE)
		{
			m_dino->map[i].push_back('_');
		}
		else
		{
			if (rand() % ROAD_DIRT_RATE == 0)
			{
				m_dino->map[i].push_back('`');
			}
			else if (rand() % ROAD_DIRT_RATE == 1)
			{
				m_dino->map[i].push_back(',');
			}
			else
			{
				m_dino->map[i].push_back(' ');
			}
		}
	}
}

void spawn_barrier()
{
	if (m_dino->barrier_cold_time != 0)
	{
		--m_dino->barrier_cold_time;
	}
	else if (rand() % m_dino->barrier_chance == 0)
	{
		Barrier* new_barrier = new Barrier(ROAD_SURFACE, rand() % DEFAULT_BARRIER_RANDOM_MAX_HEIGHT + 1, true);
		m_dino->barriers.push_back(new_barrier);
		m_dino->barrier_cold_time = BARRIER_COLD_TIME;
	}
}

void barrier_animation()
{
	for (int i = 0; i < m_dino->barriers.size(); i++)
	{
		m_dino->barriers[i]->deform();
	}
}

void barrier_map_move()
{
	// 1. clear barrier_map
	// 2. write in all the barriers
	// 3. move them
	// 4. delete them if x < 0
	
	for (int i = 0; i < MAP_HEIGHT; i++)
	{
		for (int j = 0; j < MAP_LENGTH; j++)
		{
			if (m_dino->barrier_map[i][j] != '\0')
				m_dino->barrier_map[i][j] = '\0';
		}
	}

	for (int i = 0; i < m_dino->barriers.size(); i++)
	{
		for (int j = m_dino->barriers[i]->pos_y; j < m_dino->barriers[i]->pos_y + m_dino->barriers[i]->height; j++)
		{
			if (j >= 0 && j < MAP_HEIGHT)
			{
				m_dino->barrier_map[j][m_dino->barriers[i]->pos_x] = 'T';
			}
		}

		m_dino->barriers[i]->move();

		if (!m_dino->barriers[i]->is_alive())
		{
			delete m_dino->barriers[i];
			m_dino->barriers.erase(m_dino->barriers.begin() + i);
			--i;
		}
	}
}

void read_previous_record()
{
	/*
	fstream file;
	file.open("record.txt", ios::in);

	if (file.is_open())
	{

	}
	else
	{
		file.close();
		file.
	}
	*/
}

void save_record()
{

}

#pragma region UI
int start_menu_display()
{
	system("cls");
	// start printing from the (0,0) position of console
	print_at(8, 2, str_repeat(MAP_LENGTH, "▌"));
	print_at_center(8, 5, "LITTLE DINOSAUR");
	for (int i = 0; i < start_menu_button.size(); i++)
	{
		print_at_center(8, 7 + i, start_menu_button[i]);
	}
	print_at(8, 7 + (int)start_menu_button.size() + 2, str_repeat(MAP_LENGTH, "▌"));

	int cursor_pos_index = 0;
	print_at_center(8, 7 + cursor_pos_index, start_menu_selected_button[cursor_pos_index]);

	int input_key;
	while (true)
	{
		input_key = key_pressed();

		if (input_key == UP)
		{
			if (cursor_pos_index > 0)
			{
				print_at_center(8, 7 + cursor_pos_index, start_menu_button[cursor_pos_index]);
				--cursor_pos_index;
				print_at_center(8, 7 + cursor_pos_index, start_menu_selected_button[cursor_pos_index]);
			}
		}
		else if (input_key == DOWN)
		{
			if (cursor_pos_index < start_menu_button.size() - 1)
			{
				print_at_center(8, 7 + cursor_pos_index, start_menu_button[cursor_pos_index]);
				++cursor_pos_index;
				print_at_center(8, 7 + cursor_pos_index, start_menu_selected_button[cursor_pos_index]);
			}
		}
		else if (input_key == ENTER)
		{
			break;
		}
	}

	system("cls");

	return cursor_pos_index;
}

void console_display()
{
	// just execute once when the game start
	

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 3 });

	// top UI bar
	if (m_dino->score != 0)
		cout << "\t" << string(MAP_LENGTH - 5, ' ') << string(5 - (int)log10(m_dino->score) - 1, '0') << m_dino->score;
	else
		cout << "\t" << string(MAP_LENGTH - 5, ' ') << string(5, '0');

	cout << "\n\n\n\n\t";

	// entire map
	for (int i = MAP_HEIGHT - 1; i >= 0; i--)
	{
		for (int j = 0; j < MAP_LENGTH; j++)
		{
			if (m_dino->pos_y == i - ROAD_SURFACE && m_dino->pos_x == j) // if y = -depth
			{
				cout << "R";
			}
			else if (m_dino->barrier_map[i][j] != '\0')
			{
				cout << m_dino->barrier_map[i][j];
			}
			else
			{
				cout << m_dino->map[i][j];
			}
		}
		cout << "\n\t";
	}
}

int game_over_menu_display()
{
	system("cls");// clear

	cout << "\n\n";
	cout << "\n\n\n\n\t";
	cout << string(MAP_LENGTH / 2 - 5, ' ') << "▌▋▋-GAME OVER-";

	cout << "\n\n\t";
	if (m_dino->score != 0)
		cout << string(MAP_LENGTH / 2 - 9, ' ') << "HIGHEST SCORE : " << string(5 - (int)log10(m_dino->score) - 1, '0') << m_dino->score;
	else
		cout << string(MAP_LENGTH / 2 - 9, ' ') << "HIGHEST SCORE : " << string(5, '0');

	cout << "\n\n\n\n\t";

	return 0;
}
#pragma endregion
