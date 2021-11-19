#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "config.h"
#include "bullet.h"
#include "collisionBox.h"

#define PLAYER_FILES {\
	ROOT_IMAGES"/me1.png",\
	ROOT_IMAGES"/me2.png",\
	ROOT_IMAGES"/me_destroy_1.png",\
	ROOT_IMAGES"/me_destroy_2.png",\
	ROOT_IMAGES"/me_destroy_3.png",\
	ROOT_IMAGES"/me_destroy_4.png",\
	}
// 玩家子弹默认射速
#define PLAYER_DEFAULT_FIRING_RATE 230

typedef enum {
	PLAYER_LIVING,
	// 死亡，播放死亡动画
	PLAYER_DYING,
	// 死透了
	PLAYER_DEATH,
	// 无敌状态
	PLAYER_INVINCIBLE,
} PlanePlayerState;

class Player : public CollisionBox {
public:
	Player(SDL_Renderer *renderer);
	~Player();
	void render();
	void bullet();
	void bulletMovs();
	void onLoop();
	// 越界判断处理
	void crossBounds();
	/** 移动
	 *  dx, dy 范围-1 ~ 1
	 * */
	void mov(float dx, float dy);
	/** 设置图片交换时间
	 * */
	void setSwapTime(Uint32 ttime);
	PlanePlayerState getState();
	// 设置子弹射速
	void setFiringRate(Uint32 rate);
	void resetBoxs();
	// 重新设置碰撞箱位置
	void resetCBovsPosition();
private:
	friend class PlaneGame;
	SDL_Renderer *m_renderer;
	SDL_Rect m_rect;
	SDL_Surface *m_surface;
	SDL_Texture *(*m_textures)[];
	int m_texturesSize;
	// 当前使用图片的texture索引
	int m_textureIndex;
	// 
	Uint32 m_lastSwapTime;
	// 移动系数，手指移动距离乘系数得到最终移动距离
	// 默认为2.0f
	float m_movRatioX, m_movRatioY;
	// 图片交换时间 ms
	Uint32 m_swapTime;
	PlanePlayerState m_state;
	// 子弹
	Bullet *m_bullets;
	// 默认的子弹配置，暨当子弹中的（某些）值等于这个实例时
	// 则表示该实例是空实例
	Bullet *m_defaultBulletConfig;
	int m_bulletsSize;
	Uint32 m_timeOfLastShoot;
	// 射速
	Uint32 m_bulletSpeed;
	static const char *images[];
	static SDL_Texture **Textures;
	static int imagesSize;
};

#endif