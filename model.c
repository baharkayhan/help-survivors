/**
 * @file model.c
 * @author baharkayhan
 * @brief
 * @version 0.1
 * @date 2024-04-21
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <unistd.h>
#include "simulator.h"

#define MAX_SURIVOR_PER_CELL 3
#define SURVIVOR_SAYISI 1000
#define DRONE_SAYISI 10

void reset_cell_color(int x, int y);
void drone_runner(void *drone);

/*SOME EXAMPLE FUNCTIONS GIVEN BELOW*/
Map map;
int numberofcells = 0;
List *survivors;
List *drones;
List *helpedsurvivors;
pthread_mutex_t map_lock; /* Mutex for map access */

void init_map(int height, int width) {
    map.height = height;
    map.width = width;
    numberofcells = height * width;
    survivors = create_list(sizeof(Survivor), numberofcells * 10);
     helpedsurvivors = create_list(sizeof(Survivor), SURVIVOR_SAYISI);
    drones = create_list(sizeof(Drone), DRONE_SAYISI);

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
    pthread_mutex_init(&map_lock, NULL); /*Initialize mutex */
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
    pthread_mutex_destroy(&map_lock); /*Destroy mutex*/
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
    /* generate random location */
     while (1){
        pthread_mutex_lock(&map_lock); /* Kilit mutex */
        /* Liste kapasitesi dolmuşsa yeni bir survivor oluşturmayın */
        if (survivors->number_of_elements >= SURVIVOR_SAYISI){
            pthread_mutex_unlock(&map_lock); /* Kilidi kaldır */
            continue;                       
        }

        pthread_mutex_unlock(&map_lock); /* Kilidi kaldır */
        /*generate random location*/
        if (map.cells != NULL){
            time_t rawtime;
            struct tm *discovery_time;

            /*survivor info*/
            char info[5] = {'A' + (random() % 26),
                            'A' + (random() % 26),
                            '0' + (random() % 9),
                            '0' + (random() % 9)};

            Coord coord = {random() % map.height, random() % map.width};

            time(&rawtime);
            discovery_time = localtime(&rawtime);

            printf("creating survivor at (%d, %d)...\n", coord.x, coord.y);
            Survivor *s = create_survivor(&coord, info, discovery_time);

            pthread_mutex_lock(&map_lock); /* Lock mutex */
            /*add to general list*/
            add(survivors, s);
            add(map.cells[coord.x][coord.y].survivors, s);
            pthread_mutex_unlock(&map_lock); /*Unlock mutex*/

            printf("Survivor added, waiting for help in cell list. Cell list size: %d\n",
                   map.cells[coord.x][coord.y].survivors->number_of_elements);

            /* Drone ile kurtarma işlemi */
            pthread_t drone_thread;
            pthread_create(&drone_thread, NULL, (void *)drone_runner, (void *)s);
            pthread_detach(drone_thread);
            draw_map();
        }
    }
}


Drone *create_drone(Coord *coord, char *info, struct tm *stime) { 
    Drone *d = malloc(sizeof(Drone));
    memset(d, 0, sizeof(Drone));
    memcpy(&(d->stime), stime, sizeof(struct tm));
    strncpy(d->info, info, sizeof(d->info));
    memcpy(&(d->coord), coord, sizeof(Coord));

    printf("drone: %s", asctime(&d->stime));
    printf("%s\n\n", d->info);
    return d;
}


void help_survivor(Drone *d, Survivor *s) {
    time_t traw;
    struct tm help_time;
    time(&traw);
    localtime_r(&traw, &help_time);

    pthread_mutex_lock(&map_lock); /* Lock mutex */

    /* Remove survivor from general list */
    survivors->removedata(survivors, s);

    /* Remove survivor from cell's list */
    List *cell_list = map.cells[s->coord.x][s->coord.y].survivors;
    cell_list->removedata(cell_list, s);

    /* Mark survivor as helped */
    memcpy(&(s->helped_time), &help_time, sizeof(struct tm));
    add(helpedsurvivors, s);

    /* Update drone status */
    d->status = 0;
    d->destination.x = -1;
    d->destination.y = -1;

    pthread_mutex_unlock(&map_lock); /* Unlock mutex */

    printf("Survivor rescued at (%d, %d)\n", s->coord.x, s->coord.y);
    reset_cell_color(s->coord.x, s->coord.y);
}


