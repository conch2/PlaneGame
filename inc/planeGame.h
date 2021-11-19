#ifndef __PLANEGAME_H__
#define __PLANEGAME_H__

#include <string>
#include <sstream>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "font.h"
#include "config.h"
#include "player.h"
#include "enemyPlane.h"
#include "background.h"

#define FONT_FILE    "/system/fonts/DroidSansChinese.ttf"
#define MY_FONT_FILE "resources/fonts/font.ttf"

typedef enum {
	START,
	PLAYING,
	PGEND,
} PlaneGameState;

class PlaneGame {
public:
	PlaneGame();
	virtual ~PlaneGame();
	int init();
	void render();
	// 主游戏循环
	void mainLoop();
	// 碰撞检查
	void scanCollision();
	void renderLevel();
	void renderBomb();
	void supply();
	void renderSupply();
	void rePlay();
	// Debug
	void showCollisionBoxs();
private:
	// 缓冲区
	std::string m_scoreBuff;
	SDL_Window *m_window;
	SDL_Renderer *m_renderer;
	PlaneGameState m_state;
	Font *m_font;
	Background *m_background;
	Player *m_player;
	EnemyPlane *m_enemy;
	// 分数
	unsigned int m_score;
	// 生命值
	int m_level;
	// 生命值图片资源
	SDL_Texture *m_levelTexture;
	SDL_Rect m_levelRect;
	// 玩家无敌持续时间
	Uint32 m_playerIncvDur;
	// 玩家上一次死亡时间
	Uint32 m_playerLastDeathT;
	// 玩家拥有的全屏炸弹数
	int m_fullScreenBomb;
	// 补给间隔
	Uint32 m_intervalOfSupply;
	int m_bombImageW, m_bombImageH;
	int m_bombSupplyImageW, m_bombSupplyImageH;
	SDL_Rect m_bombRect;
	SDL_Rect m_bombSupplyRect;
	SDL_Texture *m_bombTexture;
	SDL_Texture *m_bombSupplyTexture;
	// 最后一次补给时间
	Uint32 m_timeOfLastSupply;
};

#endif