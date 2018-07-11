#include <cstdio>
#include <string>
#include <sdl.h>

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

            // Current rendered texture
            LTexture* currentTexture = nullptr;

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
                }

                // Set texture base on current keystate
                const Uint8* currentKeyStates = SDL_GetKeyboardSat


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
