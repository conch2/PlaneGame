#ifndef __BACKGROUND_H__
#define __BACKGROUND_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define BACKGROUND_IMAGE_FILE "resources/images/background.png"

extern int displayW, displayH;
extern FILE *LogFile;
extern int PGerrno;

class Background {
public:
	Background(SDL_Renderer *renderer);
	~Background();
	void render();
	void reSetBg();
	void setBackground(const char *file);
	void setSleep(int timeT, int distance);
private:
	friend class PlaneGame;
	SDL_Renderer *m_renderer;
	SDL_Surface *m_surface;
	SDL_Texture *m_texture;
	SDL_Rect *m_rect;
	// 当前屏幕上最上张图的Rect索引值
	int m_firstRect;
	int m_rectSize;
	int m_imageW, m_imageH;
	int m_movTime;
	int m_distance;
};

#endif