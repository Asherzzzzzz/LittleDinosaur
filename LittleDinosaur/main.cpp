#include "dinosaur_game.h"

inline int game_over_menu_process()
{
    cursor_pos_index_initialize();
    console_clear();
    game_over_menu_initialize();

    int game_over_menu_button_selection = 0;

    while (true)
    {
        game_over_menu_button_selection = game_over_menu_button_select();

        if (game_over_menu_button_selection == 0)
        {
            break;
        }

        execute_per_frame(10, game_over_menu_animation);

        sleep_per_frame();
    }

    return 0;
}

inline void game_process()
{
    console_clear();
    score_reset();

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

        execute_per_frame(4, score_increase);

        // frame per FRAME_TIME millisecond
        sleep_per_frame();
    }

    score_record_update();
    save_record();
}

inline void start_menu_process()
{
    while (true)
    {
        cursor_pos_index_initialize();
        console_clear();
        start_menu_initialize();

        int start_menu_button_selection = 0;

        while (true)
        {
            start_menu_button_selection = start_menu_button_select();

            if (start_menu_button_selection == 0)
            {         
                game_process();
                game_over_menu_process();
                break;
            }
            else if (start_menu_button_selection == 1)
            {
            }
            else if (start_menu_button_selection == 2)
            {
                return;
            }

            execute_per_frame(10, start_menu_animation);

            sleep_per_frame();
        }
    }

}

int main()
{
    init();
    read_previous_record();
    
    start_menu_process();

    console_clear();

    return 0;
}