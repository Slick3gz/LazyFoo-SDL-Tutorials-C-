#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <SDL.h>
#include <SDL_image.h>

/*************************************************************************
 Window wrapper class
*************************************************************************/
class LWindow
{
public:
    // constructor
    LWindow();

    // creates window
    bool init();

    // Handles window events
    void handleEvent(SDL_Event& e);

    // Focuses on window
    void focus();

    // Shows windows contents
    void render();

    // Deallocate internals
    void free();

    // Window dimensions
    int getWidth();
    int getHeight();

    // window focii
    bool hasMouseFocus();
    bool hasKeyboardFocus();
    bool isMinimized();
    bool isShown();

private:
    // Window data
    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;
    int mWindowID;

    // Window dimensions
    int mWidth;
    int mHeight;

    // Window focus
    bool mMouseFocus;
    bool mKeyboardFocus;
    bool mFullScreen;
    bool mMinimized;
    bool mShown;
};
/*********************************************************************/
/*************************************************************************
 Texture wrapper class
*************************************************************************/
class LTexture
{
public:
    // Constructor
    LTexture();

    // Desstructor
    ~LTexture();

    // Loads image at specified path
    bool loadFromFile(std::string path);

    // Creates image from string
    bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
    // Deallocate texture
    void free();

    // Renders texture at a given point
    void render(int x, int y,
                SDL_Rect* clip = nullptr,
                double angle = 0.0,
                SDL_Point* center = nullptr,
                SDL_RendererFlip flip = SDL_FLIP_NONE);

    // Set color Modulation
    void setColor(Uint8 red, Uint8 green, Uint8 blue);

    // Set alpha blending
    void setBlendMode(SDL_BlendMode blending);

    // Set Alpha
    void setAlpha(Uint8 alpha);

    // Gets image dimensions
    int getWidth();
    int getHeight();

private:
    // The actual hardware texture
    SDL_Texture* mTexture;

    // Image dimensions
    int mWidth;
    int mHeight;
};

/*********************************************************************/

/**********************************************************************
Constants
**********************************************************************/
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int TOTAL_WINDOWS = 3;
/*********************************************************************/

/**********************************************************************
Globals
**********************************************************************/
// The window we'll be rendering to
LWindow gWindows[TOTAL_WINDOWS];

// The window renderer
SDL_Renderer* gRenderer = nullptr;

// Globally used font
#ifdef _SDL_TTF_H
TTF_Font* gFont = nullptr;
#endif // _SDL_TTF_H

// Scene textures
LTexture gSceneTexture;

/*********************************************************************/

/**************************************************************************
Function Declarations
**************************************************************************/
bool init();

bool loadMedia();

void close();
/************************************************************************/

/**************************************************************************
LWindow Method Declarations
***************************************************************************/

LWindow::LWindow(): mWindow(nullptr), mMouseFocus(false), mKeyboardFocus(false),
                    mFullScreen(false), mMinimized(false), mWidth(0), mHeight(0)
{

}

bool LWindow::init()
{
    // Create window
    mWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
                               SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if(mWindow != nullptr)
    {
        mMouseFocus = true;
        mKeyboardFocus = true;
        mWidth = SCREEN_WIDTH;
        mHeight = SCREEN_HEIGHT;

        // Create renderer for window
        mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if(mRenderer == NULL)
        {
            std::cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(mWindow);
            mWindow = NULL;
        }
        else
        {
            // Initialize renderer color
            SDL_SetRenderDrawColor(mRenderer, 0xff, 0xff, 0xff, 0xff);

            // Grab window id
            mWindowID = SDL_GetWindowID(mWindow);

            // Flag as opened
            mShown = true;
        }

    }
    else
    {
        std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
    }

    return mWindow != nullptr && mRenderer != NULL;
}

void LWindow::free()
{

}


