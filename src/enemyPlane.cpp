#include "../inc/enemyPlane.h"
#include <fcntl.h>
#include <string>
#include <cstdlib>
#include <ctime>

const char *Enemy1::images[] = {
	ROOT_IMAGES"/enemy1.png",
	ROOT_IMAGES"/enemy1_down1.png",
	ROOT_IMAGES"/enemy1_down2.png",
	ROOT_IMAGES"/enemy1_down3.png",
	ROOT_IMAGES"/enemy1_down4.png"
};
int Enemy1::imagesSize = sizeof(Enemy1::images) / sizeof(char*);
SDL_Texture **Enemy1::Textures = NULL;
SDL_Renderer *Enemy1::Renderer = NULL;
int Enemy1::imageW = -1;
int Enemy1::imageH = -1;
int Enemy1::numOfExample = 0;
Uint32 Enemy1::MoveSpeed = 4;
int Enemy1::Distance = 1;

const char *MidEnemy::images[] = {
	ROOT_IMAGES"/enemy2.png",
	ROOT_IMAGES"/enemy2_hit.png",
	ROOT_IMAGES"/enemy2_down1.png",
	ROOT_IMAGES"/enemy2_down2.png",
	ROOT_IMAGES"/enemy2_down3.png",
	ROOT_IMAGES"/enemy2_down4.png"
};
int MidEnemy::imagesSize = sizeof(MidEnemy::images) / sizeof(char*);
SDL_Texture **MidEnemy::Textures = NULL;
SDL_Renderer *MidEnemy::Renderer = NULL;
int MidEnemy::imageW = -1;
int MidEnemy::imageH = -1;
Uint32 MidEnemy::MoveSpeed = 6;
int MidEnemy::Distance = 1;
int MidEnemy::NewPlaneBloodVolume = 8;
Uint32 MidEnemy::DOAwhenShot = 150;

const char *BigEnemy::images[] = {
	ROOT_IMAGES"/enemy3_n1.png",
	ROOT_IMAGES"/enemy3_n2.png",
	ROOT_IMAGES"/enemy3_hit.png",
	ROOT_IMAGES"/enemy3_down1.png",
	ROOT_IMAGES"/enemy3_down2.png",
	ROOT_IMAGES"/enemy3_down3.png",
	ROOT_IMAGES"/enemy3_down4.png",
	ROOT_IMAGES"/enemy3_down5.png",
	ROOT_IMAGES"/enemy3_down6.png"
};
int BigEnemy::imagesSize = sizeof(BigEnemy::images) / sizeof(char*);
SDL_Texture **BigEnemy::Textures = NULL;
SDL_Renderer *BigEnemy::Renderer = NULL;
int BigEnemy::imageW = -1;
int BigEnemy::imageH = -1;
Uint32 BigEnemy::MoveSpeed = 8;
int BigEnemy::Distance = 1;
int BigEnemy::NewPlaneBloodVolume = 24;
Uint32 BigEnemy::DOAwhenShot = 150;
Uint32 BigEnemy::SwapTime = 150;

EnemyPlane::EnemyPlane(SDL_Renderer *renderer) {
	m_enemy1Size = 0;
	m_enemy1 = NULL;
	m_midEnemy = NULL;
	m_midEnemySize = 0;
	m_bigEnemy = NULL;
	m_bigEnemySize = 0;
	m_lastCreateEnemy1 = SDL_GetTicks();
	m_lastCreateMidEnemy = SDL_GetTicks();
	m_lastCreateBigEnemy = SDL_GetTicks();
	m_intervalOfNewEnemy1 = 300;
	m_intervalOfNewMidEnemy = 5E+3;
	m_intervalOfNewBigEnemy = 10000;
}

EnemyPlane::~EnemyPlane() {
	if (m_enemy1) {
		SDL_free(m_enemy1);
	}
	if (m_midEnemy) {
		SDL_free(m_midEnemy);
	}
	if (m_bigEnemy) {
		SDL_free(m_bigEnemy);
	}
}

void EnemyPlane::Init(SDL_Renderer *renderer) {
	Enemy1::Init(renderer);
	MidEnemy::Init(renderer);
	BigEnemy::Init(renderer);
}

