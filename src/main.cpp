#include "../inc/config.h"
#include "../inc/planeGame.h"

#define FPS 100

int displayW = -1, displayH = -1;
FILE *LogFile = NULL;
// 游戏帧
int fpsTick = 1000 / FPS;
// 代替errno，值：小于0则出错了，等于0无错误，大于0警告
int PGerrno = 0;
int GameState = 1;
// 图片缩放比例
float ImageProportion = 1.f;

int main() {
	// 日志文件
	LogFile = fopen(LOG_FILE, "w");
	PlaneGame *game = new PlaneGame();
	fprintf(LogFile, "开始PlaneGame\n");
	fprintf(LogFile, "NULL\n");
	if (game->init() == 0) {
		GameState = 0;
		game->mainLoop();
	}
	GameState = 2;
	fprintf(LogFile, "退出PlaneGame\n");
	delete game;
	fclose(LogFile);
	return 0;
}