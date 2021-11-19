#include "../inc/bullet.h"

int Bullet::imageW = 0;
int Bullet::imageH = 0;
uint Bullet::NumOfObj = 0;
SDL_Texture *Bullet::texture1 = NULL;
SDL_Texture *Bullet::texture2 = NULL;

Bullet::Bullet(int bulletType, SDL_Renderer *renderer, int x, int y) : m_bulletType(bulletType) {
	NumOfObj++;
	m_renderer = renderer;
	if (!texture1) {
		Init(renderer);
	}
	switch (m_bulletType) {
	case 0:
		m_texture = texture1;
		break;
	case 1:
		m_texture = texture2;
		break;
	}
	m_boxs = &m_rect;
	m_rect.w = imageW * ImageProportion;
	m_rect.h = imageH * ImageProportion;
	m_rect.x = x;
	m_rect.y = y;
	m_timeOfLastMove = SDL_GetTicks();
}

Bullet::Bullet(const Bullet &obj) : CollisionBox() {
	NumOfObj++;
	m_renderer = obj.m_renderer;
	m_bulletType = obj.m_bulletType;
	m_texture = obj.m_texture;
	m_boxs = &m_rect;
	m_rect.w = imageW;
	m_rect.h = imageH;
	m_rect.x = obj.m_rect.x;
	m_rect.y = obj.m_rect.y;
	m_timeOfLastMove = SDL_GetTicks();
}

Bullet::~Bullet() {
	if (m_surface) {
		SDL_FreeSurface(m_surface);
	}
	NumOfObj--;
	if ((!NumOfObj) && (GameState = 2)) {
		Quit();
	}
}

void Bullet::Init(SDL_Renderer *renderer) {
	Quit();
	SDL_Surface *surface = IMG_LoadPNG_RW(SDL_RWFromFile(BULLET_TYPE0, "rb"));
	if (!surface) {
		fprintf(LogFile, "Bullet begin load image Error! %s\n", IMG_GetError());
		PGerrno = -2;
		return ;
	}
	texture1 = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	surface = IMG_LoadPNG_RW(SDL_RWFromFile(BULLET_TYPE0, "rb"));
	if (!surface) {
		fprintf(LogFile, "Bullet begin load image Error! %s\n", IMG_GetError());
		PGerrno = -2;
		return ;
	}
	texture2 = SDL_CreateTextureFromSurface(renderer, surface);
	imageW = surface->w;
	imageH = surface->h;
	SDL_FreeSurface(surface);
}

void Bullet::Quit() {
	if (texture1) {
		SDL_DestroyTexture(texture1);
		texture1 = NULL;
	}
	if (texture2) {
		SDL_DestroyTexture(texture2);
		texture2 = NULL;
	}
}

void Bullet::render() {
	SDL_RenderCopy(m_renderer, m_texture, NULL, &m_rect);
}

void Bullet::mov() {
	Uint32 nowT = SDL_GetTicks();
	float speed = (float)(nowT - m_timeOfLastMove) / (float)m_moveTime;
	speed *= m_distance;
	speed += m_lastDecimal;
	int movT = speed;
	m_lastDecimal = speed - movT;
	m_rect.y -= movT;
	if (movT) {
		m_timeOfLastMove = SDL_GetTicks();
	}
}

void Bullet::setSpeed(int moveTime, int distance) {
	m_moveTime = moveTime;
	m_distance = distance;
}