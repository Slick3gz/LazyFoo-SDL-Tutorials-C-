#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

/*************************************************************************
 Timer Class
*************************************************************************/
class LTimer
{
public:
    // Constructor
    LTimer();

    void start();
    void stop();
    void pause();
    void unpause();

    //Gets the timer's time
    Uint32 getTicks();

    // Checks the status of the timer
    bool isStarted();
    bool isPaused();

private:
    // The clock time when the timer started
    Uint32 mStartTicks;

    // The ticks stored  when the timer was paused
    Uint32 mPausedTicks;

    // The timer status
    bool mPaused;
    bool mStarted;
};
/************************************************************************/

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
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

/*********************************************************************/

/**********************************************************************
Globals
**********************************************************************/
// The window we'll be rendering to
SDL_Window* gWindow = nullptr;

// The window renderer
SDL_Renderer* gRenderer = nullptr;

// Globally used font
TTF_Font* gFont = nullptr;

// Rendered Texture
LTexture gInputTextTexture;
LTexture gPromptTextTexture;


/*********************************************************************/

/**************************************************************************
Function Declarations
**************************************************************************/
bool init();

bool loadMedia();

void close();
/************************************************************************/

/************************************************************************
LTimer Method Declarations
*************************************************************************/
LTimer::LTimer(): mStartTicks(0), mPausedTicks(0), mPaused(false), mStarted(false)
{
}

void LTimer::start()
{
    // Start the timer
    mStarted = true;

    // Unpause the timer
    mPaused = false;

    // Get the current clock time
    mStartTicks = SDL_GetTicks();
    mPausedTicks = 0;
}

void LTimer::stop()
{
    // Stop the timer
    mStarted = false;

    // Unpause the timer
    mPaused = false;

    // Clear tick variables
    mStartTicks = 0;
    mPausedTicks = 0;
}

void LTimer::pause()
{
    // If the timer is running and isn't already paused
    if( mStarted && !mPaused)
    {
        mPaused = true;

        // Calculate the paused tickes
        mPausedTicks = SDL_GetTicks() - mStartTicks;
        mStartTicks = 0;
    }
}

void LTimer::unpause()
{
    // If timer is running and paused
    if( mStarted && mPaused)
    {
        // Unpause the timer
        mPaused = false;

        // Reset the starting ticks
        mStartTicks = SDL_GetTicks() - mPausedTicks;

        // Reset the paused ticks
        mPausedTicks = 0;
    }
}

Uint32 LTimer::getTicks()
{
    // The actual timer time
    Uint32 time = 0;

    // If the timer is running
    if(mStarted)
    {
        // If the timer is paused
        if(mPaused)
        {
            time = mPausedTicks;
        }
        else
        {
            time = SDL_GetTicks() - mStartTicks;
        }
    }
    return time;
}

bool LTimer::isStarted()
{
    // timer is running and paused or unpaused
    return mStarted;
}

bool LTimer::isPaused()
{
    // Timer is running and paused
    return mPaused && mStarted;
}
/************************************************************************/

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
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}

				// Initialize SDL_ttf
				if( TTF_Init() == -1 )
                {
                    std::cout << "SDL_ttf could not initialize! SDL_ttf Error: " << SDL_GetError() ;
                    success = false;
                }
			}
		}
	}

	return success;
}

bool loadMedia()
{
    // Loading success flag
    bool success = true;

    SDL_Color textColor = {0, 0, 0, 255};
    // Open the font
    gFont = TTF_OpenFont("32_text_input_and_clipboard_handling/lazy.ttf", 28 );
    if( gFont == nullptr )
    {
        std::cout << "Failed to load lazy font! SDL_ttf Error: " << SDL_GetError() ;
        success = false;
    }
    if(!gPromptTextTexture.loadFromRenderedText("Enter some text", textColor))
    {
        std::cout << "Unable to render text!" << std::endl;
        success = false;
    }


    return success;
}

void close()
{
    // Free loaded images
    gPromptTextTexture.free();
    gInputTextTexture.free();

    // Free global font
    TTF_CloseFont(gFont);
    gFont = NULL;
    // Destroy window
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);

    // Quit SDL subsystem
    TTF_Quit();
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
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

            // Set the text color
            SDL_Color textColor = { 0, 0, 0, 255 };

            // The current input text
            std::string inputText = "Some text";
            gInputTextTexture.loadFromRenderedText(inputText.c_str(), textColor);

            SDL_StartTextInput();




			//While application is running
			while( !quit )
			{
			    bool renderText = false;

				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}
					else if( e.type == SDL_KEYDOWN)
                    {
                        // Handle backspace
                        if(e.key.keysym.sym == SDLK_BACKSPACE && inputText.length() > 0)
                        {
                            // lop off character
                            inputText.pop_back();
                            renderText = true;
                        }
                        else if(e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
                        {
                            SDL_SetClipboardText(inputText.c_str());
                        }
                        else if(e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)
                        {
                            inputText = SDL_GetClipboardText();
                            renderText = true;
                        }
                    }
                    else if(e.type == SDL_TEXTINPUT)
                    {
                        // Not copy or pasting
                        if(!((e.text.text[0] == 'c' || e.text.text[0] == 'C') && (e.text.text[0] == 'v' || e.text.text[0] == 'V') && SDL_GetModState() & KMOD_CTRL))
                        {
                            // Append character
                            inputText += e.text.text;
                            renderText = true;
                        }
                    }
				}

                // Render text if needed
                if(renderText)
                {
                    // Text is not empty
                    if(inputText != "")
                    {
                        gInputTextTexture.loadFromRenderedText(inputText.c_str(), textColor);
                    }
                    // Text is empty
                    else
                    {
                        // Render space texture
                        gInputTextTexture.loadFromRenderedText(" ", textColor);
                    }
                }

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xff, 0xff, 0xff, 0xff );
				SDL_RenderClear( gRenderer );

                // Render textures
                gPromptTextTexture.render((SCREEN_WIDTH - gPromptTextTexture.getWidth()) / 2, 0);
                gInputTextTexture.render((SCREEN_WIDTH - gInputTextTexture.getWidth()) / 2, gPromptTextTexture.getHeight());

				//Update screen
				SDL_RenderPresent( gRenderer );


			}
		}
		SDL_StopTextInput();
	}

	//Free resources and close SDL
	close();

	return 0;

}



