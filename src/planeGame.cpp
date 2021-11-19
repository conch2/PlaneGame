#include "../inc/planeGame.h"

PlaneGame::PlaneGame() : m_window(NULL), m_renderer(NULL) {
	m_font = NULL;
	m_enemy = NULL;
	m_state = START;
	m_player = NULL;
	m_background = NULL;
	m_score = 0;
	m_level = 3;
	m_levelTexture = NULL;
	m_playerIncvDur = 3000;
}

PlaneGame::~PlaneGame() {
	GameState = 2;
	if (m_levelTexture) {
		SDL_DestroyTexture(m_levelTexture);
	}
	if (m_background) {
		delete m_background;
	}
	if (m_font) {
		delete m_font;
	}
	if (m_player) {
		delete m_player;
	}
	Bullet::Quit();
	if (m_renderer) {
		SDL_DestroyRenderer(m_renderer);
	}
	if (m_window) {
		SDL_DestroyWindow(m_window);
	}
	SDL_Quit();
	IMG_Quit();
	TTF_Quit();
}

int PlaneGame::init() {
	// 尽量够用就行
	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_EVENTS
				| SDL_INIT_AUDIO | SDL_INIT_HAPTIC) < 0) {
	//if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		fprintf(LogFile,"Failed to init SDL2! Error: %s\n", SDL_GetError());
		return -1;
	}
	if (TTF_Init() < 0) {
		fprintf(LogFile, "Failed to init TTF! Error: %s\n", TTF_GetError());
		return -1;
	}
	if (IMG_Init(IMG_INIT_PNG | IMG_INIT_PNG) < 0) {
		fprintf(LogFile, "Failed to init IMG! Error: %s\n", IMG_GetError());
		return -1;
	}
	m_window = SDL_CreateWindow("PlaneGame",0,0,1,1,SDL_WINDOW_SHOWN);
	if (!m_window) {
		fprintf(LogFile,"Cannot create window! Error: %s\n",SDL_GetError());
		return -1;
	}
	m_renderer = SDL_CreateRenderer(m_window,-1,SDL_RENDERER_ACCELERATED);
	if (!m_renderer) {
		fprintf(LogFile,"Cannot create renderer! Error: %s\n",SDL_GetError());
		return -1;
	}
	SDL_GetWindowSize(m_window, &displayW, &displayH);
	m_font = new Font(m_renderer);
	Bullet::Init(m_renderer);
	EnemyPlane::Init(m_renderer);
	m_enemy = new EnemyPlane(m_renderer);
	m_background = new Background(m_renderer);
	// 设置背景图片移动速度
	m_background->setSleep(5,1);
	// 计算图片缩放比
	ImageProportion = (displayW / m_background->m_imageW) - 0.5;
	m_font->addFont(MY_FONT_FILE, 32);
	m_font->addFont(MY_FONT_FILE, 64);
	m_player = new Player(m_renderer);
	SDL_SetRenderDrawColor(m_renderer,0xFF,0xFF,0xFF,0xFF);
	m_scoreBuff = "";
	m_playerLastDeathT = -1;
	// 设置生命值相关
	m_level = 3;
	m_playerIncvDur = 2000;
	SDL_RWops *src = SDL_RWFromFile(LEVEL_IMAGE, "rb");
	SDL_Surface *surface = IMG_LoadPNG_RW(src);
	if (surface == NULL) {
		fprintf(LogFile, "Load level image Error! %s\n", SDL_GetError());
		PGerrno = -2;
		return -2;
	}
	m_levelTexture = SDL_CreateTextureFromSurface(m_renderer, surface);
	m_levelRect.w = surface->w * ImageProportion;
	m_levelRect.h = surface->h * ImageProportion;
	m_levelRect.x = displayW - m_levelRect.w;
	m_levelRect.y = displayH - m_levelRect.h;
	SDL_FreeRW(src);
	SDL_FreeSurface(surface);
	m_player->crossBounds();
	// 补给相关
	m_intervalOfSupply = 30E+3;
	m_timeOfLastSupply = SDL_GetTicks();
	m_fullScreenBomb = 0;
	src = SDL_RWFromFile(BOMB_IMAGE, "rb");
	surface = IMG_LoadPNG_RW(src);
	if (surface == NULL) {
		fprintf(LogFile, "Load bomb image Error! %s\n", IMG_GetError());
		PGerrno = -2;
		return -2;
	}
	m_bombTexture = SDL_CreateTextureFromSurface(m_renderer, surface);
	m_bombImageW = surface->w;
	m_bombImageH = surface->h;
	m_bombRect.w = m_bombImageW * ImageProportion;
	m_bombRect.h = m_bombImageH * ImageProportion;
	m_bombRect.x = 0;
	m_bombRect.y = displayH - m_bombRect.h;
	SDL_FreeRW(src);
	SDL_FreeSurface(surface);
	src = SDL_RWFromFile(BOMB_SUPPLY_IMAGE, "rb");
	surface = IMG_LoadPNG_RW(src);
	if (surface == NULL) {
		fprintf(LogFile, "Load bomb supply image Error! %s\n", IMG_GetError());
		PGerrno = -2;
		return -2;
	}
	m_bombSupplyTexture = SDL_CreateTextureFromSurface(m_renderer, surface);
	m_bombSupplyImageW = surface->w;
	m_bombSupplyImageH = surface->h;
	m_bombSupplyRect.w = m_bombSupplyImageW * ImageProportion;
	m_bombSupplyRect.h = m_bombSupplyImageH * ImageProportion;
	SDL_FreeRW(src);
	SDL_FreeSurface(surface);
	return PGerrno;
}

