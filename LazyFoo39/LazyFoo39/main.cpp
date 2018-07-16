//
//  main.cpp
//  LazyFoo26
//
//  Created by Slick on 7/11/18.
//  Copyright � 2018 Slick Coding. All rights reserved.
//

#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

/**********************************************************************
 Constants
 **********************************************************************/
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// The dimensions of the level
const int LEVEL_WIDTH = 1280;
const int LEVEL_HEIGHT = 960;

// Tile constants
const int TILE_WIDTH = 80;
const int TILE_HEIGHT = 80;
const int TOTAL_TILES = 192;
const int TOTAL_TILE_SPRITES = 12;

// The different tile sprites
const int TILE_RED = 0;
const int TILE_GREEN = 1;
const int TILE_BLUE = 2;
const int TILE_CENTER = 3;
const int TILE_TOP = 4;
const int TILE_TOPRIGHT = 5;
const int TILE_RIGHT = 6;
const int TILE_BOTTOMRIGHT = 7;
const int TILE_BOTTOM = 8;
const int TILE_BOTTOMLEFT = 9;
const int TILE_LEFT = 10;
const int TILE_TOPLEFT = 11;

// Number of enemies
const int TOTAL_ENEMIES = 500;
/*********************************************************************/

/*************************************************************************
 Tile Class
*************************************************************************/
class Tile
{
public:
    // Initializes position and type
    Tile(int x, int y, int tileType);
    
    // Show the tile
    void render(SDL_Rect& camera);
    
    // Get the tile type
    int getType();
    
    // Get the collision box
    SDL_Rect getBox();
    
private:
    // The attributes of the tile
    SDL_Rect mBox;
    
    // The tile type
    int mType;
};
/*********************************************************************/

/*************************************************************************
 UIBar Class
 *************************************************************************/
class UIBar
{
public:
    // Constructor
    UIBar(SDL_Rect& camera, SDL_Color color, int width, int height, int offsetX = 0, int offsetY = 0);
    
    // Show UIBar
    void render();
    
    void updatePosition(SDL_Rect& camera);
    
    void updateWidth(int value);
    
    int getWidth() { return bar.w; }
private:
    SDL_Rect bar;
    SDL_Color color;
    int max = 100;
    int offsetX = 0;
    int offsetY = 0;
    int width;
};
/*********************************************************************/

/*************************************************************************
 Dot Class
 *************************************************************************/
class Dot
{
public:
    // The dimensions of the dot
    static const int DOT_WIDTH = 20;
    static const int DOT_HEIGHT = 20;
    static const int DOT_ID = 0;
    
    // Maximum axis velocity of the dot
    static const int DOT_VEL = 10;
    
    // Constructor
    Dot(int x = 0, int y = 0);
    
    // Takes key presses and adjusts the dot's velocity
    void handleEvent(SDL_Event& e);
    
    // Moves the dot and checks collision
    void move(Tile *tiles[]);
    
    // Centers the camera over the dot
    void setCamera(SDL_Rect& camera);
    
    // Shows the dot on the screen relative to the camera
    void render(SDL_Rect& camera);
    
    SDL_Rect& getBox() { return mBox; }
    
    int getHealth() { return health; }
    
    void updateHealth(int value) { health -= value; }
    

private:
    // Collision box of the dot
    SDL_Rect mBox;
    
    int mPosX, mPosY;
    // The velocity of the dot
    int mVelX, mVelY;
    
    int health = 100;
    
    
};
/************************************************************************/

/*************************************************************************
 Enemy class
*************************************************************************/
class Enemy
{
public:
    // Enenmy dimensions
    static const int ENEMY_WIDTH = 20;
    static const int ENEMY_HEIGHT = 20;
    
    // Maximum velocity
    static const int ENEMY_VEL = 5;
    
    Enemy(int x, int y);
    
    // Moves the enemy and checks collision
    void move(Tile* tiles[]);
    
    // Shows the enemy
    void render(SDL_Rect& camera);
    
    SDL_Rect& getBox() { return mBox; }
private:
    SDL_Rect mBox;
    