/** moves(flies) drone on the map:
based on its speed it jumps cells toward its destination*/
void move_drone(Drone *drone) {
    while (drone->destination.x != -1 && drone->destination.y != -1){
        pthread_mutex_lock(&map_lock);

        /* Move drone towards the destination */
        if (drone->coord.x < drone->destination.x){
            drone->coord.x++;
        }
        else if (drone->coord.x > drone->destination.x){
            drone->coord.x--;
        }

        if (drone->coord.y < drone->destination.y){
            drone->coord.y++;
        }
        else if (drone->coord.y > drone->destination.y){
            drone->coord.y--;
        }

        pthread_mutex_unlock(&map_lock); /* Unlock mutex */
    }
}

/*THREAD FUNCTION: simulates a drone: */
void drone_runner(void *drone) {
    Drone *d = (Drone *)drone;
    while (1){

        if (d->status == 0 && d->destination.x == -1 && d->destination.y == -1){
            
            pthread_mutex_lock(&map_lock); // Lock mutex

            for (int i = 0; i < survivors->number_of_elements; i++){
                Survivor *s = (Survivor *)get_element(survivors, i);
                if (s->helped_time.tm_year == 0){ 
                    d->destination = s->coord;
                    d->status = 1 /*BUSY*/;
                    printf("Drone %s assigned to survivor at (%d, %d)\n", d->info, s->coord.x, s->coord.y);
                    break;
                }
            }

            pthread_mutex_unlock(&map_lock); /* Unlock mutex */
        }
        if (d->destination.x != -1 && d->destination.y != -1){
            move_drone(d);
            /* Check if reached the destination */
            if (d->coord.x == d->destination.x && d->coord.y == d->destination.y){
                pthread_mutex_lock(&map_lock); /* Lock mutex */

                for (int i = 0; i < map.cells[d->coord.x][d->coord.y].survivors->number_of_elements; i++){
                    Survivor *s = (Survivor *)get_element(map.cells[d->coord.x][d->coord.y].survivors, i);
                    if (s->helped_time.tm_year == 0){ 
                        help_survivor(d, s);
                        break;
                    }
                }

                pthread_mutex_unlock(&map_lock); // Unlock mutex
            }
        }
    }
}
/*THREAD FUNCTION: an AI that controls drones based on survivors*/
void drone_controller(void *args) {
    pthread_t drone_threads[DRONE_SAYISI]; /* Drone iş parçacığı dizisi */

    for (int i = 0; i < DRONE_SAYISI; i++){
        Coord coord = {-1, -1};
        time_t rawtime;
        struct tm *stime;
        time(&rawtime);
        stime = localtime(&rawtime);

        char info[5] = {'D', 'R', '0' + (i / 10), '0' + (i % 10)};

        Drone *d = create_drone(&coord, info, stime);
        if (d != NULL){
            add(drones, d);

            pthread_t drone_thread;
            pthread_create(&drone_thread, NULL, (void *)drone_runner, (void *)d);
            pthread_detach(drone_thread);      
        }
    }

    
    for (int i = 0; i < DRONE_SAYISI; i++){
        pthread_join(drone_threads[i], NULL);
    }
}
void reset_cell_color(int x, int y){
    // ANSI kodları kullanarak ekranın rengini sıfırlama
    printf("\x1b[0m"); // Ekranın rengini sıfırlar (ANSI kodu kullanarak)
    printf("(%d, %d) hücresinin rengi sifirlandi.\n", x, y);
}

/*you should add all the necessary functions
you can add more .c files e.g. survior.c, drone.c
But, for MVC model, controller is a bridge between view and model:
put data update functions only in model, not in your view or controller.
*/
