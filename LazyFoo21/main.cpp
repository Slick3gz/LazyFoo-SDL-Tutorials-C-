#include <iostream>
#include <cstdlib>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>


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
const int WALKING_ANIMATION_FRAMES = 4;
/*********************************************************************/

/**********************************************************************
Globals
**********************************************************************/
// The window we'll be rendering to
SDL_Window* gWindow = nullptr;

// The window renderer
SDL_Renderer* gRenderer = nullptr;

#ifdef _SDL_TTF_H
// Globally used font
TTF_Font* gFont = nullptr;
#endif // _SDL_TTF_H

// Rendered Texture
LTexture gPromptTexture;

// The music that will be played
Mix_Music *gMusic = nullptr;

// The sound effects that will be used
Mix_Chunk* gScratch = nullptr;
Mix_Chunk* gHigh = nullptr;
Mix_Chunk* gMedium = nullptr;
Mix_Chunk* gLow = nullptr;


/*********************************************************************/

/**************************************************************************
Function Declarations
**************************************************************************/
bool init();

bool loadMedia();

void close();
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
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
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
                #ifdef _SDL_TTF_H
				// Initialize SDL_ttf
				if( TTF_Init() == -1 )
                {
                    std::cout << "SDL_ttf could not initialize! SDL_ttf Error: " << SDL_GetError() ;
                    success = false;
                }
                #endif // _SDL_TTF_H

                if( Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
                {
                    std::cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() ;
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

    #ifdef _SDL_TTF_H
    // Open the font
    gFont = TTF_OpenFont("16_true_type_fonts/lazy.ttf", 28 );
    if( gFont == nullptr )
    {
        std::cout << "Failed to load lazy font! SDL_ttf Error: " << SDL_GetError() ;
        success = false;
    }
    else
    {
        // Render text
        SDL_Color textColor = {0,0,0};

    }
    #endif // _SDL_TTF_H

    if(!gPromptTexture.loadFromFile("21_sound_effects_and_music/prompt.png"))
    {
        success = false;
    }

    // Load music
    gMusic = Mix_LoadMUS("21_sound_effects_and_music/beat.wav");
    if ( gMusic == nullptr )
    {
        std::cout << "Failed to load beat music! SDL_mixer Error: " << Mix_GetError();
        success = false;
    }

    // Load sound effects
    gScratch = Mix_LoadWAV("21_sound_effects_and_music/scratch.wav");
    if ( gScratch == nullptr )
    {
        std::cout << "Failed to load scratch sound effect! SDL_mixer Error: " << Mix_GetError();
        success = false;
    }

    gHigh = Mix_LoadWAV("21_sound_effects_and_music/high.wav");
    if ( gHigh == nullptr )
    {
        std::cout << "Failed to load high sound effect! SDL_mixer Error: " << Mix_GetError();
        success = false;
    }

    gMedium = Mix_LoadWAV("21_sound_effects_and_music/medium.wav");
    if ( gMedium == nullptr )
    {
        std::cout << "Failed to load meduium sound effect! SDL_mixer Error: " << Mix_GetError();
        success = false;
    }

    gLow = Mix_LoadWAV("21_sound_effects_and_music/low.wav");
    if ( gLow == nullptr )
    {
        std::cout << "Failed to load low sound effect! SDL_mixer Error: " << Mix_GetError();
        success = false;
    }
    return success;
}

void close()
{
    // Free loaded images
    gPromptTexture.free();

    // Free music
    Mix_FreeMusic(gMusic);

    // Free Sound Effects
    Mix_FreeChunk(gScratch);
    Mix_FreeChunk(gHigh);
    Mix_FreeChunk(gMedium);
    Mix_FreeChunk(gLow);

    #ifdef _SDL_TTF_H
    // Free global font
    TTF_CloseFont(gFont);
    gFont = NULL;
    TTF_Quit();
    #endif // _SDL_TTF_H
    // Destroy window
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow == nullptr;
    gRenderer == nullptr;

    // Quit SDL subsystem
    Mix_Quit();
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

			// Angle of rotation
			double degrees = 0;

			SDL_RendererFlip flipType = SDL_FLIP_NONE;

			//Event handler
			SDL_Event e;



			//While application is running
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
					else if( e.type == SDL_KEYDOWN)
                    {
                        switch( e.key.keysym.sym )
                        {
                        case SDLK_1:
                            Mix_PlayChannel(-1, gHigh, 0);
                            break;
                        case SDLK_2:
                            Mix_PlayChannel(-1, gMedium, 0);
                            break;
                        case SDLK_3:
                            Mix_PlayChannel(-1, gLow, 0);
                            break;
                        case SDLK_4:
                            Mix_PlayChannel(-1, gScratch, 0);
                            break;
                        case SDLK_9:
                            if( Mix_PlayingMusic() == 0)
                            {
                                // Play the music
                                Mix_PlayMusic(gMusic, -1);
                            }
                            // If music is being played
                            else
                            {
                                // If the music is paused
                                if( Mix_PausedMusic() == 1)
                                {
                                    // Rssume
                                    Mix_ResumeMusic();
                                }
                                // if the music is playing
                                else
                                {
                                    Mix_PauseMusic();
                                }
                            }
                            break;
                        case SDLK_0:
                            // Stop the music
                            Mix_HaltMusic();
                            break;
                        }
                    }
				}



				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xff, 0xff, 0xff, 0xff );
				SDL_RenderClear( gRenderer );



                gPromptTexture.render(0,0);




				//Update screen
				SDL_RenderPresent( gRenderer );


			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;

}



