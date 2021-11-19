#include "../inc/collisionBox.h"

CollisionBox::CollisionBox() {
	m_boxs = NULL;
	m_boxsSize = 0;
}

CollisionBox::~CollisionBox() {
	if (m_boxs) {
		SDL_free(m_boxs);
	}
}

int CollisionBox::detection(CollisionBox *cb) {
	int flag;
	for (int i=0; i < cb->m_boxsSize; i++) {
		flag = detection(cb->m_boxs[i]);
		if (flag) {
			return flag;
		}
	}
	return 0;
}

int CollisionBox::detection(const SDL_Rect *boxs, int boxsSize) {
	int flag;
	for (int i=0; i < boxsSize; i++) {
		flag = detection(boxs[i]);
		if (flag) {
			return flag;
		}
	}
	return 0;
}

#define MIN(i, j) (((i) < (j)) ? (i) : (j))
#define MAX(i, j) (((i) > (j)) ? (i) : (j))

int CollisionBox::detection(const SDL_Rect &rect) {
	int minX, minY;
	int maxX, maxY;
	for (int j=0; j < this->m_boxsSize; j++) {
		minX = MAX(rect.x, this->m_boxs[j].x);
		minY = MAX(rect.y, this->m_boxs[j].y);
		maxX = MIN(rect.x+rect.w, this->m_boxs[j].x+this->m_boxs[j].w);
		maxY = MIN(rect.y+rect.h, this->m_boxs[j].y+this->m_boxs[j].h);
		if (minX < maxX && minY < maxY) {
			return 1;
		}
	}
	return 0;
}

#undef MIN
#undef MAX

void CollisionBox::setBoxs(int size) {
	void *n = SDL_realloc(m_boxs, sizeof(SDL_Rect)*size);
	if (!n) {
		fprintf(LogFile, "CollisionBox: Failed to realloc! Error: %s\n", SDL_GetError());
		PGerrno = -2;
		return ;
	}
	m_boxs = (SDL_Rect*)n;
	m_boxsSize = size;
}

void CollisionBox::resetBoxXY(int index, int x, int y) {
	if (index >= m_boxsSize) {
		return ;
	}
	m_boxs[index].x = x;
	m_boxs[index].y = y;
}

void CollisionBox::resetBoxWH(int index, int w, int h) {
	if (index >= m_boxsSize) {
		return ;
	}
	m_boxs[index].w = w;
	m_boxs[index].h = h;
}

int CollisionBox::getBoxsSize() {
	return this->m_boxsSize;
}

SDL_Rect *CollisionBox::getBoxs() {
	return this->m_boxs;
}