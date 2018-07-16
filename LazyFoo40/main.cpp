//
//  main.cpp
//  LazyFoo26
//
//  Created by Slick on 7/11/18.
//  Copyright � 2018 Slick Coding. All rights reserved.
//

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <SDL.h>
#include <SDL_image.h>



/**********************************************************************
 Constants
 **********************************************************************/
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int TOTAL_PARTICLES = 20;

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

    // Pixel manipulators
    bool lockTexture();
    bool unlockTexture();
    void* getPixels();
    int getPitch();

private:
    // The actual hardware texture
    SDL_Texture* mTexture;
    void* mPixels;
    int mPitch;
    // Image dimensions
    int mWidth;
    int mHeight;
};

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
#endif

LTexture gFooTexture;

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
    // Get rid of pre-existing texture
    free();

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
        // convert surface to display format
        SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(loadedSurface, SDL_GetWindowPixelFormat(gWindow), NULL);
        if(formattedSurface == nullptr)
        {
            std::cout << "Unable to convert loaded surface to display format!  SDL Error: " << SDL_GetError() << std::endl;
        }
        else
        {
            // Create blank streamable texture
            newTexture = SDL_CreateTexture(gRenderer, SDL_GetWindowPixelFormat(gWindow), SDL_TEXTUREACCESS_STREAMING,
                                           formattedSurface->w, formattedSurface->h);
            if(newTexture == nullptr)
            {
                std::cout << "Unable to create a blank texture! SDL Error: " << SDL_GetError() << std::endl;
            }
            else
            {
                // Lock texture for manipulation
                SDL_LockTexture(newTexture, NULL, &mPixels, &mPitch);

                // Copy loaded/formatted surface pixels
                memcpy(mPixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h);

                // Unlock texture to update
                SDL_UnlockTexture(newTexture);
                mPixels = NULL;

                // Get image dimensions
                mWidth = formattedSurface->w;
                mHeight = formattedSurface->h;
            }

            // Get rid of old formatted surface
            SDL_FreeSurface(formattedSurface);
        }
        // Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }
    // Return sucess
    mTexture = newTexture;
    return mTexture != NULL;
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
#endif
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

bool LTexture::lockTexture()
{
    bool success = true;

    // Texture is already locked
    if(mPixels != nullptr)
    {
        std::cout << "Texture is already locked!" << std::endl;
        success = false;
    }
    // Lock Texture
    else
    {
        if(SDL_LockTexture(mTexture, NULL, &mPixels, &mPitch) != 0)
        {
            std::cout << "Unable to lock texture! " << SDL_GetError() << std::endl;
            success = false;
        }
    }
    return success;
}

bool LTexture::unlockTexture()
{
    bool success = true;

    // Texture is not locked
    if(mPixels == nullptr)
    {
        std::cout << "Texture is not locked!" << std::endl;
        success = false;
    }
    // Unloack texture
    else
    {
        SDL_UnlockTexture(mTexture);
        mPixels = nullptr;
        mPitch = 0;
    }
    return success;
}

void* LTexture::getPixels()
{
    return mPixels;
}

int LTexture::getPitch()
{
    return mPitch;
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
            gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
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
#endif
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
    gFont = TTF_OpenFont("25_capping_frame_rate/lazy.ttf", 28 );
    if( gFont == nullptr )
    {
        std::cout << "Failed to load lazy font! SDL_ttf Error: " << SDL_GetError() ;
        success = false;
    }
#endif

    if(!gFooTexture.loadFromFile("40_texture_manipulation/foo.png"))
    {
        std::cout << "Failed to load corner texture!" << std::endl;
        success = false;
    }
    else
    {
        // Lock Texture
        if(!gFooTexture.lockTexture())
        {
            std::cout << "Unable to lock texture!" << std::endl;
        }
        // Manual color key
        else
        {
            // Allocate format from window
            Uint32 format = SDL_GetWindowPixelFormat(gWindow);
            SDL_PixelFormat* mappingFormat = SDL_AllocFormat(format);

            // Get pixel data
            Uint32* pixels = (Uint32*)gFooTexture.getPixels();
            int pixelCount = (gFooTexture.getPitch() / 4) * gFooTexture.getHeight();

            // Map colors
            Uint32 colorKey = SDL_MapRGB(mappingFormat, 0, 0xff, 0xff);
            Uint32 transparent = SDL_MapRGBA(mappingFormat, 0xff, 0xff,0xff, 0x00);

            // Color key pixels
            for(auto i = 0; i < pixelCount; ++i)
            {
                if(pixels[i] == colorKey)
                {
                    pixels[i] = transparent;
                }
            }

            // Unlock texture
            gFooTexture.unlockTexture();

            // Free format
            SDL_FreeFormat(mappingFormat);
        }
    }
    return success;
}

void close()
{
    // Free loaded images
    gFooTexture.free();
#ifdef _SDL_TTF_H
    // Free global font
    TTF_CloseFont(gFont);
    TTF_Quit();
#endif

    // Destroy window
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);

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


            // The dot that will be moving around on the screen


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

                }



                //Clear screen
                SDL_SetRenderDrawColor( gRenderer, 0xff, 0xff, 0xff, 0xff );
                SDL_RenderClear( gRenderer );



                gFooTexture.render((SCREEN_WIDTH - gFooTexture.getWidth()) / 2,
                                   (SCREEN_HEIGHT - gFooTexture.getHeight()) /2 );


                //Update screen
                SDL_RenderPresent( gRenderer );


            }
        }
    }

    //Free resources and close SDL
    close();

    return 0;

}




