#ifndef __FONT_H__
#define __FONT_H__

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

extern int PGerrno;
extern FILE *LogFile;

class Font {
public:
	Font(SDL_Renderer *renderer);
	~Font();
	int addFont(const char *file, int ptsize);
	TTF_Font *getFont(int index);
	int getFontSize();
	void renderFont(int index, const char *str, SDL_Color fg, int x, int y);
	void renderFont(int index, std::string &str, SDL_Color fg, int x, int y);
private:
	friend class PlaneGame;
	SDL_Renderer *m_renderer;
	int m_fontSize;
	TTF_Font *(*m_fonts)[];
	SDL_Surface *m_surface;
	SDL_Texture *m_texture;
};

#endif