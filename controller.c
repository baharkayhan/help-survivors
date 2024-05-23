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
    
    /* Create threads for survivor_generator and drone_controller */
    pthread_t survivor_thread, drone_controller_thread;
    pthread_create(&survivor_thread, NULL, (void *)survivor_generator, NULL);
    pthread_create(&drone_controller_thread, NULL, (void *)drone_controller, NULL);

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
    /* Bekleyin ve iş parçacıklarının bitmesini bekleyin */
    pthread_join(survivor_thread, NULL);
    pthread_join(drone_controller_thread, NULL);
    
    printf("quitting...\n");
    freemap();
    /*quit everything*/
    quit_all();
    return 0;
}
