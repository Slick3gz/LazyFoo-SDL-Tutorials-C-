#include <cstdlib>
#include <string>
#include <SDL.h>
#include <SDL_image.h>

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

    // Deallocate texture
    void free();

    // Renders texture at a given point
    void render(int x, int y, SDL_Rect* clip = nullptr);

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

// Scene sprites
LTexture gModulatedTexture;
LTexture gBackgroundTexture;

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

void LTexture::render( int x, int y, SDL_Rect* clip )
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
    SDL_RenderCopy(gRenderer, mTexture, clip, &renderQuad);
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
			}
		}
	}

	return success;
}

bool loadMedia()
{
    // Loading success flag
    bool success = true;

    // Load Foo texture
    if(!gModulatedTexture.loadFromFile("13_alpha_blending/fadeout.png"))
    {
        printf("Failed to load Foo texture image\n");
        success = false;
    }
    else
    {
        gModulatedTexture.setBlendMode(SDL_BLENDMODE_BLEND);
    }

    // Load background texture
    if(!gBackgroundTexture.loadFromFile("13_alpha_blending/fadein.png"))
    {
        printf("Failed to load Foo texture image\n");
        success = false;
    }
    return success;
}

void close()
{
    // Free loaded images

    // Destroy window
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow == nullptr;
    gRenderer == nullptr;

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
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			Uint8 r = 0, g = 0, b = 0;
			Uint8 a = 255;

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
					else if(e.type == SDL_KEYDOWN)
                    {
                        switch (e.key.keysym.sym)
                        {
                        case SDLK_q:
                            r += 32;
                            break;
                        case SDLK_w:
                            g += 32;
                            break;
                        case SDLK_e:
                            b += 32;
                            break;
                        case SDLK_a:
                            r -= 32;
                            break;
                        case SDLK_s:
                            g -= 32;
                            break;
                        case SDLK_d:
                            b -= 32;
                            break;
                        case SDLK_r:
                            if(a + 32 > 255)
                            {
                                a = 255;
                            }
                            else
                            {
                                a += 32;
                            }
                            break;
                        case SDLK_f:
                            if(a - 32 < 0)
                            {
                                a = 0;
                            }
                            else
                            {
                                a -= 32;
                            }
                            break;
                        }
                    }
				}

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xff, 0xff, 0xff, 0xff );
				SDL_RenderClear( gRenderer );

				// Render top left sprite
				gBackgroundTexture.render((SCREEN_WIDTH - gBackgroundTexture.getWidth())/2 , (SCREEN_HEIGHT - gBackgroundTexture.getHeight())/2, 0);
                gModulatedTexture.setColor(r,g,b);
                gModulatedTexture.setAlpha(a);
				gModulatedTexture.render((SCREEN_WIDTH - gModulatedTexture.getWidth())/2 , (SCREEN_HEIGHT - gModulatedTexture.getHeight())/2, 0);





				//Update screen
				SDL_RenderPresent( gRenderer );
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