void PlaneGame::mainLoop() {
	SDL_Event event;
	
	while (PGerrno >= 0) {
	//while (true) {
		// Event
		while (SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_QUIT:
				return ;
			case SDL_WINDOWEVENT:
				int dw, dh;
				SDL_GetWindowSize(m_window, &dw, &dh);
				if (dw != displayW || dh != displayH) {
					displayW = dw;
					displayH = dh;
					m_background->reSetBg();
					m_player->crossBounds();
					m_levelRect.x = displayW - m_levelRect.w;
					m_levelRect.y = displayH - m_levelRect.h;
				}
				break;
			case SDL_FINGERDOWN:
				if (m_state == START) {
					m_state = PLAYING;
					m_enemy->m_lastCreateEnemy1 = SDL_GetTicks();
				} else if (m_state == PGEND) {
					if (m_player->m_state != PLAYER_DEATH) {
						break;
					}
					rePlay();
				}
				break;
			case SDL_FINGERMOTION:
				if (m_state != PGEND || m_player->m_state == PLAYER_LIVING) {
					m_player->mov(event.tfinger.dx, event.tfinger.dy);
				}
				break;
			}
		}
		// 绘图
		render();
		if (m_player->m_state == PLAYER_INVINCIBLE && SDL_GetTicks() - m_playerLastDeathT >= m_playerIncvDur) {
			m_player->m_state = PLAYER_LIVING;
			m_player->m_textureIndex = 0;
		}
		m_player->onLoop();
		if (m_state == PLAYING) {
			// 玩家发射子弹
			m_player->bullet();
			// 子弹移动
			m_player->bulletMovs();
			// 生成敌机
			m_enemy->playing();
			// 碰撞检测
			scanCollision();
			// 补给
			supply();
		}
	}
}

void PlaneGame::render() {
	static Uint32 lastFpsT = 0;
	Uint32 nowT = SDL_GetTicks();
	std::stringstream buffStream;
	// 限制FPS
	if (nowT - lastFpsT < fpsTick) {
		return ;
	}
	lastFpsT = nowT;
	SDL_Color fontColor = {0x00, 0x0F, 0x0F, 0xFF};
	//SDL_SetRenderDrawColor(m_renderer,0xFF,0xFF,0xFF,0xFF);
	SDL_RenderClear(m_renderer);
	
	m_background->render();
	
	switch (m_state) {
	case PGEND:
		if (m_player->m_state == PLAYER_DYING) {
			m_enemy->render();
			m_player->render();
		} else {
			buffStream << "游戏结束";
			buffStream >> m_scoreBuff;
			m_font->renderFont(1, m_scoreBuff, fontColor, 230, 1100);
		}
		break;
	case START:
		buffStream << "点击任意位置开始游戏";
		buffStream >> m_scoreBuff;
		m_font->renderFont(1, m_scoreBuff, fontColor, 230, 1100);
		break;
	case PLAYING:
		renderLevel();
		renderBomb();
		renderSupply();
		m_enemy->render();
		m_player->render();
		buffStream << "分数：" << m_score;
		buffStream >> m_scoreBuff;
		m_font->renderFont(0, m_scoreBuff, {0x0, 0x0, 0x0, 0xFF}, 0, 0);
		break;
	}
	// Debug
	//showCollisionBoxs();
	SDL_RenderPresent(m_renderer);
}

void PlaneGame::renderLevel() {
	// 文字
	SDL_Color fg = {0x0, 0x0, 0x0, 0xFF};
	SDL_Rect fontRect;
	SDL_Surface *fontSurface;
	std::stringstream strs;
	std::string str = "";
	strs << "x" << m_level;
	strs >> str;
	fontSurface = TTF_RenderUTF8_Blended((*m_font->m_fonts)[1], str.c_str(), fg);
	if (!fontSurface) {
		fprintf(LogFile, "Cannot create surface! %s\n", TTF_GetError());
		PGerrno = -2;
		return ;
	}
	fontRect.w = fontSurface->w;
	fontRect.h = fontSurface->h;
	SDL_Texture *fontTexture = SDL_CreateTextureFromSurface(m_renderer, fontSurface);
	SDL_FreeSurface(fontSurface);
	fontRect.x = displayW - fontRect.w;
	fontRect.y = displayH - fontRect.h;
	SDL_RenderCopy(m_renderer, fontTexture, NULL, &fontRect);
	SDL_DestroyTexture(fontTexture);
	// 图片
	m_levelRect.x = fontRect.x - m_levelRect.w;
	SDL_RenderCopy(m_renderer, m_levelTexture, NULL, &m_levelRect);
}

