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

int CollisionBox::detection(const SDL_Rect *boxs1, const SDL_Rect *boxs2, int boxsSize1, int boxsSize2) {
	int minX, minY;
	int maxX, maxY;
	for (int i=0; i < boxsSize1; i++) {
		for (int j=0; j < boxsSize2; j++) {
			minX = MAX(boxs1[i].x, boxs2[j].x);
			minY = MAX(boxs1[i].y, boxs2[j].y);
			maxX = MAX(boxs1[i].x+boxs1[i].w, boxs2[j].x+boxs2[j].w);
			maxY = MAX(boxs1[i].y+boxs1[i].h, boxs2[j].y+boxs2[j].h);
			if (minX < maxX && minY < maxY) {
				return 1;
			}
		}
	}
	return 0;
}

int CollisionBox::detection(const SDL_Rect &rect1, const SDL_Rect &rect2) {
	int minX, minY;
	int maxX, maxY;
	minX = MAX(rect1.x, rect2.x);
	minY = MAX(rect1.y, rect2.y);
	maxX = MIN(rect1.x+rect1.w, rect2.x+rect2.w);
	maxY = MIN(rect1.y+rect1.h, rect2.y+rect2.h);
	if (minX < maxX && minY < maxY) {
		return 1;
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