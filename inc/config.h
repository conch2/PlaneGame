#ifndef __PLANE_GAME_CONFIG_H__
#define __PLANE_GAME_CONFIG_H__

#include <SDL2/SDL.h>
#include <fcntl.h>
#include <string>
#include <sstream>

#define ROOT_IMAGES "resources/images"
#define LOG_FILE    "PlaneGame.log"
#define ENEMY1_DEFAULT_MOVE_DISTANCE 5
#define LEVEL_IMAGE ROOT_IMAGES"/life.png"
#define BOMB_IMAGE  ROOT_IMAGES"/bomb.png"
#define BOMB_SUPPLY_IMAGE  ROOT_IMAGES"/bomb_supply.png"

extern int fpsTick;
extern int PGerrno;
extern int GameState;
extern FILE *LogFile;
extern float ImageProportion;
extern int displayW, displayH;

#endif