void EnemyPlane::Quit() {
	Enemy1::Quit();
	MidEnemy::Quit();
	BigEnemy::Quit();
}

void EnemyPlane::playing() {
	int tr;
	Uint32 u;
	Uint32 nowT = SDL_GetTicks();
	u = nowT - m_lastCreateEnemy1;
	u /= m_intervalOfNewEnemy1;
	//m_lastCreateEnemy += (m_intervalOfNewEnemy * u);
	//m_lastCreateEnemy = nowT;
	for (tr = 0; tr < m_enemy1Size; tr++) {
		m_enemy1[tr].m_boxs = &m_enemy1[tr].m_rect;
		if (m_enemy1[tr].m_state == ENEMY_ALIVE) {
			// 移动
			nowT = SDL_GetTicks();
			if (nowT - m_enemy1[tr].m_lastMoveTime > m_enemy1[tr].m_moveSpeed) {
				m_enemy1[tr].m_rect.y += m_enemy1[tr].m_distance * ((nowT - m_enemy1[tr].m_lastMoveTime) / m_enemy1[tr].m_moveSpeed);
				if (m_enemy1[tr].m_rect.y > displayH) {
					m_enemy1[tr].m_state = ENEMY_DEATH;
				}
				m_enemy1[tr].m_lastMoveTime = nowT;
			}
		}
		else if (m_enemy1[tr].m_state == ENEMY_IN_DEATH) {
			int num = (SDL_GetTicks() - m_enemy1[tr].m_timeOfDeath) / m_enemy1[tr].m_deathAnimationDuration;
			num++;
			if (num >= Enemy1::imagesSize) {
				m_enemy1[tr].m_state = ENEMY_DEATH;
			} else 
				m_enemy1[tr].m_texture = Enemy1::Textures[num];
		}
		else if (u && m_enemy1[tr].m_state == ENEMY_DEATH) {
			u = 0;
			m_lastCreateEnemy1 = SDL_GetTicks();
			// CPU时间
			std::srand(clock());
			//std::srand(std::time(NULL));
			m_enemy1[tr].m_texture = Enemy1::Textures[0];
			// 随机生成
			m_enemy1[tr].m_rect.x = std::rand() % (displayW - m_enemy1[tr].m_rect.w);
			m_enemy1[tr].m_rect.y = 0 - Enemy1::imageH;
			m_enemy1[tr].m_lastMoveTime = nowT;
			m_enemy1[tr].m_state = ENEMY_ALIVE;
			m_enemy1[tr].m_moveSpeed = 2 + std::rand() % 3;
			//m_enemy1[tr].m_moveSpeed = Enemy1::MoveSpeed;
			m_enemy1[tr].m_distance = Enemy1::Distance;
		}
	}
	if (u && tr == m_enemy1Size) {
		m_enemy1Size += u;
		void *newE = SDL_realloc(m_enemy1, sizeof(Enemy1)*m_enemy1Size);
		if (!newE) {
			fprintf(LogFile, "EnemyPlane Cannot new object!\n");
			write(LogFile->_file, "h\n", 2);
			PGerrno = -2;
			return;
		}
		m_enemy1 = (Enemy1*)newE;
		new (&m_enemy1[tr])Enemy1();
	}
	nowT = SDL_GetTicks();
	u = nowT - m_lastCreateMidEnemy;
	u /= m_intervalOfNewMidEnemy;
	for (tr=0; tr < m_midEnemySize; tr++) {
		//m_midEnemy[tr].m_boxs = &m_midEnemy[tr].m_rect;
		if (m_midEnemy[tr].m_state == ENEMY_ALIVE) {
			m_midEnemy[tr].move();
		}
		else if (m_midEnemy[tr].m_state == ENEMY_IN_DEATH) {
			int num = (SDL_GetTicks() - m_midEnemy[tr].m_timeOfDeath) / m_midEnemy[tr].m_deathAnimationDuration;
			num += 3;
			if (num >= MidEnemy::imagesSize) {
				m_midEnemy[tr].m_state = ENEMY_DEATH;
			} else 
				m_midEnemy[tr].m_texture = MidEnemy::Textures[num];
		}
		else if (m_midEnemy[tr].m_state == ENEMY_SHOT) {
			// 被射中
			m_midEnemy[tr].move();
			if (SDL_GetTicks() - m_midEnemy[tr].m_whenWasItShot >= MidEnemy::DOAwhenShot) {
				m_midEnemy[tr].m_state = ENEMY_ALIVE;
				m_midEnemy[tr].m_texture = MidEnemy::Textures[0];
			}
		}
		else if (u && m_midEnemy[tr].m_state == ENEMY_DEATH) {
			u = 0;
			m_lastCreateMidEnemy = SDL_GetTicks();
			// CPU时间
			std::srand(clock());
			m_midEnemy[tr].m_texture = MidEnemy::Textures[0];
			m_midEnemy[tr].m_rect.x = std::rand() % (displayW - m_midEnemy[tr].m_rect.w);
			m_midEnemy[tr].m_rect.y = 0 - m_midEnemy[tr].m_rect.h;
			m_midEnemy[tr].m_lastMoveTime = nowT;
			m_midEnemy[tr].m_state = ENEMY_ALIVE;
			m_midEnemy[tr].m_moveSpeed = 3 + std::rand() % 4;
			//m_midEnemy[tr].m_moveSpeed = MidEnemy::MoveSpeed;
			m_midEnemy[tr].m_distance = MidEnemy::Distance;
			// 设置血量
			m_midEnemy[tr].m_bloodVolume = MidEnemy::NewPlaneBloodVolume;
			m_intervalOfNewMidEnemy = 100 + std::rand() % 3000;
		}
	}
	if (u && tr == m_midEnemySize) {
		m_midEnemySize += u;
		void *newE = SDL_realloc(m_midEnemy, sizeof(MidEnemy)*m_midEnemySize);
		if (!newE) {
			fprintf(LogFile, "EnemyPlane Cannot new object!\n");
			write(LogFile->_file, "h\n", 2);
			PGerrno = -2;
			return;
		}
		m_midEnemy = (MidEnemy*)newE;
		new (&m_midEnemy[tr])MidEnemy();
	}
	nowT = SDL_GetTicks();
	u = nowT - m_lastCreateBigEnemy;
	u /= m_intervalOfNewBigEnemy;
	int num;
	for (tr=0; tr < m_bigEnemySize; tr++) {
		switch (m_bigEnemy[tr].m_state) {
		case ENEMY_ALIVE:
			m_bigEnemy[tr].move();
			m_bigEnemy[tr].swapImage();
			break;
		case ENEMY_SHOT:
			m_bigEnemy[tr].move();
			if (BigEnemy::DOAwhenShot + m_bigEnemy[tr].m_whenWasItShot <= SDL_GetTicks()) {
				m_bigEnemy[tr].m_state = ENEMY_ALIVE;
				m_bigEnemy[tr].m_texture = BigEnemy::Textures[0];
			}
			break;
		case ENEMY_IN_DEATH:
			num = (SDL_GetTicks() - m_bigEnemy[tr].m_timeOfDeath) / m_bigEnemy[tr].m_deathAnimationDuration;
			num += 2;
			if (num >= BigEnemy::imagesSize) {
				m_bigEnemy[tr].m_state = ENEMY_DEATH;
			} else {
				m_bigEnemy[tr].m_texture = BigEnemy::Textures[num];
			}
			break;
		case ENEMY_DEATH:
			if (u) {
				u = 0;
				nowT = SDL_GetTicks();
				m_lastCreateBigEnemy = nowT;
				m_bigEnemy[tr].m_lastMoveTime = nowT;
				std::srand(clock());
				m_bigEnemy[tr].m_texture = BigEnemy::Textures[0];
				m_bigEnemy[tr].m_rect.x = std::rand() % (displayW - m_bigEnemy[tr].m_rect.w);
				m_bigEnemy[tr].m_rect.y = 0 - m_bigEnemy[tr].m_rect.h;
				m_bigEnemy[tr].m_state = ENEMY_ALIVE;
				m_bigEnemy[tr].m_moveSpeed = BigEnemy::MoveSpeed;
				m_bigEnemy[tr].m_distance = BigEnemy::Distance;
				m_bigEnemy[tr].m_bloodVolume = BigEnemy::NewPlaneBloodVolume;
			}
			break;
		}
	}
	if (u && tr == m_bigEnemySize) {
		m_bigEnemySize += u;
		void *newE = SDL_realloc(m_bigEnemy, sizeof(BigEnemy)*m_bigEnemySize);
		if (!newE) {
			fprintf(LogFile, "EnemyPlane Cannot new object!\n");
			write(LogFile->_file, "h\n", 2);
			PGerrno = -2;
			return;
		}
		m_bigEnemy = (BigEnemy*)newE;
		new (&m_bigEnemy[tr])BigEnemy();
	}
}

