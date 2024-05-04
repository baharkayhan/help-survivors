/*you can try to print map, or you SDL to show/render map in window.*/
/**
 * view.c
 *      Author: adaskin
 */
#include "SDL2/SDL.h"
#include "simulator.h"

#define CSIZE 20 /*cell size in SDL-grid*/

/*do not change any of this*/
SDL_bool done = 1; /*controls the view loop*/
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Event event;

int windowheight = 0;
int windowwidth = 0;

/**
 * initializes  window and renderer
 * window size is (windowHeight x windowWidth)
 */
int init_sdl_window(Map map) {
    windowheight = map.height * CSIZE; /*each mapcell is CSIZExCSIZE*/
    windowwidth = map.width * CSIZE;
    printf("height: %d, width:%d\n", windowheight, windowwidth);
    printf("height: %d, width:%d\n", map.height, map.width);
    /*
     * Initialises the SDL video subsystem (as well as the events subsystem).
     * Returns 0 on success or a negative error code on failure using
     * SDL_GetError().
     */
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL failed to initialise: %s\n", SDL_GetError());
        return 1;
    }
    /* Creates a SDL window */
    window =
        SDL_CreateWindow("SDL Example",           /* Title of the SDL window */
                         SDL_WINDOWPOS_UNDEFINED, /* Position x of the window */
                         SDL_WINDOWPOS_UNDEFINED, /* Position y of the window */
                         windowwidth,  /* Width of the window in pixels */
                         windowheight, /* Height of the window in pixels */
                         0);           /* Additional flag(s) */
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer != NULL) {
        SDL_SetWindowTitle(window, "Medeniyet Drone simulator");
        done = 0;
    } else {
        perror("renderer");
    }
    return done;
}

/*
 */
static void draw_rectangles() {

    for (int i = 0; i < map.height; i++) {
        for (int j = 0; j < map.width; j++) {
            SDL_Rect rect = {CSIZE * j, CSIZE * i, CSIZE, CSIZE};

            if (map.cells[i][j].survivors != NULL &&
                map.cells[i][j].survivors->number_of_elements != 0) {
                /*change color to black*/
				int r = map.cells[i][j].survivors->number_of_elements;
                r = (r%15);
				SDL_SetRenderDrawColor(renderer, 100+r*10, 255-r*10, 0, SDL_ALPHA_OPAQUE);
            } else {
                /*change color to nonblack*/
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            }

            /*draw cell with the determined color*/
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}
/**
 * draws initial map with the cell size csize on the window
 */
int draw_map() {
    draw_rectangles();
    /* draw lines for grid */
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    /*draw horizontal*/
    for (int i = 0; i <= windowheight; i = i + CSIZE) {
        SDL_RenderDrawLine(renderer, 0, i, windowwidth, i);
    }
    /* draw lines for grid */
    SDL_SetRenderDrawColor(renderer, 255, 155, 155, SDL_ALPHA_OPAQUE);

    /* draw vertical */
    for (int j = 0; j <= windowwidth; j = j + CSIZE) {
        SDL_RenderDrawLine(renderer, j, 0, j, windowheight);
    }

    SDL_RenderPresent(renderer);

    return 0;
}

/**
 * checks for the quit event,
 * returns true if event occurs
 */
int check_events() {

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            done = 1;
        }
        if (event.type == SDL_KEYDOWN) {
            done = 1;
        }
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            done = 1;
        }
    }

    return done;
}

/**
 * destroys window and renderer
 */
int quit_all() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();

    return 0;
}
