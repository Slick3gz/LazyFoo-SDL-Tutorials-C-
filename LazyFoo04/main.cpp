#include <cstdio>
#include <string>
#include "SDL.h"

        // Key press surfaces constants
/*******************************************/
enum KeyPressSurfaces
{
    KEY_PRESS_SURFACE_DEFAULT,
    KEY_PRESS_SURFACE_UP,
    KEY_PRESS_SURFACE_DOWN,
    KEY_PRESS_SURFACE_LEFT,
    KEY_PRESS_SURFACE_RIGHT,
    KEY_PRESS_SURFACE_TOTAL
};

/*******************************************/

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

// The images that correspond to a keypress
SDL_Surface* gKeyPressSurfaces[ KEY_PRESS_SURFACE_TOTAL ];

// Current displayed image
SDL_Surface* gCurrentSurface = nullptr;

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

    // Load default surface
    gKeyPressSurfaces[ KEY_PRESS_SURFACE_DEFAULT ] = loadSurface("04_key_presses/press.bmp");
    if( gKeyPressSurfaces[ KEY_PRESS_SURFACE_DEFAULT ] == nullptr)
    {
        printf("Failed to load default image!\n");
        success = false;
    }

    // Load up surface
    gKeyPressSurfaces[ KEY_PRESS_SURFACE_UP ] = loadSurface("04_key_presses/up.bmp");
    if( gKeyPressSurfaces[ KEY_PRESS_SURFACE_UP ] == nullptr)
    {
        printf("Failed to load up image!\n");
        success = false;
    }

    // Load down surface
    gKeyPressSurfaces[ KEY_PRESS_SURFACE_DOWN ] = loadSurface("04_key_presses/down.bmp");
    if( gKeyPressSurfaces[ KEY_PRESS_SURFACE_DOWN ] == nullptr)
    {
        printf("Failed to load down image!\n");
        success = false;
    }

    // Load left surface
    gKeyPressSurfaces[ KEY_PRESS_SURFACE_LEFT ] = loadSurface("04_key_presses/left.bmp");
    if( gKeyPressSurfaces[ KEY_PRESS_SURFACE_LEFT ] == nullptr)
    {
        printf("Failed to load left image!\n");
        success = false;
    }

    // Load right surface
    gKeyPressSurfaces[ KEY_PRESS_SURFACE_RIGHT ] = loadSurface("04_key_presses/right.bmp");
    if( gKeyPressSurfaces[ KEY_PRESS_SURFACE_RIGHT ] == nullptr)
    {
        printf("Failed to load right image!\n");
        success = false;
    }
    return success;
}

SDL_Surface* loadSurface( std::string path)
{
    // Load image at specified path
    SDL_Surface* loadedSurface = SDL_LoadBMP(path.c_str());
    if(loadedSurface == nullptr)
    {
        printf("Unable to load image %s! SDL Error: %s", path.c_str(), SDL_GetError());
    }
    return loadedSurface;
}
void close()
{
	//Deallocate surfaces
	for( int i = 0; i < KEY_PRESS_SURFACE_TOTAL; ++i )
	{
		SDL_FreeSurface( gKeyPressSurfaces[ i ] );
		gKeyPressSurfaces[ i ] = NULL;
	}

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

            // Set default current surface
            gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_DEFAULT ];

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

                    // User presses a key
                    else if(event.type == SDL_KEYDOWN)
                    {
                        // Select surfaces base on key press
                        switch(event.key.keysym.sym)
                        {
                        case SDLK_UP:
                            gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_UP ];
                            break;
                        case SDLK_DOWN:
                            gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_DOWN ];
                            break;
                        case SDLK_LEFT:
                            gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_LEFT ];
                            break;
                        case SDLK_RIGHT:
                            gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_RIGHT ];
                            break;
                        default:
                            gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_DEFAULT ];
                            break;
                        }
                    }
                }


                // Apply the current image
                SDL_BlitSurface( gCurrentSurface,
                                nullptr,
                                gScreenSurface,
                                nullptr);

                // Update the surface
                SDL_UpdateWindowSurface(gWindow);
            }
        }
    }

    // Free resources and close SDL
    close();

    return 0;
}
/******************************************************/