void EnemyPlane::render() {
	int i;
	for (i=0; i < m_bigEnemySize; i++) {
		m_bigEnemy[i].render();
	}
	for (i=0; i < m_midEnemySize; i++) {
		m_midEnemy[i].render();
	}
	for (i=0; i < m_enemy1Size; i++) {
		m_enemy1[i].render();
	}
}

void EnemyPlane::rePlay() {
	m_lastCreateEnemy1 = SDL_GetTicks();
	m_lastCreateMidEnemy = SDL_GetTicks();
	m_lastCreateBigEnemy = SDL_GetTicks();
	m_intervalOfNewEnemy1 = 300;
	if (m_enemy1) SDL_free(m_enemy1);
	if (m_midEnemy) SDL_free(m_midEnemy);
	if (m_bigEnemy) SDL_free(m_bigEnemy);
	m_enemy1 = NULL;
	m_midEnemy = NULL;
	m_bigEnemy = NULL;
	m_enemy1Size = 0;
	m_midEnemySize = 0;
	m_bigEnemySize = 0;
}

int EnemyPlane::playerCollision(Player &player) {
	SDL_Rect *playerBoxs = player.getBoxs();
	int playerBoxsSize = player.getBoxsSize();
	for (int i=0; i < m_enemy1Size; i++) {
		if (m_enemy1[i].m_state != ENEMY_ALIVE) {
			continue;
		}
		if (m_enemy1[i].detection(playerBoxs, playerBoxsSize)) {
			return 1;
		}
	}
	for (int i=0; i < m_midEnemySize; i++) {
		if (m_midEnemy[i].m_state == ENEMY_ALIVE || m_midEnemy[i].m_state == ENEMY_SHOT) {
			if (m_midEnemy[i].detection(playerBoxs, playerBoxsSize)) {
				return 1;
			}
		}
	}
	for (int i=0; i < m_bigEnemySize; i++) {
		if (m_bigEnemy[i].m_state == ENEMY_ALIVE || m_bigEnemy[i].m_state == ENEMY_SHOT) {
			if (m_bigEnemy[i].detection(playerBoxs, playerBoxsSize)) {
				return 1;
			}
		}
	}
	return 0;
}

