#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>


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

const int TOTAL_DATA = 10;

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
LTexture gPromptTextTexture;
LTexture gDataTextures[TOTAL_DATA];



// Data points
Sint32 gData[ TOTAL_DATA ];

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
    
    // Text rendering color
    SDL_Color textColor = {0, 0, 0, 255};
    SDL_Color highlightColor = { 0xff, 0, 0, 0xff };
    
    // Open the font
    gFont = TTF_OpenFont("33_file_reading_and_writing/lazy.ttf", 28 );
    if( gFont == nullptr )
    {
        std::cout << "Failed to load lazy font! SDL_ttf Error: " << SDL_GetError() ;
        success = false;
    }
    else
    {
        if(!gPromptTextTexture.loadFromRenderedText("Enter data:", textColor))
        {
            std::cout << "Unable to render text!" << std::endl;
            success = false;
        }
    }
    
    // Open file for reading in binary
    SDL_RWops* file = SDL_RWFromFile("33_file_reading_and_writing/nums.bin", "r+b");
    
    // File doesn't exist
    if( file == nullptr)
    {
        std::cout << "Warning: Unable to open file! SDL Error: " << SDL_GetError() << std::endl;
        
        // Create file for writing
        file = SDL_RWFromFile("33_file_reading_and_writing/nums.bin", "w+b");
        if(file == nullptr)
        {
            std::cout << "New file created!" << std::endl;
            
            // Initialize data
            for(auto i = 0; i < TOTAL_DATA; ++i)
            {
                gData[i] = 0;
                SDL_RWwrite(file, &gData[i], sizeof(Sint32), 1);
            }
            
            // Close file handler
            SDL_RWclose(file);
        }
        else
        {
            std::cout << "Error: Unable to create file! SDL Error: " << SDL_GetError() << std::endl;
            success = false;
        }
    }
    // File exists
    else
    {
        // Load data
        std::cout << "Reading file...!" << std::endl;
        for(auto i = 0; i < TOTAL_DATA; ++i)
        {
            SDL_RWread(file, &gData[i], sizeof(Sint32), 1);
        }
        
        // Close File Hanlder
        SDL_RWclose(file);
    }
    
    // Initialize data textures
    gDataTextures[0].loadFromRenderedText(std::to_string((long long)gData[0]), highlightColor);
    
    for(auto i = 1; i < TOTAL_DATA; ++i)
    {
        gDataTextures[i].loadFromRenderedText(std::to_string((long long)gData[i]), textColor);
    }
    
    return success;
}

void close()
{
    // Open data for writing
    SDL_RWops* file = SDL_RWFromFile("33_file_reading_and_writing/nums.bin", "w+b");
    if(file != nullptr)
    {
        // Save data
        for(auto i = 0; i < TOTAL_DATA; ++i)
        {
            SDL_RWwrite(file, &gData, sizeof(Sint32), 1);
        }
        
        // Close file handler
        SDL_RWclose(file);
    }
    else
    {
        std::cout << "Error: Unable to save file! " << SDL_GetError() << std::endl;
        
    }
    
    // Free loaded images
    gPromptTextTexture.free();
    for(auto i = 1; i < TOTAL_DATA; ++i)
    {
        gDataTextures[i].free();
    }
    
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
            SDL_Color highlightColor = {0xff, 0, 0, 0xff};
            
            // Current input point
            int currentData = 0;
            
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
                        switch (e.key.keysym.sym)
                        {
                            // Previous data entry
                            case SDLK_UP:
                                // Rereender previous entry input point
                                gDataTextures[currentData].loadFromRenderedText(std::to_string((long long)gData[currentData]), textColor);
                                --currentData;
                                if(currentData < 0)
                                {
                                    currentData = TOTAL_DATA - 1;
                                }
                                
                                // Rerender current entry input point
                                gDataTextures[currentData].loadFromRenderedText(std::to_string((long long)gData[currentData]), highlightColor);
                                break;
                                
                                // Next data entry
                            case SDLK_DOWN:
                                // Rerender previous entry input point
                                gDataTextures[currentData].loadFromRenderedText(std::to_string((long long)gData[currentData]), textColor);
                                ++currentData;
                                if(currentData == TOTAL_DATA)
                                {
                                    currentData = 0;
                                }
                                
                                // Rerender current entry input point
                                gDataTextures[currentData].loadFromRenderedText(std::to_string((long long)gData[currentData]), highlightColor);
                                break;
                                
                            case SDLK_LEFT:
                                // Decrement input point
                                --gData[currentData];
                                
                                gDataTextures[currentData].loadFromRenderedText(std::to_string((long long)gData[currentData]), highlightColor);
                                break;
                                
                            case SDLK_RIGHT:
                                // Increment input point
                                ++gData[currentData];
                                
                                gDataTextures[currentData].loadFromRenderedText(std::to_string((long long)gData[currentData]), highlightColor);
                                break;
                        }
                    }
                }
                
                
                
                //Clear screen
                SDL_SetRenderDrawColor( gRenderer, 0xff, 0xff, 0xff, 0xff );
                SDL_RenderClear( gRenderer );
                
                // Render textures
                gPromptTextTexture.render((SCREEN_WIDTH - gPromptTextTexture.getWidth()) / 2, 0);
                for(auto i = 0; i < TOTAL_DATA; ++i)
                {
                    gDataTextures[i].render((SCREEN_WIDTH - gDataTextures[i].getWidth()) / 2, gPromptTextTexture.getHeight() + gDataTextures[0].getHeight() * i);
                }
                
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



