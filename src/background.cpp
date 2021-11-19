#include "../inc/background.h"

#include <fcntl.h>
#include <string>
#include <sstream>

Background::Background(SDL_Renderer *renderer) : m_renderer(renderer) {
	m_movTime = 10;
	m_distance = 1;
	m_surface = IMG_LoadPNG_RW(SDL_RWFromFile(BACKGROUND_IMAGE_FILE, "rb"));
	if (!m_surface) {
		fprintf(LogFile, "Cannot load image! Error: %s\n", IMG_GetError());
		PGerrno = -1;
		return;
	}
	m_texture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
	m_imageW = m_surface->w;
	m_imageH = m_surface->h;
	SDL_FreeSurface(m_surface);
	m_surface = NULL;
	m_rect = NULL;
	reSetBg();
}

Background::~Background() {
	if (m_surface) {
		SDL_FreeSurface(m_surface);
	}
	if (m_texture) {
		SDL_DestroyTexture(m_texture);
	}
}

void Background::render() {
	static Uint32 lastT = 0;
	Uint32 startT = SDL_GetTicks();
	int sleep = (float)(startT - lastT) / m_movTime;
	sleep *= m_distance;
	int movT = sleep;
	if (movT) {
		lastT = startT;
	}
	for (int i=0; i < m_rectSize; i++) {
		m_rect[i].y += movT;
		// 超出屏幕
		if (m_rect[i].y > displayH) {
			m_rect[i].y = m_rect[m_firstRect].y + ((m_firstRect < i) ? 0 : movT) - m_rect[i].h;
			m_firstRect = i;
		}
		SDL_RenderCopy(m_renderer, m_texture, NULL, &m_rect[i]);
	}
}

void Background::reSetBg() {
	int imageNumber = 0;
	float rectH = m_imageH * (float)(displayW / m_imageW);
	float fNum = (displayH / rectH);
	imageNumber = (int)fNum;
	if (fNum - (float)imageNumber != 0.f) {
		imageNumber += 2;
	}
	void *newRect = SDL_realloc(m_rect, sizeof(SDL_Rect)*imageNumber);
	if (!newRect) {
		fprintf(LogFile, "Backgroun realloc error! Error: %s\n", SDL_GetError());
		PGerrno = -2;
		return ;
	}
	m_rect = (SDL_Rect*)newRect;
	for (int i=0; i < imageNumber; i++) {
		m_rect[i].x = 0;
		m_rect[i].w = displayW;
		m_rect[i].h = rectH;
		m_rect[i].y = rectH * i;
	}
	m_rectSize = imageNumber;
	m_firstRect = 0;
}

void Background::setBackground(const char *file) {
	
}

void Background::setSleep(int timeT, int distance) {
	m_movTime = timeT;
	m_distance = distance;
}