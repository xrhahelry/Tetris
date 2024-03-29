#include "../headers/board.h"
#include "../headers/GetTexture.h"
#include "../headers/Tetromino.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <ctime>
#include <string>

Board::Board(SDL_Renderer *Renderer, Menu *mainMenu) {
  BLOCKSIZE = 50;
  boardHeight = BLOCKSIZE * 20;
  boardWidth = BLOCKSIZE * 10;
  VERTICALSHIFT = 40;
  HORIZONTALSHIFT = 710;
  INITIAL_X = 910;
  INITIAL_Y = 40;
  renderer = Renderer;
  grid = {HORIZONTALSHIFT, VERTICALSHIFT, boardWidth, boardHeight};
  bg = {0, 0, 1920, 1080};
  background = GetTexture(renderer, "./assets/img/background.png");
  gridBackground = GetTexture(renderer, "./assets/img/grid.png");
  baseTile = GetTexture(renderer, "./assets/img/block.png");
  tetrominoTile = GetTexture(renderer, "assets/img/block.png");
  nextTetrominoTile = GetTexture(renderer, "assets/img/block.png");
  textColor = {255, 255, 255, 255};
  kanit = TTF_OpenFont("assets/fonts/Kanit-Bold.ttf", 45);
  menu = mainMenu;
  level = 1;
  score = 0;
  totalLinesCleared = 0;

  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 10; j++) {
      valueGrid[i][j] = 0;
      visibleGrid[i][j] = {HORIZONTALSHIFT + (j * BLOCKSIZE),
                           VERTICALSHIFT + (i * BLOCKSIZE), BLOCKSIZE,
                           BLOCKSIZE};
    }
  }
}

Board::~Board() {
  SDL_DestroyTexture(gridBackground);
  SDL_DestroyTexture(baseTile);
  SDL_DestroyTexture(tetrominoTile);
  SDL_DestroyTexture(nextTetrominoTile);
  SDL_DestroyTexture(background);
  TTF_CloseFont(kanit);
  gridBackground = NULL;
  baseTile = NULL;
  tetrominoTile = NULL;
  nextTetrominoTile = NULL;
  background = NULL;
  kanit = NULL;
};

