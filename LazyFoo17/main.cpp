#include <iostream>
#include <cstdlib>
#include <string>
#include <SDL.h>
#include <SDL_image.h>

/****************************************************************************
Button Enum
****************************************************************************/
enum LButtonSprite
{
    BUTTON_SPRITE_MOUSE_OUT = 0,
    BUTTON_SPRITE_MOUSE_OVER_MOTION = 1,
    BUTTON_SPRITE_MOUSE_DOWN = 2,
    BUTTON_SPRITE_MOUSE_UP = 3,
    BUTTON_SPRITE_TOTAL = 4
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
    #ifdef _SDL_TTF_H
    // Creates image from string
    bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
    #endif // _SDL_TTF_H

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
LButton Class
**********************************************************************/
class LButton
{
public:
    // Initializes internal variables
    LButton();

    // Sets top left position
    void setPosition(int x, int y);

    // Handles mouse event
    void handleEvent(SDL_Event* e);

    // Shows button sprite
    void render();

private:
    // Top left position
    SDL_Point mPosition;

    // Currently used global sprite
    LButtonSprite mCurrentSprite;
};
/*********************************************************************/

/**********************************************************************
Constants
**********************************************************************/
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int WALKING_ANIMATION_FRAMES = 4;

/**********************************************************************
Button Constants
**********************************************************************/
const int BUTTON_WIDTH = 300;
const int BUTTON_HEIGHT = 200;
const int TOTAL_BUTTONS = 4;
/*********************************************************************/

/**********************************************************************
Globals
**********************************************************************/
// The window we'll be rendering to
SDL_Window* gWindow = nullptr;

// The window renderer
SDL_Renderer* gRenderer = nullptr;



//Mouse button sprites
SDL_Rect gSpriteClips[ BUTTON_SPRITE_TOTAL ];
LTexture gButtonSpriteSheetTexture;

//Buttons objects
LButton gButtons[ TOTAL_BUTTONS ];

/*********************************************************************/

/**************************************************************************
Function Declarations
**************************************************************************/
bool init();

bool loadMedia();

void close();
/************************************************************************/

/************************************************************************
LButton Method Declarations
************************************************************************/
LButton::LButton()
{
    mPosition.x = 0;
    mPosition.y = 0;

    mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
}

void LButton::setPosition(int x, int y)
{
    mPosition.x = x;
    mPosition.y = y;
}

void LButton::handleEvent(SDL_Event* e)
{
    // If mouse event happened
    if(e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
    {
        // Get mouse position
        int x, y;
        SDL_GetMouseState(&x, &y);

        // Check if mouse in in button
        bool inside = true;

        // Mouse is left of the button
        if(x < mPosition.x)
        {
            inside = false;
        }

        // Mouse is right of the button
        else if(x > mPosition.x + BUTTON_WIDTH)
        {
            inside = false;
        }

        // Mouse above the button
        else if(y < mPosition.y)
        {
            inside = false;
        }

        else if(y > mPosition.y + BUTTON_HEIGHT)
        {
            inside = false;
        }

        // Mouse is outside button
        if(!inside)
        {
            mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
        }
        // Mouse is inside button
        else
        {
            // Set mouse over sprite
            switch(e->type)
            {
            case SDL_MOUSEMOTION:
                mCurrentSprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
                break;
            case SDL_MOUSEBUTTONDOWN:
                mCurrentSprite = BUTTON_SPRITE_MOUSE_DOWN;
                break;
            case SDL_MOUSEBUTTONUP:
                mCurrentSprite = BUTTON_SPRITE_MOUSE_UP;
                break;
            }
        }
    }
}

void LButton::render()
{
    // Show current button sprite
    gButtonSpriteSheetTexture.render(mPosition.x, mPosition.y, &gSpriteClips[mCurrentSprite]);
}
/***********************************************************************/
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

    // Open the font

        // Render text
        if(!gButtonSpriteSheetTexture.loadFromFile("17_mouse_events/button.png"))
        {
            std::cout << "Failed to render text texture!" << std::endl;
            success = false;
        }
        else
        {
            //Set sprites
            for( int i = 0; i < BUTTON_SPRITE_TOTAL; ++i )
            {
                gSpriteClips[ i ].x = 0;
                gSpriteClips[ i ].y = i * 200;
                gSpriteClips[ i ].w = BUTTON_WIDTH;
                gSpriteClips[ i ].h = BUTTON_HEIGHT;
            }

            //Set buttons in corners
            gButtons[ 0 ].setPosition( 0, 0 );
            gButtons[ 1 ].setPosition( SCREEN_WIDTH - BUTTON_WIDTH, 0 );
            gButtons[ 2 ].setPosition( 0, SCREEN_HEIGHT - BUTTON_HEIGHT );
            gButtons[ 3 ].setPosition( SCREEN_WIDTH - BUTTON_WIDTH, SCREEN_HEIGHT - BUTTON_HEIGHT );
        }


    return success;
}

void close()
{
    // Free loaded images
    gButtonSpriteSheetTexture.free();


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
					// Handle button events
					for(int i = 0; i < TOTAL_BUTTONS; ++i)
                    {
                        gButtons[i].handleEvent(&e);
                    }
				}

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xff, 0xff, 0xff, 0xff );
				SDL_RenderClear( gRenderer );



                // Render buttons
                for(int i = 0; i < TOTAL_BUTTONS; ++i)
                {
                    gButtons[i].render();
                }




				//Update screen
				SDL_RenderPresent( gRenderer );


			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;

}