Enemy1::Enemy1() {
	numOfExample++;
	if (!Textures) {
		fprintf(LogFile, "Enemy1 not init!\n");
		PGerrno = -3;
		return;
	}
	m_texture = Textures[0];
	m_rect.w = imageW * ImageProportion;
	m_rect.h = imageH * ImageProportion;
	m_boxs = &m_rect;
	m_boxsSize = 1;
	m_moveSpeed = MoveSpeed;
	m_distance = Distance;
	m_deathAnimationDuration = 160;
	m_state = ENEMY_DEATH;
}

Enemy1::Enemy1(const Enemy1 &obj) {
	numOfExample++;
	m_texture = obj.m_texture;
	this->m_rect = obj.m_rect;
	m_boxs = &this->m_rect;
	m_boxsSize = obj.m_boxsSize;
	m_moveSpeed = obj.m_moveSpeed;
	m_distance = obj.m_distance;
	m_deathAnimationDuration = obj.m_deathAnimationDuration;
}

Enemy1::Enemy1(SDL_Texture *texture, SDL_Rect rect, Uint32 lastMoveTime
				, Uint32 moveSpeed, int distance, SDL_Rect *boxs, int boxsSize) {
	m_texture = texture;
	m_rect = rect;
	m_lastMoveTime = lastMoveTime;
	m_moveSpeed = moveSpeed;
	m_distance = distance;
	m_boxs = boxs;
	m_boxsSize = boxsSize;
}

