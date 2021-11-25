#ifndef __BULLET_H__
#define __BULLET_H__

#include <SDL2/SDL_image.h>

#include "config.h"
#include "collisionBox.h"

#define BULLET_TYPE0 ROOT_IMAGES"/bullet1.png"
#define BULLET_TYPE1 ROOT_IMAGES"/bullet2.png"

class Bullet : public CollisionBox {
public:
	Bullet(int bulletType, SDL_Renderer *renderer, int x, int y);
	Bullet(const Bullet &obj);
	~Bullet();
	// 在使用Bullet类之前调用
	static void Init(SDL_Renderer *renderer);
	// 在退出前调用
	static void Quit();
	void mov();
	void setSpeed(int moveTime, int m_distance);
	void render();
	void setType(int type);
	
	SDL_Renderer *m_renderer;
	int m_bulletType;
	SDL_Surface *m_surface;
	SDL_Texture *m_texture;
	static SDL_Texture *texture1;
	static SDL_Texture *texture2;
	static int imageW, imageH;
	float m_lastDecimal;
	int m_moveTime;
	int m_distance;
	Uint32 m_timeOfLastMove;
	SDL_Rect m_rect;
private:
	friend class Player;
};

#endif