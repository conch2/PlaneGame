#ifndef __SUPPLY_H__
#define __SUPPLY_H__

#include "font.h"
#include "config.h"

#include <vector>
#include <SDL2/SDL_image.h>

typedef struct {
	SDL_Texture *t;
	SDL_Rect r;
	Uint32 lastMoveT;
	Uint32 moveSpeed;
	Uint32 distance;
} TRectM;

typedef struct {
	SDL_Texture *t;
	SDL_Rect r;
} TR;

class Supply {
public:
	Supply(SDL_Renderer *renderer);
	~Supply();
	void playing();
	void render();
	void setNumFSB(int num);
	int getNumFSB();
	void setFont(Font *font);
private:
	friend class PlaneGame;
	Font *m_font;
	SDL_Renderer *m_renderer;
	// 全屏炸弹数
	int m_numOfFullScreenBomb;
	// 补给间隔
	Uint32 m_intervalOfSupply;
	// 最后一次补给时间
	Uint32 m_timeOfLastSupply;
	int m_bombImageW, m_bombImageH;
	int m_bombSupplyImageW, m_bombSupplyImageH;
	int m_bulletSupplyImageW, m_bulletSupplyImageH;
	SDL_Texture *m_bombTexture;
	SDL_Texture *m_bombSupplyTexture;
	SDL_Texture *m_bulletSupplyTexture;
	SDL_Rect m_bombRect;
	TR m_numBombTR;
	std::vector<TRectM> *m_renderTable;
};

#endif