menuState Board::game() {
  SDL_Event e;
  bool quit = false;
  Uint64 currentTicks;
  const SDL_Point INITIAL_POS1 = {INITIAL_X, INITIAL_Y};
  const SDL_Point INITIAL_POS2 = {INITIAL_X, INITIAL_Y + BLOCKSIZE};
  SDL_Point nextPos;
  SDL_Point NEXT_POS1 = {HORIZONTALSHIFT + boardWidth + BLOCKSIZE * 2,
                         VERTICALSHIFT + BLOCKSIZE * 4 + 20};
  SDL_Point NEXT_POS2 = {HORIZONTALSHIFT + boardWidth + BLOCKSIZE * 2,
                         VERTICALSHIFT + BLOCKSIZE * 5 + 20};
  SDL_Rect currentTetromino[4];
  SDL_Point pos;
  menuState currentState = playState;

  Tetromino t(renderer, BLOCKSIZE, tetrominoTile);
  Tetromino next(renderer, BLOCKSIZE, nextTetrominoTile);
  shape current = t.random();
  shape nextPiece = next.random();

  if (current == L_Shape || current == J_Shape || current == T_Shape ||
      nextPiece == L_Shape || nextPiece == J_Shape || nextPiece == T_Shape) {
    pos = INITIAL_POS2;
    nextPos = NEXT_POS2;
  } else {
    pos = INITIAL_POS1;
    nextPos = NEXT_POS1;
  }

  t.update(pos, t.random(), currentTetromino);
  next.update(nextPos, nextPiece, currentTetromino);

  while (!quit) {
    if (currentState == playState) {
      while (SDL_PollEvent(&e)) {
        if (e.type == SDL_KEYDOWN) {
          if (e.key.keysym.sym == SDLK_SPACE) {
            t.rotate(currentTetromino);
            if (collisionBlocks(currentTetromino)) {
              t.rotate(currentTetromino);
              t.rotate(currentTetromino);
              t.rotate(currentTetromino);
            }
          } else if (e.key.keysym.sym == SDLK_LEFT) {
            if (!outOfLeftBounds(currentTetromino)) {
              pos.x -= 50;
              t.update(pos, currentTetromino);
              if (collisionBlocks(currentTetromino)) {
                pos.x += 50;
                t.update(pos, currentTetromino);
              }
            }
          } else if (e.key.keysym.sym == SDLK_RIGHT) {
            if (!outOfRightBounds(currentTetromino)) {
              pos.x += 50;
              t.update(pos, currentTetromino);
              if (collisionBlocks(currentTetromino)) {
                pos.x -= 50;
                t.update(pos, currentTetromino);
              }
            }
          } else if (e.key.keysym.sym == SDLK_DOWN) {
            pos.y += BLOCKSIZE;
            t.update(pos, currentTetromino);

            if (collisionBlocks(currentTetromino)) {
              pos.y -= BLOCKSIZE;
              t.update(pos, currentTetromino);
              insert(currentTetromino, t.getShape(), &quit);
              lineClear();

              if (nextPiece == L_Shape || nextPiece == J_Shape ||
                  nextPiece == T_Shape) {
                pos = INITIAL_POS2;
              } else {
                pos = INITIAL_POS1;
              }

              t.update(pos, nextPiece, currentTetromino);
              nextPiece = next.random();

              if (nextPiece == L_Shape || nextPiece == J_Shape ||
                  nextPiece == T_Shape) {
                nextPos = NEXT_POS2;
              } else {
                nextPos = NEXT_POS1;
              }

              next.update(nextPos, nextPiece, currentTetromino);
            }
          } else if (e.key.keysym.sym == SDLK_UP) {
            bool drop = true;
            while (drop) {
              pos.y += BLOCKSIZE;
              t.update(pos, currentTetromino);

              if (collisionBlocks(currentTetromino)) {
                pos.y -= BLOCKSIZE;
                t.update(pos, currentTetromino);
                insert(currentTetromino, t.getShape(), &quit);
                lineClear();

                if (nextPiece == L_Shape || nextPiece == J_Shape ||
                    nextPiece == T_Shape) {
                  pos = INITIAL_POS2;
                } else {
                  pos = INITIAL_POS1;
                }

                t.update(pos, nextPiece, currentTetromino);
                nextPiece = next.random();

                if (nextPiece == L_Shape || nextPiece == J_Shape ||
                    nextPiece == T_Shape) {
                  nextPos = NEXT_POS2;
                } else {
                  nextPos = NEXT_POS1;
                }

                next.update(nextPos, nextPiece, currentTetromino);
                drop = false;
              } else if (collisionGround(currentTetromino)) {
                insert(currentTetromino, t.getShape(), &quit);
                lineClear();

                if (nextPiece == L_Shape || nextPiece == J_Shape ||
                    nextPiece == T_Shape) {
                  pos = INITIAL_POS2;
                } else {
                  pos = INITIAL_POS1;
                }

                t.update(pos, nextPiece, currentTetromino);
                nextPiece = next.random();

                if (nextPiece == L_Shape || nextPiece == J_Shape ||
                    nextPiece == T_Shape) {
                  nextPos = NEXT_POS2;
                } else {
                  nextPos = NEXT_POS1;
                }

                next.update(nextPos, nextPiece, currentTetromino);
                drop = false;
              }
            }
          } else if (e.key.keysym.sym == SDLK_ESCAPE) {
            currentState = pauseState;
          }
        }
      }

      update();
      t.render();
      next.render();
      SDL_RenderPresent(renderer);

      if (SDL_GetTicks64() - currentTicks >=
          (1000 * (1 - (0.1 * (level - 1))))) {
        currentTicks = SDL_GetTicks64();
        pos.y += BLOCKSIZE;
        t.update(pos, currentTetromino);

        if (collisionBlocks(currentTetromino)) {
          pos.y -= BLOCKSIZE;
          t.update(pos, currentTetromino);
          insert(currentTetromino, t.getShape(), &quit);
          lineClear();

          if (nextPiece == L_Shape || nextPiece == J_Shape ||
              nextPiece == T_Shape) {
            pos = INITIAL_POS2;
          } else {
            pos = INITIAL_POS1;
          }

          t.update(pos, nextPiece, currentTetromino);
          nextPiece = next.random();

          if (nextPiece == L_Shape || nextPiece == J_Shape ||
              nextPiece == T_Shape) {
            nextPos = NEXT_POS2;
          } else {
            nextPos = NEXT_POS1;
          }

          next.update(nextPos, nextPiece, currentTetromino);
        } else if (collisionGround(currentTetromino)) {
          pos.y -= BLOCKSIZE;
          t.update(pos, currentTetromino);
          insert(currentTetromino, t.getShape(), &quit);
          lineClear();

          if (nextPiece == L_Shape || nextPiece == J_Shape ||
              nextPiece == T_Shape) {
            pos = INITIAL_POS2;
          } else {
            pos = INITIAL_POS1;
          }

          t.update(pos, nextPiece, currentTetromino);
          nextPiece = next.random();

          if (nextPiece == L_Shape || nextPiece == J_Shape ||
              nextPiece == T_Shape) {
            nextPos = NEXT_POS2;
          } else {
            nextPos = NEXT_POS1;
          }

          next.update(nextPos, nextPiece, currentTetromino);
        }
      }

      if (quit == true) {
        menu->death(std::to_string(level), std::to_string(score),
                    std::to_string(totalLinesCleared));
        return freshState;
      }
    } else if (currentState == pauseState) {
      currentState = menu->pause();
    } else if (currentState == restartState) {
      return currentState;
    } else if (currentState == quitState) {
      return freshState;
    }
  }
  return currentState;
}

