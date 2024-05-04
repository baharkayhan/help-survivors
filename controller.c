/*
 * controller.c
 *      Author: adaskin
 */

#include "SDL2/SDL.h"
#include "simulator.h"

/*do not change any of this*/
extern SDL_bool done;

int main(int argc, char* argv[]) {
    /*initializes map*/
    init_map(40, 30);

    /*initializes window*/
    printf("initialize window\n");
    init_sdl_window(map);

    printf("draw map\n");
    /*draws initial map*/
    draw_map();

    /* repeat until window is closed */
    while (!done) {

        /*check user events: e.g. click to close x*/
        check_events();

        /* update model:survivors, drones etc. */
        survivor_generator(NULL);

        /*draws new updated map*/
        draw_map();
   
        SDL_Delay(1000); /*sleep(1);*/
  
    }
    printf("quitting...\n");
    freemap();
    /*quit everything*/
    quit_all();
    return 0;
}