Enemy1::~Enemy1() {
	numOfExample--;
}

void Enemy1::Init(SDL_Renderer *renderer) {
	Renderer = renderer;
	Textures = (SDL_Texture**)SDL_malloc(sizeof(SDL_Texture*)*imagesSize);
	if (!Textures) {
		fprintf(LogFile, "Enemy1 malloc Error! %s\n", SDL_GetError());
		PGerrno = -2;
		return;
	}
	SDL_memset(Textures, 0, sizeof(SDL_Texture*)*imagesSize);
	for (int i=0; i < imagesSize; i++) {
		Textures[i] = IMG_LoadTexture(Renderer, images[i]);
		if (!Textures[i]) {
			fprintf(LogFile, "Enemy1 load image Error! %s\n", IMG_GetError());
			PGerrno = -2;
			return ;
		}
	}
	SDL_QueryTexture(Textures[0], NULL, NULL, &imageW, &imageH);
}

void Enemy1::Quit() {
	if (!Textures) {
		for (int i=0; i < imagesSize; i++) {
			SDL_DestroyTexture(Textures[i]);
		}
		SDL_free(Textures);
	}
}

void Enemy1::render() {
	if (m_state == ENEMY_DEATH) {
		return ;
	}
	SDL_RenderCopy(Renderer, m_texture, NULL, &m_rect);
}

void Enemy1::setSpeed(Uint32 moveSpeed, int distance) {
	m_moveSpeed = moveSpeed;
	m_distance = distance;
}

MidEnemy::MidEnemy() {
	m_state = ENEMY_DEATH;
	m_deathAnimationDuration = 200;
	m_rect.w = imageW * ImageProportion;
	m_rect.h = imageH * ImageProportion;
	m_boxs = &m_rect;
	m_boxsSize = 1;
	m_texture = Textures[0];
}

MidEnemy::~MidEnemy() {
	
}

void MidEnemy::Init(SDL_Renderer *renderer) {
	Renderer = renderer;
	Textures = (SDL_Texture**)SDL_malloc(sizeof(SDL_Texture*)*imagesSize);
	if (!Textures) {
		fprintf(LogFile, "MidEnemy malloc Error! %s\n", SDL_GetError());
		PGerrno = -2;
		return;
	}
	SDL_memset(Textures, 0, sizeof(SDL_Texture*)*imagesSize);
	for (int i=0; i < imagesSize; i++) {
		Textures[i] = IMG_LoadTexture(Renderer, images[i]);
		if (!Textures[i]) {
			fprintf(LogFile, "MidEnemy load image Error! %s\n", IMG_GetError());
			PGerrno = -2;
			return ;
		}
	}
	SDL_QueryTexture(Textures[0], NULL, NULL, &imageW, &imageH);
}

void MidEnemy::Quit() {
	if (!Textures) {
		for (int i=0; i < imagesSize; i++) {
			SDL_DestroyTexture(Textures[i]);
		}
		SDL_free(Textures);
	}
}

void MidEnemy::render() {
	if (m_state == ENEMY_DEATH) {
		return ;
	}
	SDL_RenderCopy(Renderer, m_texture, NULL, &m_rect);
	if (m_state == ENEMY_IN_DEATH) {
		return;
	}
	// 绘制血量
	SDL_Rect blood;
	float bloodVolumeF = (float)m_bloodVolume / NewPlaneBloodVolume;
	blood.h = m_rect.h * 0.03;
	blood.x = m_rect.x;
	blood.y = m_rect.y - blood.h;
	blood.w = m_rect.w * bloodVolumeF;
	if (bloodVolumeF < 0.3) {
		SDL_SetRenderDrawColor(Renderer, 0xFF, 0x0, 0x0, 0xFF);
	} else {
		SDL_SetRenderDrawColor(Renderer, 0x0, 0xFF, 0x0, 0xFF);
	}
	SDL_RenderFillRect(Renderer, &blood);
	blood.w = m_rect.w;
	SDL_SetRenderDrawColor(Renderer, 0, 0x0, 0x0, 0xFF);
	SDL_RenderDrawRect(Renderer, &blood);
}