void LWindow::handleEvent(SDL_Event& e)
{
    // Window event occurred
    if(e.type == SDL_WINDOWEVENT && e.window.windowID == mWindowID)
    {
        // Capiton update flag
        bool updateCaption = false;

        switch(e.window.event)
        {
            // Window appeared
        case SDL_WINDOWEVENT_SHOWN:
            mShown = true;
            break;

            // Window discappeared
        case SDL_WINDOWEVENT_HIDDEN:
            mShown = false;
            break;

            // Get new dimensions  and repaint on window size change
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            mWidth = e.window.data1;
            mHeight = e.window.data2;
            SDL_RenderPresent(gRenderer);
            break;

            // Repaint on exposure
        case SDL_WINDOWEVENT_EXPOSED:
            SDL_RenderPresent(gRenderer);
            break;

            // Mouse entered window
        case SDL_WINDOWEVENT_ENTER:
            mMouseFocus = true;
            updateCaption = true;
            break;

            // mouse left window
        case SDL_WINDOWEVENT_LEAVE:
            mMouseFocus = false;
            updateCaption = true;
            break;

            // window has keyboard focus
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            mKeyboardFocus = true;
            updateCaption = true;
            break;

            // Window lost keyoard focus
        case SDL_WINDOWEVENT_FOCUS_LOST:
            mKeyboardFocus = false;
            updateCaption = true;
            break;

            // window minimized
        case SDL_WINDOWEVENT_MINIMIZED:
            mMinimized = true;
            break;

            // window maximized
        case SDL_WINDOWEVENT_MAXIMIZED:
            mMinimized = false;
            break;

            // window restored
        case SDL_WINDOWEVENT_RESTORED:
            mMinimized = false;
            break;

            // Hide on close
        case SDL_WINDOWEVENT_CLOSE:
            SDL_HideWindow(mWindow);
            break;
        }

        // Update window caption with new data
        if(updateCaption)
        {
            std::stringstream caption;
            caption << "SDL Tutorial - MouseFocus: " << ( (mMouseFocus) ? "On" : "Off" ) <<
                    " KeyboardFocus: " << ((mKeyboardFocus) ? "On" : "Off");
            SDL_SetWindowTitle(mWindow, caption.str().c_str());
        }
    }
    else if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN)
    {
        if(mFullScreen)
        {
            SDL_SetWindowFullscreen(mWindow, SDL_FALSE);
            mFullScreen = false;
        }
        else
        {
            SDL_SetWindowFullscreen(mWindow, SDL_TRUE);
            mFullScreen = true;
            mMinimized = false;
        }

    }
}

void LWindow::focus()
{
    // Restore window if needed
    if(!mShown)
    {
        SDL_ShowWindow(mWindow);
    }

    // Move window forward
    SDL_RaiseWindow(mWindow);
}

void LWindow::render()
{
    if(!mMinimized)
    {
        // Clear screen
        SDL_SetRenderDrawColor(mRenderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderClear(mRenderer);

        // Update screen
        SDL_RenderPresent(mRenderer);
    }
}

int LWindow::getWidth() { return mWidth; }
int LWindow::getHeight() { return mHeight ;}
bool LWindow::hasMouseFocus() { return mMouseFocus; }
bool LWindow::hasKeyboardFocus() { return mKeyboardFocus; }
bool LWindow::isMinimized() { return mMinimized; }
bool LWindow::isShown() { return mShown; }


/**************************************************************************/
/*********************************************************************
LTexture Method Declarations
*********************************************************************/

// Constructor
LTexture::LTexture()
{
    // Initialize
    mTexture = nullptr;
    mWidth = 0;
    mHeight = 0;
}

LTexture::~LTexture()
{
    // Deallocate
    free();
}

bool LTexture::loadFromFile(std::string path)
{
    //The final texture
    SDL_Texture* newTexture = nullptr;

    // Load image at the specified path
    SDL_Surface* loadedSurface = IMG_Load(path.c_str() );
    if( loadedSurface == nullptr)
    {
        printf("Unable to load image %s! SDL_image Error: %s\n",
               path.c_str(), SDL_GetError());
    }
    else
    {
        // Color key image
        SDL_SetColorKey(loadedSurface, SDL_TRUE,
                        SDL_MapRGB(loadedSurface->format, 0, 0xff, 0xff) );

        // Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if( newTexture == nullptr)
        {
            printf("Unable to create texture from %s! SDL_Error: %s",
                   path.c_str(), SDL_GetError());
        }
        else
        {
            // Get image dimensions
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }

        SDL_FreeSurface( loadedSurface );
    }
    mTexture = newTexture;
    return mTexture != nullptr;
}

void LTexture::free()
{
    // Free texture if it exists
    if ( mTexture != nullptr)
    {
        SDL_DestroyTexture( mTexture );
        mTexture = nullptr;
        mWidth = 0;
        mHeight = 0;
    }
}

#ifdef _SDL_TTF_H
// Create image from text
bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor)
{
    // Get rid of preexisting texture
    free();

    // Render text surface
    SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor );
    if(textSurface == nullptr)
    {
        std::cout << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
    }
    else
    {
        // Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
        if( mTexture == nullptr)
        {
            std::cout << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
        }
        else
        {
            // Get image dimensions
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }
        // Get rid of old surface
        SDL_FreeSurface( textSurface );
    }
    return mTexture != nullptr;
}
#endif // _SDL_TTF_H