    // Starting position
    int mPosX, mPosY;
    
    int mVelX, mVelY;
    
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

LTexture gDotTexture;
LTexture gEnemyTexture;
LTexture gTileTexture;
SDL_Rect gTileClips[TOTAL_TILE_SPRITES];

/*********************************************************************/

/**************************************************************************
 Function Declarations
 **************************************************************************/
bool init();

bool loadMedia(Tile* tiles[]);

void close(Tile* tiles[]);

bool checkCollision(SDL_Rect a, SDL_Rect b);

bool touchesWall(SDL_Rect box, Tile* tiles[]);

bool setTiles(Tile* tiles[]);

/************************************************************************/

/*********************************************************************
 Tile Method Declarations
*********************************************************************/
Tile::Tile(int x, int y, int tileType)
{
    // Get the offsets
    mBox.x = x;
    mBox.y = y;
    
    // Set the collision box
    mBox.w = TILE_WIDTH;
    mBox.h = TILE_HEIGHT;
    
    // Get the tile type
    mType = tileType;
}

void Tile::render(SDL_Rect& camera)
{
    // If tile is on screen
    if(checkCollision(camera, mBox))
    {
        // Show the tile
        gTileTexture.render(mBox.x - camera.x, mBox.y - camera.y, &gTileClips[mType]);
    }
}

int Tile::getType()
{
    return mType;
}
SDL_Rect Tile::getBox()
{
    return mBox;
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
 Enemy Method Declarations
*********************************************************************/
Enemy::Enemy(int x, int y): mPosX(x), mPosY(y), mVelX(0), mVelY(0)
{
    mBox.x = x;
    mBox.y = y;
    mBox.w = ENEMY_WIDTH;
    mBox.h = ENEMY_HEIGHT;
}

void Enemy::move(Tile* tiles[])
{
    // Move the enemy left or right
    int spreadX = sin(rand())*150;
    if( spreadX >= 0 && spreadX <= 10)
    {
        mVelX = sin(rand())*5;
    }

    mBox.x += mVelX;
    
    
    // if the dot collided or went too far to the or right
    if((mBox.x < 0) || (mBox.x + ENEMY_WIDTH > LEVEL_WIDTH) || touchesWall(mBox, tiles))
    {
        // Move back
        mBox.x -= mVelX;
    }
    int spreadY = sin(rand())*150;
    if( spreadY >= 0 && spreadY <= 10)
    {
        mVelY = sin(rand())*5;
    }

    // Move the dot up or down
    mBox.y += mVelY;
    
    // If the dot went too far up or down
    if((mBox.y < 0) || (mBox.y + ENEMY_HEIGHT > LEVEL_HEIGHT) || touchesWall(mBox, tiles))
    {
        // Move back
        mBox.y -= mVelY;
    }
}

void Enemy::render(SDL_Rect &camera)
{
    // If enemy is on the screen
    if(checkCollision(camera, mBox))
    {
        gEnemyTexture.render(mBox.x - camera.x, mBox.y - camera.y);
    }
}

/**************************************************************************/

/*********************************************************************
 Dot Method Declarations
 *********************************************************************/
Dot::Dot(int x, int y): mVelX(0), mVelY(0)
{
    mBox.x = x;
    mBox.y = y;
    mBox.w = DOT_WIDTH;
    mBox.h = DOT_HEIGHT;
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

void Dot::move(Tile* tiles[])
{
    // Move the dot left or right
    mBox.x += mVelX;
    
    
    // if the dot collided or went too far to the or right
    if((mBox.x < 0) || (mBox.x + DOT_WIDTH > LEVEL_WIDTH) || touchesWall(mBox, tiles))
    {
        // Move back
        mBox.x -= mVelX;
    }
    
    // Move the dot up or down
    mBox.y += mVelY;
    
    // If the dot went too far up or down
    if((mBox.y < 0) || (mBox.y + DOT_HEIGHT > LEVEL_HEIGHT) || touchesWall(mBox, tiles))
    {
        // Move back
        mBox.y -= mVelY;
    }
}

void Dot::setCamera(SDL_Rect &camera)
{
    // Center the camera over the dot
    camera.x = (mBox.x + DOT_WIDTH / 2) - SCREEN_WIDTH / 2;
    camera.y = (mBox.y + DOT_HEIGHT / 2) - SCREEN_HEIGHT /2;
    
    // Keept the camera in bounds
    if(camera.x < 0)
    {
        camera.x = 0;
    }
    if(camera.y < 0)
    {
        camera.y = 0;
    }
    if(camera.x > LEVEL_WIDTH - camera.w)
    {
        camera.x = LEVEL_WIDTH - camera.w;
    }
    if(camera.y > LEVEL_HEIGHT - camera.h)
    {
        camera.y = LEVEL_HEIGHT - camera.h;
    }
}


void Dot::render(SDL_Rect& camera)
{
    // Show the dot
    gDotTexture.render(mBox.x - camera.x, mBox.y - camera.y);
}

/***************************************************************************
 UIBar Definitions
***************************************************************************/
UIBar::UIBar(SDL_Rect& camera, SDL_Color color, int width, int height, int offsetX, int offsetY):
            color(color), offsetX(offsetX), offsetY(offsetY)
{
    
    bar.x = camera.x + offsetX;
    bar.y = camera.y + offsetY;
    bar.w = width;
    bar.h = height;
}

// Show UIBar
void UIBar::render()
{
    SDL_SetRenderDrawColor(gRenderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(gRenderer, &bar);
}

void UIBar::updatePosition(SDL_Rect& camera)
{
    bar.x = camera.w - SCREEN_WIDTH + offsetX;
    bar.y = camera.h - SCREEN_HEIGHT  + offsetY;
}

void UIBar::updateWidth(int value)
{
    bar.w -= value;
}
/***************************************************************************/

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
    SDL_SetWindowFullscreen(gWindow, SDL_WINDOW_FULLSCREEN);
    return success;
}

bool loadMedia(Tile* tiles[])
{
    // Loading success flag
    bool success = true;

    
    if(!gDotTexture.loadFromFile("39_tiling/dot.bmp"))
    {
        std::cout << "Failed to load dot texture." << std::endl;
        success = false;
    }
    
    if(!gEnemyTexture.loadFromFile("39_tiling/enemy.png"))
    {
        std::cout << "Failed to load dot texture." << std::endl;
        success = false;
    }
    
    if(!gTileTexture.loadFromFile("39_tiling/tiles.png"))
    {
        std::cout << "Failed to load dot texture." << std::endl;
        success = false;
    }
    
    // Load tile map
    if(!setTiles(tiles))
    {
        std::cout << "Failed to load tile set!" << std::endl;
        success = false;
    }
    return success;
}


bool setTiles(Tile* tiles[])
{
    // Success flag
    bool tilesLoaded = true;
    
    // the tile offsets
    int x = 0, y = 0;
    
    // Open the map
    std::ifstream map("39_tiling/lazy.map");
    if(!map.is_open())
    {
        std::cout << "Unable to load map file!" << std::endl;
        tilesLoaded = false;
    }
    else
    {
        // Initialize the tiles
        for(auto i = 0; i < TOTAL_TILES; ++i)
        {
            // Determines what kind of tile will be made
            int tileType = -1;
            
            // read tile from map file
            map >> tileType;
            
            // If there was a problem in reading the map
            if(map.fail())
            {
                // Stop loading map
                std::cout << "Error loading map: Unexpected end of file!" << std::endl;
                tilesLoaded = false;
                break;
            }
            
            // If tthe number is a valid tile number
            if((tileType >= 0) && (tileType < TOTAL_TILE_SPRITES))
            {
                tiles[i] = new Tile(x, y, tileType);
            }
            // If we don't recognize the tile type
            else
            {
                // stop load map
                std::cout << "Error loading map: Invalid tile type at " << i << std::endl;
                tilesLoaded = false;
                break;
            }
            
            // Move to the next tile spot
            x += TILE_WIDTH;
            
            // If we've goine too far
            if(x >= LEVEL_WIDTH)
            {
                // Move back
                x = 0;
                
                // Move to the next row
                y += TILE_HEIGHT;
            }
        }
        
        // clip the sprite sheet
        if(tilesLoaded)
        {
            gTileClips[TILE_RED].x = 0;
            gTileClips[TILE_RED].y = 0;
            gTileClips[TILE_RED].w = TILE_WIDTH;
            gTileClips[TILE_RED].h = TILE_HEIGHT;
            
            gTileClips[TILE_GREEN].x = 0;
            gTileClips[TILE_GREEN].y = 80;
            gTileClips[TILE_GREEN].w = TILE_WIDTH;
            gTileClips[TILE_GREEN].h = TILE_HEIGHT;
            
            gTileClips[TILE_BLUE].x = 0;
            gTileClips[TILE_BLUE].y = 160;
            gTileClips[TILE_BLUE].w = TILE_WIDTH;
            gTileClips[TILE_BLUE].h = TILE_HEIGHT;
            
            gTileClips[TILE_TOPLEFT].x = 80;
            gTileClips[TILE_TOPLEFT].y = 0;
            gTileClips[TILE_TOPLEFT].w = TILE_WIDTH;
            gTileClips[TILE_TOPLEFT].h = TILE_HEIGHT;
            
            gTileClips[TILE_LEFT].x = 80;
            gTileClips[TILE_LEFT].y = 80;
            gTileClips[TILE_LEFT].w = TILE_WIDTH;
            gTileClips[TILE_LEFT].h = TILE_HEIGHT;
            
            gTileClips[TILE_BOTTOMLEFT].x = 80;
            gTileClips[TILE_BOTTOMLEFT].y = 160;
            gTileClips[TILE_BOTTOMLEFT].w = TILE_WIDTH;
            gTileClips[TILE_BOTTOMLEFT].h = TILE_HEIGHT;
            
            gTileClips[TILE_TOP].x = 160;
            gTileClips[TILE_TOP].y = 0;
            gTileClips[TILE_TOP].w = TILE_WIDTH;
            gTileClips[TILE_TOP].h = TILE_HEIGHT;
            
            gTileClips[TILE_CENTER].x = 160;
            gTileClips[TILE_CENTER].y = 80;
            gTileClips[TILE_CENTER].w = TILE_WIDTH;
            gTileClips[TILE_CENTER].h = TILE_HEIGHT;
            
            gTileClips[TILE_BOTTOM].x = 160;
            gTileClips[TILE_BOTTOM].y = 160;
            gTileClips[TILE_BOTTOM].w = TILE_WIDTH;
            gTileClips[TILE_BOTTOM].h = TILE_HEIGHT;
            
            gTileClips[TILE_TOPRIGHT].x = 240;
            gTileClips[TILE_TOPRIGHT].y = 0;
            gTileClips[TILE_TOPRIGHT].w = TILE_WIDTH;
            gTileClips[TILE_TOPRIGHT].h = TILE_HEIGHT;
            
            gTileClips[TILE_RIGHT].x = 240;
            gTileClips[TILE_RIGHT].y = 80;
            gTileClips[TILE_RIGHT].w = TILE_WIDTH;
            gTileClips[TILE_RIGHT].h = TILE_HEIGHT;
            
            gTileClips[TILE_BOTTOMRIGHT].x = 240;
            gTileClips[TILE_BOTTOMRIGHT].y = 160;
            gTileClips[TILE_BOTTOMRIGHT].w = TILE_WIDTH;
            gTileClips[TILE_BOTTOMRIGHT].h = TILE_HEIGHT;
        }
    }
    map.close();
    
    return tilesLoaded;
}

bool touchesWall(SDL_Rect box, Tile* tiles[])
{
    // Go throught the tiles
    for(auto i = 0; i < TOTAL_TILES; ++i)
    {
        // If the tile is a wall type tile
        if((tiles[i]->getType() >= TILE_CENTER) && (tiles[i]->getType() <= TILE_TOPLEFT))
        {
            // If the collision box touches the wall tile
            if(checkCollision(box, tiles[i]->getBox()))
            {
                return true;
            }
        }
    }
    // If no wall tiles were touches
    return false;
}

bool checkCollision( SDL_Rect a, SDL_Rect b )
{
    //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;
    
    //Calculate the sides of rect A
    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;
    
    //Calculate the sides of rect B
    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;
    
    //If any of the sides from A are outside of B
    if( bottomA <= topB )
    {
        return false;
    }
    
    if( topA >= bottomB )
    {
        return false;
    }
    
    if( rightA <= leftB )
    {
        return false;
    }
    
    if( leftA >= rightB )
    {
        return false;
    }
    
    //If none of the sides from A are outside B
    return true;
}

void close( Tile* tiles[] )
{
    //Deallocate tiles
    for( int i = 0; i < TOTAL_TILES; ++i )
    {
        if( tiles[ i ] == NULL )
        {
            delete tiles[ i ];
            tiles[ i ] = NULL;
        }
    }
    
    //Free loaded images
    gDotTexture.free();
    gTileTexture.free();
    
    //Destroy window
    SDL_DestroyRenderer( gRenderer );
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;
    gRenderer = NULL;
    
    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}


/**************************************************************************
 Main
 **************************************************************************/
int main( int argc, char* args[] )
{
    Tile* tileSet[TOTAL_TILES];
    
    bool gameover = false;
    
    //Start up SDL and create window
    if( !init() )
    {
        printf( "Failed to initialize!\n" );
    }
    else
    {
        
        //Load media
        if( !loadMedia(tileSet) )
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
            Dot dot;
            
            Enemy* enemies[TOTAL_ENEMIES];
            for(auto i = 0; i < TOTAL_ENEMIES; ++i)
            {
                enemies[i] = new Enemy(0, SCREEN_HEIGHT);
            }
            
            
            
            SDL_Rect camera;
            camera.x = LEVEL_WIDTH / 2;
            camera.y = LEVEL_HEIGHT / 2;
            camera.w = SCREEN_WIDTH;
            camera.h = SCREEN_HEIGHT;
            
            SDL_Color backgroundBarColor = {0x00, 0x00, 0x00, 0x44};
            UIBar backgroundBar(camera, backgroundBarColor, 120, 40, 0, 0);
            
            SDL_Color healthBarColor = {0x00, 0xff, 0x00, 0xbb};
            UIBar healthBar(camera, healthBarColor, 100, 20, 10, 10);

            //While application is running
            while( !quit && !gameover )
            {
                healthBar.updatePosition(camera);
                
                backgroundBar.updatePosition(camera);
                
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
                dot.move(tileSet);
                
                for(auto i = 0; i < TOTAL_ENEMIES; ++i)
                {
                    enemies[i]->move(tileSet);
                    if(checkCollision(dot.getBox(), enemies[i]->getBox()))
                    {
                        dot.updateHealth(1);
                        healthBar.updateWidth(1);
                        if(dot.getHealth() <= 0)
                        {
                            gameover = true;
                        }
                    }
                }
                
                
                dot.setCamera(camera);
                
                
                //Clear screen
                SDL_SetRenderDrawColor( gRenderer, 0xff, 0xff, 0xff, 0xff );
                SDL_RenderClear( gRenderer );
                SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
                // render level
                for(auto i = 0; i < TOTAL_TILES; ++i)
                {
                    tileSet[i]->render(camera);
                }
                
                // Render objects
                dot.render(camera);
                for(auto i = 0; i < TOTAL_ENEMIES; ++i)
                {
                    enemies[i]->render(camera);
                }
                
                
                backgroundBar.render();
                healthBar.render();
                
                //Update screen
                SDL_RenderPresent( gRenderer );
                
                
            }
            for(auto i = 0; i < TOTAL_ENEMIES; ++i)
            {
                delete enemies[i];
            }
        }
        
    }
    if(gameover)
    {
        std::cout << "Game Over." << std::endl;
    }
    
    close(tileSet);
    
    return 0;
    
}