void Board::update() {
  SDL_RenderCopy(renderer, background, NULL, &bg);
  SDL_RenderCopy(renderer, gridBackground, NULL, &grid);

  textBox.x = HORIZONTALSHIFT + boardWidth + BLOCKSIZE;
  textBox.y = VERTICALSHIFT;
  textScore = GetTexture(renderer, kanit, "Score", textColor, &textBox);
  SDL_RenderCopy(renderer, textScore, NULL, &textBox);

  textBox.y += textBox.h;
  textScoreValue =
      GetTexture(renderer, kanit, std::to_string(score), textColor, &textBox);
  SDL_RenderCopy(renderer, textScoreValue, NULL, &textBox);

  textBox.y += textBox.h;
  textNext = GetTexture(renderer, kanit, "Next", textColor, &textBox);
  SDL_RenderCopy(renderer, textNext, NULL, &textBox);

  textBox.x = HORIZONTALSHIFT - BLOCKSIZE * 3;
  textBox.y = VERTICALSHIFT;
  textLevel = GetTexture(renderer, kanit, "Level", textColor, &textBox);
  SDL_RenderCopy(renderer, textLevel, NULL, &textBox);

  textBox.x += BLOCKSIZE * 1;
  textBox.y += textBox.h;
  textLevelValue =
      GetTexture(renderer, kanit, std::to_string(level), textColor, &textBox);
  SDL_RenderCopy(renderer, textLevelValue, NULL, &textBox);

  textBox.x -= BLOCKSIZE * 4 + 20;
  textBox.y += textBox.h;
  textLines = GetTexture(renderer, kanit, "Lines Cleared", textColor, &textBox);
  SDL_RenderCopy(renderer, textLines, NULL, &textBox);

  textBox.x += BLOCKSIZE * 4;
  textBox.y += textBox.h;
  textLinesValue = GetTexture(
      renderer, kanit, std::to_string(totalLinesCleared), textColor, &textBox);
  SDL_RenderCopy(renderer, textLinesValue, NULL, &textBox);

  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 10; j++) {
      if (valueGrid[i][j] != 0) {
        if (valueGrid[i][j] == 1) {
          SDL_SetTextureColorMod(baseTile, 60, 248, 251);
          SDL_RenderCopy(renderer, baseTile, NULL, &visibleGrid[i][j]);
        } else if (valueGrid[i][j] == 2) {
          SDL_SetTextureColorMod(baseTile, 255, 204, 0);
          SDL_RenderCopy(renderer, baseTile, NULL, &visibleGrid[i][j]);
        } else if (valueGrid[i][j] == 3) {
          SDL_SetTextureColorMod(baseTile, 231, 9, 212);
          SDL_RenderCopy(renderer, baseTile, NULL, &visibleGrid[i][j]);
        } else if (valueGrid[i][j] == 4) {
          SDL_SetTextureColorMod(baseTile, 39, 174, 96);
          SDL_RenderCopy(renderer, baseTile, NULL, &visibleGrid[i][j]);
        } else if (valueGrid[i][j] == 5) {
          SDL_SetTextureColorMod(baseTile, 236, 159, 5);
          SDL_RenderCopy(renderer, baseTile, NULL, &visibleGrid[i][j]);
        } else if (valueGrid[i][j] == 6) {
          SDL_SetTextureColorMod(baseTile, 168, 233, 28);
          SDL_RenderCopy(renderer, baseTile, NULL, &visibleGrid[i][j]);
        } else if (valueGrid[i][j] == 7) {
          SDL_SetTextureColorMod(baseTile, 246, 3, 3);
          SDL_RenderCopy(renderer, baseTile, NULL, &visibleGrid[i][j]);
        }
      }
    }
  }
  SDL_DestroyTexture(textNext);
  SDL_DestroyTexture(textScore);
  SDL_DestroyTexture(textLevel);
  SDL_DestroyTexture(textLines);
  SDL_DestroyTexture(textScoreValue);
  SDL_DestroyTexture(textLevelValue);
  SDL_DestroyTexture(textLinesValue);
  textNext = NULL;
  textScore = NULL;
  textLevel = NULL;
  textLines = NULL;
  textScoreValue = NULL;
  textLevelValue = NULL;
  textLinesValue = NULL;
}

