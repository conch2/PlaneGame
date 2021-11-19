#ifndef __COLLISION_BOX_H__
#define __COLLISION_BOX_H__

#include <SDL2/SDL.h>

#include "config.h"

class CollisionBox {
public:
	CollisionBox();
	virtual ~CollisionBox();
	int detection(CollisionBox *cb);
	int detection(const SDL_Rect &rect);
	int detection(const SDL_Rect *boxs, int boxsSize);
	void setBoxs(int size);
	void resetBoxXY(int index, int x, int y);
	void resetBoxWH(int index, int w, int h);
	int getBoxsSize();
	SDL_Rect *getBoxs();
protected:
	SDL_Rect *m_boxs;
	int m_boxsSize;
};

#endif