#include <cstdio>
#include "SDL.h"


        // Screen dimension constants
/*******************************************/
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
/*******************************************/

        // Function Declarations
/******************************************/
// Starts up SDL and creates window
bool init();

// Loads media
bool loadMedia();

// Frees media and shuts down SDL
void close();
/*****************************************/

        // Global Variables
/****************************************/
// The window we'll be rendering to
SDL_Window* gWindow = nullptr;

// The surface contained by the window
SDL_Surface* gScreenSurface = nullptr;

// The image we will load and show on the screen
SDL_Surface* gHelloWorld = nullptr;

/***************************************/

        // Function Definitions
/***************************************/
bool init()
{
    // Initialization flag
    bool success = true;

    // Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        success = false;
    }
    else
    {
        // Create Window
        gWindow = SDL_CreateWindow("SDL Tutorial",
                                   SDL_WINDOWPOS_UNDEFINED,
                                   SDL_WINDOWPOS_UNDEFINED,
                                   SCREEN_WIDTH,
                                   SCREEN_HEIGHT,
                                   SDL_WINDOW_SHOWN);
        if(gWindow == nullptr)
        {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            // Get window Surface
            gScreenSurface = SDL_GetWindowSurface(gWindow);
        }
    }
    return success;
}

bool loadMedia()
{
    // Loading success flag
    bool success = true;

    // Load splash image
    gHelloWorld = SDL_LoadBMP("hello_world.bmp");
    if(gHelloWorld == nullptr)
    {
        printf("Unable to lead image %s! SDLError: %s\n",
               "hello_world.bmp",
               SDL_GetError());
        success = false;
    }
    return success;
}

void close()
{
    // Deallocate surface
    SDL_FreeSurface(gHelloWorld);
    gHelloWorld = nullptr;

    // Destroy window
    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;

    // Quit SDL subsystems
    SDL_Quit();
}
/**************************************************************/


            // Main Function
/**************************************************************/
int main(int argc, char* args[])
{

    // Start up SDL and create window
    if(!init())
    {
        printf("Failed to initialize!\n");
    }
    else
    {
        // Load Media
        if(!loadMedia())
        {
            printf("Failed to load media!\n");
        }
        else
        {
            SDL_BlitSurface(gHelloWorld,
                            nullptr,
                            gScreenSurface,
                            nullptr);

            // Updated the surface
            SDL_UpdateWindowSurface(gWindow);

            // Wait two seconds
            SDL_Delay(2000);
        }
    }
    // main loop flag
    bool quit = false;

    // Event handler
    SDL_Event event;

    // While application is running
    while(!quit)
    {
        while(SDL_PollEvent(&event) != 0)
        {
            // User requests quit
            if(event.type == SDL_QUIT)
            {
                quit = true;
            }
        }
    }
    // Free resources and close SDL
    close();

    return 0;
}
/******************************************************/
