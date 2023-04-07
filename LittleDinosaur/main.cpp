#include "dinosaur_game.h"

inline void game_process()
{
    print_at(8, 2, str_repeat(MAP_LENGTH / 2, "▌ "));
    print_at(8, 7 + MAP_HEIGHT + 1, str_repeat(MAP_LENGTH / 2, "▌ "));

    console_display();

    while (true)
    {
        // update in every frame
        dino_jump();

        // update in every 2 frames
        execute_per_frame(2, &dino_jump_animation);

        // update in every frame
        map_move();
        spawn_barrier();

        execute_per_frame(5, &barrier_animation);

        barrier_map_move();

        if (is_bumped())
        {
            break;
        }

        console_display();

        if (execute_per_frame(4))
            ++m_dino->score;

        // frame per FRAME_TIME millisecond
        sleep_per_frame();
    }

    save_record();

    switch (game_over_menu_display())
    {
    case 0:
        break;
    }
}

int main()
{
    init();
    read_previous_record();
    
    while (true)
    {
        int selected_index = start_menu_display();

        if (selected_index == 0)
        {            
            game_process();
        }
        else if (selected_index == 1)
        {

        }
        else if (selected_index == 2)
        {
            break;
        }

        sleep_per_frame();
    }

    return 0;
}