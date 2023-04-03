#include "dinosaur_game.h"

int main()
{
    Dino* MyDino = init();
    
    console_display(MyDino);

    while (true)
    {
        dino_jump(MyDino);
        map_move(MyDino);
        
        ++MyDino->score;

        console_display(MyDino);
        Sleep(MyDino->per_iteration_time);

        map_move(MyDino);
        console_display(MyDino);
        Sleep(MyDino->per_iteration_time);
    }

    return 0;
}
