#include "../inc/supply.h"

Supply::Supply(SDL_Renderer *renderer) : m_renderer(renderer) {
	m_intervalOfSupply = 30E+3;
	m_font = NULL;
	m_renderTable = new std::vector<TRectM>();
	SDL_RWops *ops = SDL_RWFromFile(BOMB_IMAGE, "rb");
	SDL_Surface *surface = IMG_LoadPNG_RW(ops);
	if (surface == NULL) {
		fprintf(LogFile, "Load bomb image Error! %s\n", IMG_GetError());
		PGerrno = -2;
		return ;
	}
	m_bombTexture = SDL_CreateTextureFromSurface(m_renderer, surface);
	m_bombImageW = surface->w;
	m_bombImageH = surface->h;
	m_bombRect.w = m_bombImageW * ImageProportion;
	m_bombRect.h = m_bombImageH * ImageProportion;
	m_bombRect.x = 0;
	m_bombRect.y = displayH - m_bombRect.h;
	SDL_FreeRW(ops);
	SDL_FreeSurface(surface);
	ops = SDL_RWFromFile(BOMB_SUPPLY_IMAGE, "rb");
	surface = IMG_LoadPNG_RW(ops);
	if (surface == NULL) {
		fprintf(LogFile, "Load bomb supply image Error! %s\n", IMG_GetError());
		PGerrno = -2;
		return ;
	}
	m_bombSupplyTexture = SDL_CreateTextureFromSurface(m_renderer, surface);
	m_bombSupplyImageW = surface->w;
	m_bombSupplyImageH = surface->h;
	SDL_FreeRW(ops);
	SDL_FreeSurface(surface);
	ops = SDL_RWFromFile(BULLET_SUPPLY_IMAGE, "rb");
	surface = IMG_LoadPNG_RW(ops);
	if (surface == NULL) {
		fprintf(LogFile, "Load bullet supply image Error! %s\n", IMG_GetError());
		PGerrno = -2;
		return ;
	}
	m_bulletSupplyTexture = SDL_CreateTextureFromSurface(m_renderer, surface);
	m_bulletSupplyImageW = surface->w;
	m_bulletSupplyImageH = surface->h;
	SDL_FreeRW(ops);
	SDL_FreeSurface(surface);
	setNumFSB(-1);
	m_timeOfLastSupply = SDL_GetTicks();
}

Supply::~Supply() {
	if (m_bombTexture) {
		SDL_DestroyTexture(m_bombTexture);
	}
	if (m_bombSupplyTexture) {
		SDL_DestroyTexture(m_bombSupplyTexture);
	}
	if (m_renderTable) {
		delete m_renderTable;
	}
	if (m_numBombTR.t) {
		SDL_DestroyTexture(m_numBombTR.t);
	}
}

void Supply::playing() {
	int i;
	TRectM newTRect;
	Uint32 nowT = SDL_GetTicks();
	for (int i=0; i < m_renderTable->size(); i++) {
		if (nowT - (*m_renderTable)[i].lastMoveT < (*m_renderTable)[i].moveSpeed) {
			continue;
		}
		(*m_renderTable)[i].lastMoveT = nowT;
		(*m_renderTable)[i].r.y += (*m_renderTable)[i].distance;
		if ((*m_renderTable)[i].r.y > displayH) {
			auto vi = m_renderTable->begin();
			vi += i;
			m_renderTable->erase(vi);
		}
	}
	nowT = SDL_GetTicks();
	if (nowT - m_timeOfLastSupply >= m_intervalOfSupply) {
		// 生成新的补给
		std::srand(clock());
		switch (std::rand() % 2) {
		case 0:
			newTRect.t = m_bombSupplyTexture;
			newTRect.r.w = m_bombSupplyImageW * ImageProportion;
			newTRect.r.h = m_bombSupplyImageH * ImageProportion;
			break;
		case 1:
			newTRect.t = m_bulletSupplyTexture;
			newTRect.r.w = m_bulletSupplyImageW * ImageProportion;
			newTRect.r.h = m_bulletSupplyImageH * ImageProportion;
			break;
		}
		newTRect.r.x = std::rand() % (displayW - newTRect.r.w);
		newTRect.r.y = 0 - newTRect.r.h;
		newTRect.moveSpeed = 5;
		newTRect.distance = 1;
		newTRect.lastMoveT = nowT;
		m_renderTable->insert(m_renderTable->begin(), newTRect);
		m_timeOfLastSupply = nowT;
	}
}

void Supply::render() {
	SDL_RenderCopy(m_renderer, m_bombTexture, NULL, &m_bombRect);
	SDL_RenderCopy(m_renderer, m_numBombTR.t, NULL, &m_numBombTR.r);
	for (int i=0; i < m_renderTable->size(); i++) {
		SDL_RenderCopy(m_renderer, (*m_renderTable)[i].t, NULL, &(*m_renderTable)[i].r);
	}
}

void Supply::setNumFSB(int num) {
	if (num == m_numOfFullScreenBomb) {
		return;
	}
	if (m_font == NULL || m_font->getFont(0) == NULL) {
		return;
	}
	if (m_numBombTR.t) {
		SDL_DestroyTexture(m_numBombTR.t);
		m_numBombTR.t = NULL;
	}
	this->m_numOfFullScreenBomb = num;
	std::string str;
	std::stringstream sstr;
	sstr << m_numOfFullScreenBomb;
	sstr >> str;
	SDL_Color fg = {0x0, 0x0, 0x0, 0xFF};
	SDL_Surface *surface = TTF_RenderUTF8_Blended(m_font->getFont(0), str.c_str(), fg);
	if (surface == NULL) {
		fprintf(LogFile, "Font bomb number Error! %s\n", IMG_GetError());
		PGerrno = -2;
		return ;
	}
	m_numBombTR.t = SDL_CreateTextureFromSurface(m_renderer, surface);
	m_numBombTR.r.w = surface->w;
	m_numBombTR.r.h = surface->h;
	m_numBombTR.r.x = m_bombRect.x + m_bombRect.w - m_numBombTR.r.w;
	m_numBombTR.r.y = m_bombRect.y;
	SDL_FreeSurface(surface);
}

int Supply::getNumFSB() {
	return this->m_numOfFullScreenBomb;
}

void Supply::setFont(Font *font) {
	this->m_font = font;
}
