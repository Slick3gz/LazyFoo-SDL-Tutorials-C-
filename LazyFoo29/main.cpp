//
//  main.cpp
//  LazyFoo26
//
//  Created by Slick on 7/11/18.
//  Copyright © 2018 Slick Coding. All rights reserved.
//

#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>
#include <sdl.h>
#include <SDL_image.h>

/*************************************************************************
 Circle structure
 *************************************************************************/
struct Circle
{
    int x, y;
    int r;
};

/*************************************************************************
 Dot Class
 *************************************************************************/
class Dot
{
public:
    // The dimensions of the dot
    static const int DOT_WIDTH = 20;
    static const int DOT_HEIGHT = 20;

    // Maximum axis velocity of the dot
    static const int DOT_VEL = 1;

    // Constructor
    Dot(int x, int y);

    // Takes key presses and adjusts the dot's velocity
    void handleEvent(SDL_Event& e);

    // Moves the dot and checks collision
    void move(SDL_Rect& square, Circle& circle);

    // Shows the dot
    void render();

    // Get collision circle
    Circle& getCollider();

private:
    // The X and Y offsets of the dot
    int mPosX, mPosY;

    // The velocity of the dot
    int mVelX, mVelY;

    // Dot's collision circle
    Circle mCollider;

    // Moves the collision boxes relative to the dot's offset
    void shiftColliders();
};
/************************************************************************/

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
const int SCREEN_FPS = 60;


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
// Rendered Texture
//LTexture gTextTexture;
LTexture gDotTexture;

/*********************************************************************/

/**************************************************************************
 Function Declarations
 **************************************************************************/
bool init();

bool loadMedia();

void close();

// Circle/Circle collision detector
bool checkCollision(Circle& a, Circle& b);

// Circle/Box collision detector
bool checkCollision(Circle& a, SDL_Rect& b);

// Calculates distance squared between two points
double distanceSquared(int x1, int y1, int x2, int y2);
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
/**************************************************************************/

/*********************************************************************
 Dot Method Declarations
*********************************************************************/
Dot::Dot(int x, int y): mPosX(x), mPosY(y), mVelX(0), mVelY(0)
{
    // Set collision circle size
    mCollider.r = DOT_WIDTH / 2;

    // Move collider relative to the circle
    shiftColliders();
}

void Dot::handleEvent(SDL_Event &e)
{
    // if Key was pressed
    if(e.type == SDL_KEYDOWN && e.key.repeat == 0)
    {
        // Adjust the velocity
        switch(e.key.keysym.sym)
        {
            case SDLK_UP: mVelY -= DOT_VEL; break;
            case SDLK_DOWN: mVelY += DOT_VEL; break;
            case SDLK_LEFT: mVelX -= DOT_VEL; break;
            case SDLK_RIGHT: mVelX += DOT_VEL; break;
        }
    }
    // If a key was released
    else if (e.type == SDL_KEYUP && e.key.repeat == 0)
    {
        // Adjust the velocity
        switch (e.key.keysym.sym)
        {
            case SDLK_UP: mVelY += DOT_VEL; break;
            case SDLK_DOWN: mVelY -= DOT_VEL; break;
            case SDLK_LEFT: mVelX += DOT_VEL; break;
            case SDLK_RIGHT: mVelX -= DOT_VEL; break;
        }
    }
}

void Dot::move(SDL_Rect& square, Circle& circle)
{
    // Move the dot left or right
    mPosX += mVelX;
    shiftColliders();

    // if the dot collided or went too far to the or right
    if((mPosX - mCollider.r < 0) || (mPosX + mCollider.r > SCREEN_WIDTH) || checkCollision(mCollider, square) || checkCollision(mCollider, circle))
    {
        // Move back
        mPosX -= mVelX;
        shiftColliders();
    }

    // Move the dot up or down
    mPosY += mVelY;
    shiftColliders();

    // If the dot went too far up or down
    if((mPosY - mCollider.r < 0) || (mPosY + mCollider.r > SCREEN_HEIGHT) || checkCollision(mCollider, square) || checkCollision(mCollider, circle))
    {
        // Move back
        mPosY -= mVelY;
        shiftColliders();
    }
}

void Dot::render()
{
    // Show the dot
    gDotTexture.render(mPosX - mCollider.r, mPosY - mCollider.r);
}

void Dot::shiftColliders()
{
	//Align collider to center of dot
	mCollider.x = mPosX;
	mCollider.y = mPosY;
}

Circle& Dot::getCollider()
{
	return mCollider;
}
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
            gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED /* | SDL_RENDERER_PRESENTVSYNC*/ );
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

    if(!gDotTexture.loadFromFile("29_circular_collision_detection/dot.bmp"))
    {
        std::cout << "Failed to load dot texture." << std::endl;
        success = false;
    }
    return success;
}

void close()
{
    // Free loaded images
    gDotTexture.free();
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

bool checkCollision(Circle& a, Circle& b)
{
    // Calculate total radius squared
    int totalRadiusSquared = a.r + b.r;
    totalRadiusSquared = totalRadiusSquared * totalRadiusSquared;

    // If the distance between the centers of the circles is less than the sum of their radii
    if(distanceSquared(a.x, a.y, b.x, b.y) < (totalRadiusSquared))
    {
        // the circles have collided
        return true;
    }
    // If not
    return false;
}

bool checkCollision(Circle& a, SDL_Rect& b)
{
    // Closest point on the collision box
    int cX, cY;

    // Find closest x offset
    if(a.x < b.x)
    {
        cX = b.x;
    }
    else if(a.x > b.x + b.w)
    {
        cX = b.x + b.w;
    }
    else
    {
        cX = a.x;
    }

    // Find the closest Y offset
    if(a.y < b.y)
    {
        cY = b.y;
    }
    else if(a.y > b.y + b.h)
    {
        cX = b.y + b.h;
    }
    else
    {
        cY = a.y;
    }

    // If the closest point is inside the circle
    if(distanceSquared(a.x, a.y, cX, cY) < a.r * a.r)
    {
        // This box and circle have collided
        return true;
    }

    // If the shapes have not collided
    return false;
}

double distanceSquared(int x1, int y1, int x2, int y2)
{
    int deltaX = x2 - x1;
    int deltaY = y2 - y1;
    return deltaX*deltaX + deltaY*deltaY;
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
            Dot dot(Dot::DOT_WIDTH / 2, Dot::DOT_HEIGHT / 2);

            // The dot that will be collided against
            Dot otherDot( SCREEN_WIDTH/4, SCREEN_HEIGHT/4);

            // Set the wall
            SDL_Rect wall;
            wall.x = 300;
            wall.y = 40;
            wall.w = 40;
            wall.h = 400;

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
                    // Handle input for the dot
                    dot.handleEvent(e);
                }

                // Move the dot anc check Collision
                dot.move(wall, otherDot.getCollider());

                //Clear screen
                SDL_SetRenderDrawColor( gRenderer, 0xff, 0xff, 0xff, 0xff );
                SDL_RenderClear( gRenderer );

                SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xff);
                SDL_RenderDrawRect(gRenderer, &wall);
               // Render objects
                dot.render();
                otherDot.render();

                //Update screen
                SDL_RenderPresent( gRenderer );


            }
        }
    }

    //Free resources and close SDL
    close();

    return 0;

}




