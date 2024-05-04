/**
 * @file model.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-04-21
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "simulator.h"
#define MAX_SURIVOR_PER_CELL 3

/*SOME EXAMPLE FUNCTIONS GIVEN BELOW*/
Map map;
int numberofcells = 0;
List *survivors;
List *drones;

void init_map(int height, int width) {
    map.height = height;
    map.width = width;
    numberofcells = height * width;
    survivors = create_list(sizeof(Survivor), numberofcells * 10);

    /*pointer array*/
    map.cells = malloc(sizeof(MapCell *) * map.height);

    for (int j = 0; j < map.height; j++) {
        /*rows for each pointer*/
        map.cells[j] = malloc(sizeof(MapCell) * map.width);
    }

    for (int i = 0; i < map.height; i++) {
        for (int j = 0; j < map.width; j++) {
            map.cells[i][j].coord.x = i;
            map.cells[i][j].coord.y = j; /**/
            map.cells[i][j].survivors = create_list(sizeof(Survivor), 10);
        }
    }
    printf("height: %d, width:%d\n", map.height, map.width);
}
void freemap() {
    for (int i = 0; i < map.height; i++) {
        for (int j = 0; j < map.width; j++) {
            List *list = map.cells[i][j].survivors;
            list->destroy(list);
        }
        free(map.cells[i]);
    }
    free(map.cells);
}
Survivor *create_survivor(Coord *coord, char *info, struct tm *discovery_time) {
    Survivor *s = malloc(sizeof(Survivor));
    memset(s, 0, sizeof(Survivor));
    memcpy(&(s->discovery_time), discovery_time, sizeof(struct tm));
    strncpy(s->info, info, sizeof(s->info));
    memcpy(&(s->coord), coord, sizeof(Coord));

    printf("survivor: %s\n", asctime(&s->discovery_time));
    printf("%s\n\n", s->info);
    return s;
}

/*THREAD FUNCTION: generates random survivor
 */
void survivor_generator(void *args) {
    // generate random location
    if (map.cells != NULL) {
        time_t traw;
        struct tm t; /*used for localtime*/

        /*survivor info*/
        char info[5] = {'A' + (random() % 26), 
                        'A' + (random() % 26),
                        '0' + (random() % 9),
                        '0' + (random() % 9)}; 

        Coord coord = {random() % map.height, random() % map.width};

        time(&traw);
        localtime_r(&traw, &t);

        printf("creating survivor...%s\n", asctime(&t));
        Survivor *s = create_survivor(&coord, info, &t);

        /*add to general list*/
        add(survivors, s);

        /*add to the list in the cell*/
        List *list = map.cells[coord.x][coord.y].survivors;
        add(list, s);

        printf("survivor added, celllist-size:%d\n", list->number_of_elements);
    }
}


Drone *create_drone(Coord *coord, char *info, struct tm *stime) { return NULL; }

/** a drone delivers aid pack to survivor,
the survivor is marked as helped and removed*/
void help_survivor(Drone *d, Survivor *s) {
    /*TODO: remove survivor from survivorlist, mapcell.survivors*/
    /*TODO: edit help_time, add to the helped_survivors*/
    /*TODO: numberofhelped++,
    drone->status is idle, destination: empty*/
}


/** moves(flies) drone on the map:
based on its speed it jumps cells toward its destination*/
void move_drone(Drone *drone) {}

/*THREAD FUNCTION: simulates a drone: */
void drone_runner(void *drone) {

}
/*THREAD FUNCTION: an AI that controls drones based on survivors*/
void drone_controller(void *args) {}

/*you should add all the necessary functions
you can add more .c files e.g. survior.c, drone.c
But, for MVC model, controller is a bridge between view and model:
put data update functions only in model, not in your view or controller.
*/