void PlaneGame::renderBomb() {
	SDL_RenderCopy(m_renderer, m_bombTexture, NULL, &m_bombRect);
}

void PlaneGame::renderSupply() {
	if (m_bombSupplyRect.y >= displayH) {
		return;
	}
	SDL_RenderCopy(m_renderer, m_bombSupplyTexture, NULL, &m_bombSupplyRect);
	m_bombSupplyRect.y += 1;
}

void PlaneGame::scanCollision() {
	// 玩家与敌机检查
	if (m_enemy->playerCollision(*m_player)) {
		switch (m_player->m_state) {
		case PLAYER_LIVING:
			if (!m_level) {
				m_player->m_state = PLAYER_DYING;
				m_state = PGEND;
			} else {
				m_level--;
				m_playerLastDeathT = SDL_GetTicks();
				m_player->m_state = PLAYER_INVINCIBLE;
			}
			break;
		case PLAYER_DYING:
			break;
		case PLAYER_INVINCIBLE:
			break;
		case PLAYER_DEATH:
			break;
		}
	}
	// 子弹与敌机检查
	for (int i=0; i < m_player->m_bulletsSize; i++) {
		Bullet &bullet = m_player->m_bullets[i];
		int &rectY = bullet.m_rect.y;
		const int &rectH = bullet.m_rect.h;
		// 超出屏幕，不进行判断
		if (rectY + rectH <= 0) {
			continue;
		}
		for (int j=0; j < m_enemy->m_enemy1Size; j++) {
			Enemy1 &enemy1 = m_enemy->m_enemy1[j];
			if (enemy1.m_state != ENEMY_ALIVE) {
				continue;
			}
			if (enemy1.detection(bullet.m_rect)) {
				rectY = 0 - rectH;
				enemy1.m_state = ENEMY_IN_DEATH;
				enemy1.m_timeOfDeath = SDL_GetTicks();
				m_score++;
				break;
			}
		}
		for (int j=0; j < m_enemy->m_midEnemySize; j++) {
			MidEnemy &midEnemy = m_enemy->m_midEnemy[j];
			if (midEnemy.m_state == ENEMY_IN_DEATH || midEnemy.m_state == ENEMY_DEATH) {
				continue;
			}
			if (midEnemy.detection(bullet.m_rect)) {
				// 移除子弹
				rectY = 0 - rectH;
				midEnemy.m_bloodVolume--;
				if (midEnemy.m_bloodVolume) {
					// 血量不空
					// 设置击中动画
					midEnemy.m_whenWasItShot = SDL_GetTicks();
					midEnemy.m_state = ENEMY_SHOT;
					midEnemy.m_texture = MidEnemy::Textures[1];
					break;
				}
				midEnemy.m_state = ENEMY_IN_DEATH;
				midEnemy.m_timeOfDeath = SDL_GetTicks();
				m_score += 10;
				break;
			}
		}
	}
}

void PlaneGame::supply() {
	Uint32 nowT = SDL_GetTicks();
	if (nowT - m_timeOfLastSupply < m_intervalOfSupply) {
		return ;
	}
	m_timeOfLastSupply = nowT;
	std::srand(time(NULL));
	m_bombSupplyRect.x = std::rand() % (displayW - m_bombSupplyRect.w);
	m_bombSupplyRect.y = 0 - m_bombSupplyRect.h;
}

void PlaneGame::rePlay() {
	m_state = START;
	m_score = 0;
	m_enemy->rePlay();
	m_player->m_rect.x = (displayW - m_player->m_rect.w) / 2;
	m_player->m_rect.y = displayH - m_player->m_rect.h;
	m_level = 3;
	m_player->m_state = PLAYER_LIVING;
	m_player->setFiringRate(PLAYER_DEFAULT_FIRING_RATE);
	m_player->resetBoxs();
}

void PlaneGame::showCollisionBoxs() {
	SDL_SetRenderDrawColor(m_renderer, 0x0, 0xF0, 0x0, 0xFF);
	for (int i=0; i < m_player->m_boxsSize; i++) {
		SDL_RenderDrawRect(m_renderer, &m_player->m_boxs[i]);
	}
	for (int i=0; i < m_enemy->m_enemy1Size; i++) {
		if (m_enemy->m_enemy1[i].m_state == ENEMY_DEATH) continue;
		for (int j=0; j < m_enemy->m_enemy1[i].m_boxsSize; j++) {
			SDL_RenderDrawRect(m_renderer, &m_enemy->m_enemy1[i].m_boxs[j]);
		}
	}
	for (int i=0; i < m_enemy->m_midEnemySize; i++) {
		if (m_enemy->m_midEnemy[i].m_state == ENEMY_DEATH) continue;
		for (int j=0; j < m_enemy->m_midEnemy[i].m_boxsSize; j++) {
			SDL_RenderDrawRect(m_renderer, &m_enemy->m_midEnemy[i].m_boxs[j]);
		}
	}
}