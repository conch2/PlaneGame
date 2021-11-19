#include "../inc/player.h"

const char *Player::images[] = PLAYER_FILES;
SDL_Texture **Player::Textures = NULL;
int Player::imagesSize = sizeof(Player::images) / sizeof(char*);

Player::Player(SDL_Renderer *renderer) : CollisionBox(), m_renderer(renderer) {
	m_state = PLAYER_LIVING;
	m_surface = NULL;
	m_texturesSize = imagesSize;
	m_textures = (SDL_Texture*(*)[])SDL_malloc(sizeof(SDL_Texture*)*m_texturesSize);
	SDL_memset(m_textures, 0, sizeof(SDL_Texture*)*m_texturesSize);
	if (!m_textures) {
		fprintf(LogFile, "Player malloc textures Error! %s\n", SDL_GetError());
		PGerrno = -2;
		m_texturesSize = 0;
		return ;
	}
	m_surface = IMG_LoadPNG_RW(SDL_RWFromFile(images[0], "rb"));
	if (!m_surface) {
		fprintf(LogFile, "Player Load image Error! Error: %s\n", IMG_GetError());
		PGerrno = -2;
		return;
	}
	// 将显示图像大小设为图像大小
	m_rect.w = m_surface->w * ImageProportion;
	m_rect.h = m_surface->h * ImageProportion;
	(*m_textures)[0] = SDL_CreateTextureFromSurface(m_renderer, m_surface);
	SDL_FreeSurface(m_surface);
	
	for (int i=1; i < imagesSize; i++) {
		m_surface = IMG_LoadPNG_RW(SDL_RWFromFile(images[i], "rb"));
		if (!m_surface) {
			fprintf(LogFile, "Player Load image Error! Error: %s\n", IMG_GetError());
			PGerrno = -2;
			return;
		}
		(*m_textures)[i] = SDL_CreateTextureFromSurface(m_renderer, m_surface);
	}
	SDL_FreeSurface(m_surface);
	m_surface = NULL;
	m_rect.x = (displayW - m_rect.w) / 2;
	m_rect.y = displayH;
	m_movRatioX = m_movRatioY = 2.0f;
	m_swapTime = 200;
	// 碰撞箱设置
	setBoxs(2);
	resetBoxs();
	// 子弹
	m_bulletsSize = 10;
	m_bullets = (Bullet*)SDL_malloc(sizeof(Bullet)*m_bulletsSize);
	if (!m_bullets) {
		fprintf(LogFile, "Player init bullet Error! %s\n", SDL_GetError());
		PGerrno = -2;
		return ;
	}
	m_defaultBulletConfig = new Bullet(0, m_renderer, 0, -1);
	m_defaultBulletConfig->setSpeed(1, 2);
	m_defaultBulletConfig->m_rect.y = -1 - m_defaultBulletConfig->m_rect.h;
	for (int i=0; i < m_bulletsSize; i++) {
		m_bullets[i] = *m_defaultBulletConfig;
	}
	m_bulletSpeed = PLAYER_DEFAULT_FIRING_RATE;
	m_textureIndex = 0;
	m_lastSwapTime = SDL_GetTicks();
	m_timeOfLastShoot = SDL_GetTicks();
}

Player::~Player() {
	if (m_defaultBulletConfig) {
		delete m_defaultBulletConfig;
	}
	if (m_bullets) {
		delete [] m_bullets;
	}
	if (m_surface) {
		SDL_FreeSurface(m_surface);
	}
	for (int i=0; i < m_texturesSize; i++) {
		if (!(*m_textures)[i]) {
			continue;
		}
		SDL_DestroyTexture((*m_textures)[i]);
	}
	SDL_free(m_textures);
}

void Player::render() {
	for (int i=0; i < m_bulletsSize; i++) {
		if ((m_bullets[i].m_rect.y + m_bullets[i].m_rect.h) > 0) {
			m_bullets[i].render();
		}
	}
	if (m_textureIndex < m_texturesSize) {
		SDL_RenderCopy(m_renderer, (*m_textures)[m_textureIndex], NULL, &m_rect);
	}
}