bool Board::collisionGround(SDL_Rect piece[4]) {
  for (int i = 0; i < 4; i++) {
    if (piece[i].y >= 1040)
      return true;
  }
  return false;
}

bool Board::outOfLeftBounds(SDL_Rect piece[4]) {
  for (int i = 0; i < 4; i++) {
    if (piece[i].x <= 710) {
      return true;
    }
  }
  return false;
}

bool Board::outOfRightBounds(SDL_Rect piece[4]) {
  for (int i = 0; i < 4; i++) {
    if (piece[i].x >= 1160) {
      return true;
    }
  }
  return false;
}

bool Board::collisionBlocks(SDL_Rect piece[4]) {
  for (int i = 0; i < 4; i++) {
    size_t x = (piece[i].x - 710) / 50;
    size_t y = (piece[i].y - 40) / 50;
    if (valueGrid[y][x] != 0) {
      return true;
    }
  }
  return false;
}

void Board::insert(SDL_Rect piece[4], shape color, bool *dead) {
  for (int i = 0; i < 4; i++) {
    if (piece[i].y <= 40) {
      *dead = true;
    }
    size_t x = (piece[i].x - 710) / 50;
    size_t y = (piece[i].y - 40) / 50;
    valueGrid[y][x] = color + 1;
  }
}

int Board::lineClear() {
  int num = 0;
  for (int i = 19; i > 0; i--) {
    if (valueGrid[i][0] != 0 && valueGrid[i][1] != 0 && valueGrid[i][2] != 0 &&
        valueGrid[i][3] != 0 && valueGrid[i][4] != 0 && valueGrid[i][5] != 0 &&
        valueGrid[i][6] != 0 && valueGrid[i][7] != 0 && valueGrid[i][8] != 0 &&
        valueGrid[i][9] != 0) {
      if (num == 0) {
        update();
        SDL_RenderPresent(renderer);
        SDL_Delay(200);
      }
      num++;
      for (int k = i; k > 0; k--) {
        for (int j = 0; j < 10; j++) {
          valueGrid[k][j] = valueGrid[k - 1][j];
        }
      }
      i++;
    }
  }
  totalLinesCleared += num;
  if (num == 4)
    score += (level * 10 * (num + 2));
  else
    score += (level * 10 * num);

  // if ((totalLinesCleared - ((level - 1) * 10) >= (level * 10) && level < 10))
  if ((totalLinesCleared >= (level * 10) && level < 10))
    level++;
  return num;
}