void MidEnemy::move() {
	Uint32 nowT = SDL_GetTicks();
	if (nowT - m_lastMoveTime < m_moveSpeed) {
		return;
	}
	m_rect.y += m_distance * ((nowT - m_lastMoveTime) / m_moveSpeed);
	if (m_rect.y > displayH) {
		m_state = ENEMY_DEATH;
	}
	m_lastMoveTime = nowT;
	m_boxs = &m_rect;
}

BigEnemy::BigEnemy() {
	m_state = ENEMY_DEATH;
	m_deathAnimationDuration = 200;
	m_rect.w = imageW * ImageProportion;
	m_rect.h = imageH * ImageProportion;
	m_boxs = &m_rect;
	m_boxsSize = 1;
	m_texture = Textures[0];
	m_lastSwapTime = SDL_GetTicks();
	m_bloodVolume = NewPlaneBloodVolume;
}

BigEnemy::~BigEnemy() {
	
}

void BigEnemy::Init(SDL_Renderer *renderer) {
	Renderer = renderer;
	Textures = (SDL_Texture**)SDL_malloc(sizeof(SDL_Texture*)*imagesSize);
	if (!Textures) {
		fprintf(LogFile, "BigEnemy malloc Error! %s\n", SDL_GetError());
		PGerrno = -2;
		return;
	}
	SDL_memset(Textures, 0, sizeof(SDL_Texture*)*imagesSize);
	for (int i=0; i < imagesSize; i++) {
		Textures[i] = IMG_LoadTexture(Renderer, images[i]);
		if (!Textures[i]) {
			fprintf(LogFile, "BigEnemy load image Error! %s\n", IMG_GetError());
			PGerrno = -2;
			return ;
		}
	}
	SDL_QueryTexture(Textures[0], NULL, NULL, &imageW, &imageH);
}

void BigEnemy::Quit() {
	if (!Textures) {
		for (int i=0; i < imagesSize; i++) {
			SDL_DestroyTexture(Textures[i]);
		}
		SDL_free(Textures);
		Textures = NULL;
	}
}

void BigEnemy::render() {
	if (m_state == ENEMY_DEATH) {
		return;
	}
	SDL_RenderCopy(Renderer, m_texture, NULL, &m_rect);
	if (m_state == ENEMY_IN_DEATH) {
		return;
	}
	// 绘制血量
	SDL_Rect blood;
	float bloodVolumeF = (float)m_bloodVolume / NewPlaneBloodVolume;
	blood.h = m_rect.h * 0.03;
	blood.x = m_rect.x;
	blood.y = m_rect.y - blood.h;
	blood.w = m_rect.w * bloodVolumeF;
	if (bloodVolumeF < 0.3) {
		SDL_SetRenderDrawColor(Renderer, 0xFF, 0x0, 0x0, 0xFF);
	} else {
		SDL_SetRenderDrawColor(Renderer, 0x0, 0xFF, 0x0, 0xFF);
	}
	SDL_RenderFillRect(Renderer, &blood);
	blood.w = m_rect.w;
	SDL_SetRenderDrawColor(Renderer, 0, 0x0, 0x0, 0xFF);
	SDL_RenderDrawRect(Renderer, &blood);
}

void BigEnemy::move() {
	Uint32 nowT = SDL_GetTicks();
	if (nowT - m_lastMoveTime < m_moveSpeed) {
		return;
	}
	m_rect.y += m_distance * ((nowT - m_lastMoveTime) / m_moveSpeed);
	if (m_rect.y > displayH) {
		m_state = ENEMY_DEATH;
	}
	m_lastMoveTime = nowT;
	m_boxs = &m_rect;
}

void BigEnemy::swapImage() {
	Uint32 nowT = SDL_GetTicks();
	if (m_lastSwapTime + SwapTime <= nowT) {
		m_lastSwapTime = nowT;
		if (m_state == ENEMY_ALIVE) {
			m_texture = (m_texture == BigEnemy::Textures[0]) ? BigEnemy::Textures[1] : BigEnemy::Textures[0];
		}
	}
}