void Player::onLoop() {
	Uint32 nTime = SDL_GetTicks();
	if (nTime - m_lastSwapTime >= m_swapTime) {
		m_lastSwapTime = nTime;
		m_textureIndex++;
	}
	switch (m_state) {
	case PLAYER_DYING:
		if (m_textureIndex >= m_texturesSize) {
			m_textureIndex = 0;
			m_state = PLAYER_DEATH;
		}
		break;
	case PLAYER_INVINCIBLE:
		if (m_textureIndex == 1) {
			m_textureIndex = m_texturesSize;
		} else if (m_textureIndex > m_texturesSize) {
			m_textureIndex = 0;
		}
		break;
	default:
		if (m_textureIndex >= 2) {
			m_textureIndex = 0;
		}
		break;
	}
}

void Player::crossBounds() {
	if (m_rect.x < 0) {
		m_rect.x = 0;
	} else if (m_rect.x + m_rect.w > displayW) {
		m_rect.x = displayW - m_rect.w;
	}
	if (m_rect.y < 0) {
		m_rect.y = 0;
	} else if (m_rect.y + m_rect.h > displayH) {
		m_rect.y = displayH - m_rect.h;
	}
	resetCBovsPosition();
}

void Player::mov(float dx, float dy) {
	m_rect.x += (int)((dx * (float)displayW) * m_movRatioX);
	m_rect.y += (int)((dy * (float)displayH) * m_movRatioY);
	crossBounds();
	// 重新设置碰撞箱位置
	this->resetBoxXY(0, m_rect.x, m_rect.y + m_rect.h * 0.3);
	resetBoxXY(1, m_rect.x + m_rect.w * 0.3, m_rect.y);
}

void Player::setSwapTime(Uint32 ttime) {
	this->m_swapTime = ttime;
}

PlanePlayerState Player::getState() {
	return this->m_state;
}

void Player::bullet() {
	int index;
	Uint32 nowTick = SDL_GetTicks();
	if (nowTick - m_timeOfLastShoot < m_bulletSpeed) {
		return ;
	}
	m_timeOfLastShoot = nowTick;
	for (index=0; index < m_bulletsSize; index++) {
		if ((m_bullets[index].m_rect.y + m_bullets[index].m_rect.h) > 0) {
			continue;
		}
		break;
	}
	if (index >= m_bulletsSize) {
		m_bulletsSize += 1;
		void *newMemory = SDL_realloc(m_bullets, sizeof(Bullet)*m_bulletsSize);
		if (!newMemory) {
			m_bulletsSize -= 1;
			fprintf(LogFile, "Player: Failed to realloc! %s\n", SDL_GetError());
			PGerrno = -2;
			return;
		}
		m_bullets = (Bullet*)newMemory;
		m_bullets[index] = *m_defaultBulletConfig;
	}
	//m_bullets[index] = *m_defaultBulletConfig;
	m_bullets[index].m_renderer = m_renderer;
	m_bullets[index].m_texture = Bullet::texture1;
	m_bullets[index].m_rect.x = m_rect.x + m_rect.w/2;
	m_bullets[index].m_rect.y = this->m_rect.y + m_bullets[index].m_rect.h;
	m_bullets[index].m_timeOfLastMove = nowTick;
	//m_bullets[index].setSpeed(1, 2);
}

void Player::bulletMovs() {
	for (int i=0; i < m_bulletsSize; i++) {
		if ((m_bullets[i].m_rect.y + m_bullets[i].m_rect.h) > 0) {
			m_bullets[i].mov();
		}
	}
}

void Player::setFiringRate(Uint32 rate) {
	m_bulletSpeed = rate;
}

void Player::resetBoxs() {
	resetCBovsPosition();
	resetBoxWH(0, m_rect.w, m_rect.h * 0.5);
	resetBoxWH(1, m_rect.w * 0.4, m_rect.h * 0.3);
}

void Player::resetCBovsPosition() {
	resetBoxXY(0, m_rect.x, m_rect.y + m_rect.h * 0.3);
	resetBoxXY(1, m_rect.x + m_rect.w * 0.3, m_rect.y);
}
