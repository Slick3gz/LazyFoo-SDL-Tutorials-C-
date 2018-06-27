#include <cstdio>
#include <string>
#include "SDL.h"
#include "SDL_image.h"

        // Key press surfaces constants
/*******************************************
enum KeyPressSurfaces
{
    KEY_PRESS_SURFACE_DEFAULT,
    KEY_PRESS_SURFACE_UP,
    KEY_PRESS_SURFACE_DOWN,
    KEY_PRESS_SURFACE_LEFT,
    KEY_PRESS_SURFACE_RIGHT,
    KEY_PRESS_SURFACE_TOTAL
};

*******************************************/

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

// Loads individual image
SDL_Surface* loadSurface( std::string path );

// Frees media and shuts down SDL
void close();
/*****************************************/

        // Global Variables
/****************************************/
// The window we'll be rendering to
SDL_Window* gWindow = nullptr;

// The surface contained by the window
SDL_Surface* gScreenSurface = nullptr;

// Current displayed image
SDL_Surface* gStretchedSurface = nullptr;

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
            //Initialize PNG loading
            int imgFlags = IMG_INIT_PNG;
            if(!(IMG_Init(imgFlags) & imgFlags))
            {
                printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
                success = false;
            }
            else
            {
                // Get window Surface
                gScreenSurface = SDL_GetWindowSurface(gWindow);

            }
        }
    }
    return success;
}

bool loadMedia()
{
    // Loading success flag
    bool success = true;

    gStretchedSurface = loadSurface("loaded.png");
    if(gStretchedSurface == nullptr)
    {
        printf("Unable to load stretched image!");
        success = false;
    }
    return success;
}

SDL_Surface* loadSurface( std::string path)
{
    // The final optimized image
    SDL_Surface* optimizedSurface = nullptr;

    // Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if(loadedSurface == nullptr)
    {
        printf("Unable to load image %s! SDL Error: %s", path.c_str(), SDL_GetError());
    }
    else
    {
        // Convert surface to screen format
        optimizedSurface = SDL_ConvertSurface(loadedSurface,
                                              gScreenSurface->format,
                                              0);
        if(optimizedSurface == nullptr)
        {
            printf("Unable to optimize image %s! SDL Error: %s\n",
                   path.c_str(),
                   SDL_GetError());
        }

        // Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );
    }
    return optimizedSurface;
}
void close()
{
	//Free loaded image
	SDL_FreeSurface( gStretchedSurface );
	gStretchedSurface = NULL;


	//Destroy window
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	//Quit SDL subsystems
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
            // main loop flag
            bool quit = false;

            // Event handler
            SDL_Event event;

            // While application is running
            while(!quit)
            {
                // Handle events on queue
                while(SDL_PollEvent(&event) != 0)
                {
                    // User requests quit
                    if(event.type == SDL_QUIT)
                    {
                        quit = true;
                    }


                }


               // Apply the image stretched
               SDL_Rect stretchRect;
               stretchRect.x = 0;
               stretchRect.y = 0;
               stretchRect.w = SCREEN_WIDTH;
               stretchRect.h = SCREEN_HEIGHT;
               SDL_BlitScaled(gStretchedSurface,
                              nullptr,
                              gScreenSurface,
                              &stretchRect);

                SDL_UpdateWindowSurface(gWindow);
            }
        }
    }


    // Free resources and close SDL
    close();

    return 0;
}
/******************************************************/
