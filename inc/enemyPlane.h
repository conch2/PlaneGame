#ifndef __ENEMY_PLANE_H__
#define __ENEMY_PLANE_H__

#include <SDL2/SDL_image.h>

#include "player.h"
#include "config.h"
#include "collisionBox.h"

typedef enum {
	// 还活着
	ENEMY_ALIVE,
	// 正在死亡
	ENEMY_IN_DEATH,
	// 死亡
	ENEMY_DEATH,
	// 被射中
	ENEMY_SHOT,
} EnemyState;

class Enemy1 : public CollisionBox {
public:
	Enemy1();
	Enemy1(const Enemy1 &obj);
	Enemy1(SDL_Texture *texture, SDL_Rect rect, Uint32 lastMoveTime,
			Uint32 moveSpeed, int distance, SDL_Rect *boxs, int boxsSize);
	~Enemy1();
	void render();
	void setSpeed(Uint32 moveSpeed, int distance);
	static void Init(SDL_Renderer *renderer);
	static void Quit();
protected:
	friend class PlaneGame;
	friend class EnemyPlane;
	SDL_Rect m_rect;
	// 当前使用的图片
	SDL_Texture *m_texture;
	Uint32 m_timeOfDeath;
	// 死亡动画播放时长
	Uint32 m_deathAnimationDuration;
	// 最后一次移动的时间
	Uint32 m_lastMoveTime;
	// 每隔m_moveSpeed移动m_distance个像数
	Uint32 m_moveSpeed;
	int m_distance;
	// 当前飞机状态
	EnemyState m_state;
	// 新生成的飞机移速
	static Uint32 MoveSpeed;
	static int Distance;
	// 有多少张图片
	static int imagesSize;
	static const char *images[];
	static SDL_Texture **Textures;
	// 记录实例对象个数
	static int numOfExample;
	static SDL_Renderer *Renderer;
	static int imageW, imageH;
};

class MidEnemy : public CollisionBox {
public:
	MidEnemy();
	~MidEnemy();
	void render();
	void move();
	static void Init(SDL_Renderer *renderer);
	static void Quit();
private:
	friend class PlaneGame;
	friend class EnemyPlane;
	Uint32 m_deathAnimationDuration;
	EnemyState m_state;
	SDL_Rect m_rect;
	Uint32 m_lastMoveTime;
	Uint32 m_timeOfDeath;
	Uint32 m_moveSpeed;
	int m_distance;
	// 血量
	int m_bloodVolume;
	// 什么时候被打中
	Uint32 m_whenWasItShot;
	SDL_Texture *m_texture;
	// 新生成的飞机血量
	static int NewPlaneBloodVolume;
	// 被打中时图片持续时长
	static Uint32 DOAwhenShot;
	static Uint32 MoveSpeed;
	static int Distance;
	static int imagesSize;
	static int imageW, imageH;
	static const char *images[];
	static SDL_Renderer *Renderer;
	static SDL_Texture **Textures;
};

class BigEnemy : public CollisionBox {
public:
	BigEnemy();
	~BigEnemy();
	void render();
	void move();
	void swapImage();
	static void Init(SDL_Renderer *renderer);
	static void Quit();
private:
	friend class PlaneGame;
	friend class EnemyPlane;
	Uint32 m_deathAnimationDuration;
	EnemyState m_state;
	SDL_Rect m_rect;
	Uint32 m_lastMoveTime;
	Uint32 m_timeOfDeath;
	Uint32 m_moveSpeed;
	Uint32 m_lastSwapTime;
	int m_distance;
	// 血量
	int m_bloodVolume;
	// 什么时候被打中
	Uint32 m_whenWasItShot;
	SDL_Texture *m_texture;
	// 新生成的飞机血量
	static int NewPlaneBloodVolume;
	// 被打中时图片持续时长
	static Uint32 DOAwhenShot;
	static Uint32 MoveSpeed;
	static int Distance;
	static int imagesSize;
	static int imageW, imageH;
	static const char *images[];
	static SDL_Renderer *Renderer;
	static SDL_Texture **Textures;
	static Uint32 SwapTime;
};

class EnemyPlane {
public:
	EnemyPlane(SDL_Renderer *renderer);
	virtual ~EnemyPlane();
	void playing();
	void render();
	void rePlay();
	int playerCollision(Player &player);
	static void Init(SDL_Renderer *renderer);
	static void Quit();
private:
	friend class PlaneGame;
	Enemy1 *m_enemy1;
	int m_enemy1Size;
	MidEnemy *m_midEnemy;
	int m_midEnemySize;
	BigEnemy *m_bigEnemy;
	int m_bigEnemySize;
	// 最后一次生成Enemy1的时间
	Uint32 m_lastCreateEnemy1;
	// 生成敌机1的间隔
	Uint32 m_intervalOfNewEnemy1;
	Uint32 m_lastCreateMidEnemy;
	Uint32 m_intervalOfNewMidEnemy;
	Uint32 m_lastCreateBigEnemy;
	Uint32 m_intervalOfNewBigEnemy;
};

#endif