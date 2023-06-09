﻿#include "dinosaur_game.h"

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

inline void print_at(short x, short y, char str, bool reset_cursor_pos)
{
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { x, y });
	cout << str;

	if (reset_cursor_pos)
	{
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 0 });
	}
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

inline void print_at_center(short x, short y, char str, bool reset_cursor_pos)
{
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)(x + (MAP_LENGTH - 1) / 2), y });
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

void console_clear()
{
	system("cls");
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

void score_reset()
{
	m_dino->score = 0;
}

void score_increase()
{
	++m_dino->score;
}

void score_record_update()
{
	if (m_dino->score > m_dino->highest_score)
	{
		m_dino->highest_score = m_dino->score;
	}
}

string score_print()
{
	if (m_dino->score == 0)
	{
		return str_repeat(5, "0");
	}
	else
	{
		return str_repeat(5 - (int)log10(m_dino->score) - 1, "0") + to_string(m_dino->score);
	}
}

string highest_score_print()
{
	if (m_dino->highest_score == 0)
	{
		return str_repeat(5, "0");
	}
	else
	{
		return str_repeat(5 - (int)log10(m_dino->highest_score) - 1, "0") + to_string(m_dino->highest_score);
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
void cursor_pos_index_initialize()
{
	m_ui->cursor_pos_index = 0;
}

void start_menu_initialize()
{
	m_ui->flag = false;

	// start printing from the (0,0) position of console
	print_at(8, 2, str_repeat(MAP_LENGTH, "▌"));
	print_at_center(8, 5, "LITTLE DINOSAUR");
	for (int i = 0; i < m_ui->start_menu_button.size(); i++)
	{
		print_at_center(8, 7 + i, m_ui->start_menu_button[i]);
	}
	print_at(8, 7 + (int)m_ui->start_menu_button.size() + 2, str_repeat(MAP_LENGTH, "▌"));

	print_at_center(8, 7 + m_ui->cursor_pos_index, m_ui->start_menu_selected_button[m_ui->cursor_pos_index]);
}

int start_menu_button_select()
{
	switch (key_pressed())
	{
	case UP:
		if (m_ui->cursor_pos_index > 0)
		{
			print_at_center(8, 7 + m_ui->cursor_pos_index, m_ui->start_menu_button[m_ui->cursor_pos_index]);
			--m_ui->cursor_pos_index;
			print_at_center(8, 7 + m_ui->cursor_pos_index, m_ui->start_menu_selected_button[m_ui->cursor_pos_index]);
		}
		break;

	case DOWN:
		if (m_ui->cursor_pos_index < m_ui->start_menu_button.size() - 1)
		{
			print_at_center(8, 7 + m_ui->cursor_pos_index, m_ui->start_menu_button[m_ui->cursor_pos_index]);
			++m_ui->cursor_pos_index;
			print_at_center(8, 7 + m_ui->cursor_pos_index, m_ui->start_menu_selected_button[m_ui->cursor_pos_index]);
		}
		break;

	case ENTER:
		return m_ui->cursor_pos_index;

	default:
		return -1;
	}

	return -1;
}

void start_menu_animation()
{
	if (m_ui->flag)
	{
		print_at_center(8, 7 + m_ui->cursor_pos_index, m_ui->start_menu_selected_button[m_ui->cursor_pos_index]);

		m_ui->flag = false;
	}
	else
	{
		print_at_center(8, 7 + m_ui->cursor_pos_index, str_repeat(MAP_LENGTH, " "));

		m_ui->flag = true;
	}
}

void final_game_map_display()
{
	console_clear();

	print_at(8, 2, str_repeat(MAP_LENGTH / 2, "▌ "));
	print_at(8, 7 + MAP_HEIGHT + 1, str_repeat(MAP_LENGTH / 2, "▌ "));

	// top UI bar
	if (m_dino->score != 0)
		print_at(8 + MAP_LENGTH - 5, 3, score_print());
	else
		print_at(8 + MAP_LENGTH - 5, 3, string(5, '0'));

	// entire map
	for (int i = 0; i < MAP_HEIGHT; i++)
	{
		for (int j = 0; j < MAP_LENGTH; j++)
		{
			if (m_dino->map[i][j] != ' ')
			{
				print_at(8 + j, 6 + MAP_HEIGHT - i, m_dino->map[i][j]);
			}
		}
	}

	for (int i = 0; i < MAP_HEIGHT; i++)
	{
		for (int j = 0; j < MAP_LENGTH; j++)
		{
			if (m_dino->barrier_map[i][j] != '\0')
			{
				print_at(8 + j, 6 + MAP_HEIGHT - i, m_dino->barrier_map[i][j]);
			}
		}
	}

	print_at(8 + m_dino->pos_x, 6 + MAP_HEIGHT - m_dino->pos_y - ROAD_SURFACE, "R");
}

void game_over_menu_initialize()
{
	m_ui->flag = false;

	print_at_center(8, 6, "▌▋▋-GAME OVER-");

	print_at_center(8, 7, "SCORE : " + score_print());
	print_at_center(8, 8, "HIGHEST SCORE : " + highest_score_print());
}

int game_over_menu_button_select()
{
	switch (key_pressed())
	{
	case UP:
		if (m_ui->cursor_pos_index > 0)
		{
			print_at_center(8, 9 + m_ui->cursor_pos_index, m_ui->game_over_menu_button[m_ui->cursor_pos_index]);
			--m_ui->cursor_pos_index;
			print_at_center(8, 9 + m_ui->cursor_pos_index, m_ui->game_over_menu_selected_button[m_ui->cursor_pos_index]);
		}
		break;

	case DOWN:
		if (m_ui->cursor_pos_index < m_ui->game_over_menu_button.size() - 1)
		{
			print_at_center(8, 9 + m_ui->cursor_pos_index, m_ui->game_over_menu_button[m_ui->cursor_pos_index]);
			++m_ui->cursor_pos_index;
			print_at_center(8, 9 + m_ui->cursor_pos_index, m_ui->game_over_menu_selected_button[m_ui->cursor_pos_index]);
		}
		break;

	case ENTER:
		return m_ui->cursor_pos_index;

	default:
		return -1;
	}

	return -1;
}

void game_over_menu_animation()
{
	if (m_ui->flag)
	{
		print_at_center(8, 9 + m_ui->cursor_pos_index, m_ui->game_over_menu_selected_button[m_ui->cursor_pos_index]);

		m_ui->flag = false;
	}
	else
	{
		print_at_center(8, 9 + m_ui->cursor_pos_index, str_repeat(MAP_LENGTH, " "));

		m_ui->flag = true;
	}
}
#pragma endregion
