#include "../inc/font.h"

Font::Font(SDL_Renderer *renderer) : m_renderer(renderer) {
	m_fonts = NULL;
	m_fontSize = 0;
	m_surface = NULL;
	m_texture = NULL;
}

Font::~Font() {
	if (m_texture) {
		SDL_DestroyTexture(m_texture);
	}
	if (!m_fonts) {
		return;
	}
	for (; m_fontSize > 0; ) {
		TTF_CloseFont((*m_fonts)[--m_fontSize]);
	}
	SDL_free(m_fonts);
}

/**
 * ptsize : 字体大小
 * */
int Font::addFont(const char *file, int ptsize) {
	void *newFonts = SDL_realloc(m_fonts, sizeof(TTF_Font*)*(m_fontSize+1));
	if (!newFonts) {
		fprintf(LogFile, "Failed to SDL_realloc fonts! Error: %s\n", TTF_GetError());
		return -1;
	}
	m_fonts = (TTF_Font*(*)[])newFonts;
	(*m_fonts)[m_fontSize] = TTF_OpenFont(file, ptsize);
	return m_fontSize++;
}

/**
 * @biref 渲染文本
 * */
void Font::renderFont(int index, const char *str, SDL_Color fg, int x, int y) {
	static int lastIndex = -1;
	static const char *lastStr = NULL;
	static SDL_Rect rect = {};
	if (index >= m_fontSize) {
		return;
	}
	if (lastIndex != index || lastStr != str) {
		if (m_texture) {
			SDL_DestroyTexture(m_texture);
		}
		m_surface = TTF_RenderUTF8_Blended((*m_fonts)[index], str, fg);
		if (!m_surface) {
			fprintf(LogFile, "TTF render UTF8 Blended Error! Error: %s\n", TTF_GetError());
			PGerrno = -2;
			return ;
		}
		m_texture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
		rect.w = m_surface->w;
		rect.h = m_surface->h;
		SDL_FreeSurface(m_surface);
		m_surface = NULL;
		lastStr = str;
		lastIndex = index;
	}
	rect.x = x;
	rect.y = y;
	SDL_RenderCopy(m_renderer, m_texture, NULL, &rect);
}

void Font::renderFont(int index, std::string &str, SDL_Color fg, int x, int y) {
	if (index >= m_fontSize) {
		return;
	}
	if (m_texture) {
		SDL_DestroyTexture(m_texture);
	}
	SDL_Rect rect = {};
	m_surface = TTF_RenderUTF8_Blended((*m_fonts)[index], str.c_str(), fg);
	m_texture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
	rect.w = m_surface->w;
	rect.h = m_surface->h;
	SDL_FreeSurface(m_surface);
	m_surface = NULL;
	rect.x = x;
	rect.y = y;
	SDL_RenderCopy(m_renderer, m_texture, NULL, &rect);
}

TTF_Font *Font::getFont(int index) {
	if (index >= m_fontSize) {
		return NULL;
	}
	return (*m_fonts)[index];
}

int Font::getFontSize() {
	return m_fontSize;
}
