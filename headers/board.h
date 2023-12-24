#pragma once
#include <SDL2/SDL.h>
#include <fstream>
#include "../headers/Tetromino.h"

class Board
{
private:
    int valueGrid[20][10];
    SDL_Rect visibleGrid[20][10];
    SDL_Rect background;
    SDL_Renderer *renderer;
    SDL_Texture *baseTile;
    int boardWidth;
    int boardHeight;
    int HORIZONTALSHIFT;
    int VERTICALSHIFT;
    int BLOCKSIZE;
    std::fstream game_state;

public:
    Board(SDL_Renderer *Renderer);
    ~Board();
    void game();
    void update();
    void updateLog();
    bool collisionGround(SDL_Rect piece[4]);
    bool outOfLeftBounds(SDL_Rect piece[4]);
    bool outOfRightBounds(SDL_Rect piece[4]);
    bool collisionBlocks(SDL_Rect piece[4]);
    void insert(SDL_Rect piece[4], shape color, bool *dead);
    int lineClear();
};