// Set color Modulation
void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
    SDL_SetTextureColorMod(mTexture, red, green, blue);
}

// Set alpha blending
void LTexture::setBlendMode(SDL_BlendMode blending)
{
    SDL_SetTextureBlendMode(mTexture, blending);
}

// Set Alpha
void LTexture::setAlpha(Uint8 alpha)
{
    SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y,
                SDL_Rect* clip,
                double angle,
                SDL_Point* center,
                SDL_RendererFlip flip)
{
    // Set rendering space and render to screen
    SDL_Rect renderQuad = {x,y, mWidth, mHeight};

    // Set clip rendering dimensions
    if( clip != nullptr)
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    // Render to screen
    SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth()
{
    return mWidth;
}

int LTexture::getHeight()
{
    return mHeight;
}
/**************************************************************************/

/***************************************************************************
Function Definitions
***************************************************************************/

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		if( !gWindows[0].init() )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}

	}

	return success;
}

bool loadMedia()
{
    // Loading success flag
    bool success = true;
#ifdef _SDL_TTF_H
    // Open the font
    gFont = TTF_OpenFont("23_advanced_timers/lazy.ttf", 28 );
    if( gFont == nullptr )
    {
        std::cout << "Failed to load lazy font! SDL_ttf Error: " << SDL_GetError() ;
        success = false;
    }
    else
    {
        // Render text
        SDL_Color textColor = {0,0,0};
        //Load stop prompt texture
		if( !gStartPromptTexture.loadFromRenderedText( "Press S to Start or Stop the Timer", textColor ) )
		{
			printf( "Unable to render start/stop prompt texture!\n" );
			success = false;
		}

		//Load pause prompt texture
		if( !gPausePromptTexture.loadFromRenderedText( "Press P to Pause or Unpause the Timer", textColor ) )
		{
			printf( "Unable to render pause/unpause prompt texture!\n" );
			success = false;
		}
    }
#endif // _SDL_TTF_H

    return success;
}

void close()
{
    // Free loaded images
    gSceneTexture.free();

    // Free global font
#ifdef _SDL_TTF_H
    TTF_CloseFont(gFont);
    gFont = NULL;
    TTF_Quit();
#endif // _SDL_TTF_H

    // Destroy windows
    for(auto i = 0; i < TOTAL_WINDOWS; ++i)
    {
        gWindows[i].free();
    }
    // Quit SDL subsystem

    IMG_Quit();
    SDL_Quit();
}

/**************************************************************************
Main
**************************************************************************/
int main( int argc, char* args[] )
{

	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{
            // Init the rest of the windows
            for(auto i = 1; i < TOTAL_WINDOWS; ++i)
            {
                gWindows[i].init();
            }
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;


			while( !quit )
			{
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}

					// Handle window events
					for(auto i = 0; i < TOTAL_WINDOWS; ++i)
                    {
                        gWindows[i].handleEvent(e);
                    }

                    // Pull up window
                    if(e.type == SDL_KEYDOWN)
                    {
                        switch(e.key.keysym.sym)
                        {
                        case SDLK_1:
                            gWindows[0].focus();
                            break;
                        case SDLK_2:
                            gWindows[1].focus();
                            break;
                        case SDLK_3:
                            gWindows[2].focus();
                            break;
                        }
                    }
				}

                // Update all windows
                for(auto i = 0; i < TOTAL_WINDOWS; ++i)
                {
                    gWindows[i].render();
                }

                bool allWindowsClosed = true;
                for(auto i = 0; i < TOTAL_WINDOWS; ++i)
                {
                    if(gWindows[i].isShown())
                    {
                        allWindowsClosed = false;
                        break;
                    }
                }

                // Application closed all windows
                if(allWindowsClosed)
                {
                    quit = true;
                }

                }
			}
		}



	return 0;

}



