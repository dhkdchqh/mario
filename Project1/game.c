
#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include <windows.h>
#pragma warning(disable : 4996)

#define map_size_y 15
#define map_size_x 224
#define BACKGROUND ' '
#define WALL '#'
#define PORTAL '@'
#define PIPE 'O'
#define POLE 'I'

static HANDLE g_hScreen[2];
int screenIndex = 0;
int map_info[map_size_y][map_size_x];
char map[map_size_y][map_size_x];

int canJump = 0;
int jumping = 0;
int jumping2 = 0;

int num_of_monster = 0;

int pipe_out_x;
int pipe_out_y;

typedef struct {
	int x, y;
	char ch;
	int alive;
	int direction;
} object;

int time;
char timeS[100];
char key;
int clear = 0;
object player;
object monster[100];

void gotoxy(int x, int);
void cursor(int n);
void move(int x, int y, object* obj);
void drawMap(void);
int isCollideWith(char block, int x, int y, object* obj);
int isCollideWithMapX(int x, int y, object* obj);
int isCollideWithCeiling(int x, int y, object* obj);
int isCollideWithFloor(int x, int y, object* obj);
void kill(object* obj);
void kill_all_monsters(void);

void readMapFile(void);
void drawMainScreen(void);
void drawClearScreen(void);
void drawGameOverScreen(void);
void init(void);
void playerControl(void);
void monsterControl(object* monster);
void drawTime(void);
void writeBuffer(int n, int x, int y, char* ch);
void writeBufferString(int n, int x, int y, char* s);
void clearBuffer(int screenIndex);


int main()
{
	int time2, cnt = 0;
	system("mode con cols=30 lines=15");
	g_hScreen[0] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	g_hScreen[1] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	drawMainScreen();
	while (1) {
		init();
		while (1)
		{
			playerControl();
			if (time % 10 == 0) {
				drawMap();
			}
			if (time % 20 == 0) {
				if (!jumping && !jumping2) {
					move(0, 1, &player);
				}
			}
			if (jumping || jumping2) {
				if (!cnt) {
					if (jumping2) {
						cnt += 2;
					}
					time2 = time;
				}
				if ((time - time2) % 20 == 0) {
					move(0, -1, &player);
					cnt++;
				}
				if (cnt == 4) {
					move(0, 0, &player);
				}
				if (cnt == 5) {
					jumping = 0;
					jumping2 = 0;
					cnt = 0;
				}
			}
			if (time % 200 == 0) {
				for (int i = 0; i < num_of_monster; i++) {
					if (monster[i].alive) {
						monsterControl(&monster[i]);
					}
				}
			}
			Sleep(10);
			time += 10;
			if (!player.alive) {
				drawGameOverScreen();
				break;
			}
			if (clear) {
				clear = 0;
				drawClearScreen();
				break;
			}
		}
	}
	return 0;
}

void init(void) // 초기 상태
{
	int direction = -1;
	kill_all_monsters();
	num_of_monster = 0;
	readMapFile();

	for (int i = 0; i < map_size_y; i++) {
		for (int j = 0; j < map_size_x; j++) {
			if (map[i][j] == 'M') {
				monster[num_of_monster].ch = 'M';
				monster[num_of_monster].x = j;
				monster[num_of_monster].y = i;
				monster[num_of_monster].alive = 1;
				monster[num_of_monster].direction = direction;
				direction = -direction;
				num_of_monster++;
			}
			else if (map[i][j] == 'P') {
				player.ch = 'P';
				player.x = j;
				player.y = i;
				player.alive = 1;
			}
		}
	}

	time = 0;

	cursor(0);
	drawMap();
	move(0, 0, &player);
	for (int i = 0; i < num_of_monster; i++) {
		if (monster[i].alive) {
			move(0, 0, &monster[i]);
		}
	}
}

void playerControl(void) // 플레이어 키 입력 체크
{
	if (kbhit()) {
		key = getch();
		switch (key) {
		case 'w':
			if (canJump) {
				jumping = 1;
				canJump = 0;
			}
			break;
		case 'a':
			move(-1, 0, &player);
			break;
		case 'd':
			move(1, 0, &player);
			break;
		case 's':
			if (map_info[player.y + 1][player.x] == 6) {
				map[player.y][player.x] = BACKGROUND;
				player.y = pipe_out_y - 1;
				player.x = pipe_out_x;
				map[player.y][player.x] = player.ch;
			}
			break;
		default:
			break;
		}
	}
}

void monsterControl(object* monster) // 몬스터 움직임 제어
{
	if (isCollideWith(WALL, monster->direction, 0, monster) || isCollideWith(PIPE, monster->direction, 0, monster) || isCollideWithMapX(monster->direction, 0, monster) || map[monster->y + 1][monster->x + monster->direction] != WALL && map[monster->y + 1][monster->x + monster->direction] != PIPE) monster->direction = -monster->direction;
	move(monster->direction, 0, monster);
}

void gotoxy(int x, int y) // 커서 위치를 이동시킨다.
{
	COORD pos = { x,y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void cursor(int n) // 0 : 커서 사라짐 / 1 : 커서 생김
{
	HANDLE hConsole;
	CONSOLE_CURSOR_INFO ConsoleCursor;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	ConsoleCursor.bVisible = n;
	ConsoleCursor.dwSize = 1;
	SetConsoleCursorInfo(hConsole, &ConsoleCursor);
}

void move(int x, int y, object * obj) // 움직임 제어. 잡다한 충돌 이벤트도 처리함
{
	if (isCollideWithMapX(x, y, obj)) {
		return;
	}
	if (isCollideWithCeiling(x, y, obj)) {
		return;
	}
	if (isCollideWithFloor(x, y, obj)) {
		if (obj->ch == player.ch) {
			kill(&player);
			return;
		}
		if (obj->ch == 'M') {
			kill(obj);
			return;
		}
		return;
	}
	if (isCollideWith(WALL, x, y, obj) || isCollideWith(PIPE, x, y, obj)) {
		if (y > 0) canJump = 1;
		return;
	}
	if (isCollideWith(WALL, x, y + 1, obj) || isCollideWith(PIPE, x, y + 1, obj)) {
		canJump = 1;
	}
	if (obj->ch == player.ch && isCollideWith('M', x, y + 1, obj)) {
		for (int i = 0; i < num_of_monster; i++) {
			if (isCollideWith(player.ch, x, -y - 1, &monster[i])) {
				kill(&monster[i]);
			}
		}
		//move(0, -1, obj);
		jumping2 = 1;
		return;
	}
	map[obj->y][obj->x] = BACKGROUND;
	if (obj->ch == player.ch && isCollideWith(PORTAL, x, y, obj) || obj->ch == player.ch && isCollideWith(POLE, x, y, obj)) {
		clear = 1;
		return;
	}
	if (obj->ch == player.ch && isCollideWith('M', x, y, obj) || obj->ch == 'M' && isCollideWith(player.ch, x, y, obj)) {
		kill(&player);
		return;
	}
	obj->x += x;
	obj->y += y;
	map[obj->y][obj->x] = obj->ch;

}
void drawMap(void) // 맵의 내용을 화면에 출력
{
	int start = 0;
	sprintf_s(timeS, 100, "%d", time / 400);
	screenIndex = !screenIndex;
	if (player.x > map_size_y / 2) start = player.x - map_size_y / 2;
	if (player.x > map_size_x - map_size_y / 2) start = map_size_x - map_size_y;
	for (int i = 0; i < map_size_y; i++) {
		for (int j = start; j < start + map_size_y; j++) {
			//writeBuffer(screenIndex, j - start, i, &map[i][j]);
			writeBuffer(screenIndex, 2 * (j - start), i, &map[i][j]);
			writeBuffer(screenIndex, 2 * (j - start) + 1, i, &map[i][j]);
			//writeBuffer(screenIndex, 4 * (j - start), 2 * i, &map[i][j]);
			//writeBuffer(screenIndex, 4 * (j - start) + 1, 2 * i, &map[i][j]);
			//writeBuffer(screenIndex, 4 * (j - start), 2 * i + 1, &map[i][j]);
			//writeBuffer(screenIndex, 4 * (j - start) + 1, 2 * i + 1, &map[i][j]);
			//writeBuffer(screenIndex, 4 * (j - start) + 2, 2 * i, &map[i][j]);
			//writeBuffer(screenIndex, 4 * (j - start) + 3, 2 * i, &map[i][j]);
			//writeBuffer(screenIndex, 4 * (j - start) + 2, 2 * i + 1, &map[i][j]);
			//writeBuffer(screenIndex, 4 * (j - start) + 3, 2 * i + 1, &map[i][j]);
		}
	}
	writeBufferString(screenIndex, 0, 0, "TIME = ");
	writeBufferString(screenIndex, 7, 0, timeS);
	SetConsoleActiveScreenBuffer(g_hScreen[screenIndex]);
}

void drawMainScreen(void) // 메인(시작) 화면
{
	char key;
	screenIndex = !screenIndex;
	clearBuffer(screenIndex);
	writeBufferString(screenIndex, 0, 0, "WELCOME TO CONSOLE MARIO");
	writeBufferString(screenIndex, 0, 1, "PRESS R TO START, E TO EXIT");
	SetConsoleActiveScreenBuffer(g_hScreen[screenIndex]);
	while (1) {
		if (kbhit()) {
			key = getch();
			if (key == 'r') {
				return;
			}
			else if (key == 'e') {
				exit(0);
			}
		}
	}
}

void drawClearScreen(void) // 게임 클리어시 화면
{
	char key;
	screenIndex = !screenIndex;
	clearBuffer(screenIndex);
	writeBufferString(screenIndex, 0, 0, "GAME CLEAR!!!");
	writeBufferString(screenIndex, 0, 1, "CLEAR TIME = ");
	writeBufferString(screenIndex, 13, 1, timeS);
	writeBufferString(screenIndex, 0, 2, "PRESS R TO RESTART, E TO EXIT");
	SetConsoleActiveScreenBuffer(g_hScreen[screenIndex]);
	while (1) {
		if (kbhit()) {
			key = getch();
			if (key == 'r') {
				return;
			}
			else if (key == 'e') {
				exit(0);
			}
		}
	}
}

void drawGameOverScreen(void) // 플레이어 사망시 화면
{
	char key = ' ';
	screenIndex = !screenIndex;
	clearBuffer(screenIndex);
	writeBufferString(screenIndex, 0, 0, "GAME OVER...");
	writeBufferString(screenIndex, 0, 1, "PRESS R TO RESTART, E TO EXIT");
	SetConsoleActiveScreenBuffer(g_hScreen[screenIndex]);
	while (1) {
		if (kbhit()) {
			key = getch();
			if (key == 'r') {
				return;
			}
			else if (key == 'e') {
				exit(0);
			}
		}
	}
}


int isCollideWith(char block, int x, int y, object * obj) // 다른 오브젝트의 블록과 충돌했는지 체크
{
	if ((map[obj->y + y][obj->x + x] == block)) return 1;
	else return 0;
}

int isCollideWithMapX(int x, int y, object * obj) // 맵의 가장자리와 충돌했는지 체크
{
	if (obj->x + x < 0 || obj->x + x > map_size_x - 1) return 1;
	else return 0;
}

int isCollideWithCeiling(int x, int y, object * obj) // 맵의 꼭대기와 충돌했는지 체크
{
	if (obj->y + y < 0) return 1;
	else return 0;
}

int isCollideWithFloor(int x, int y, object * obj) // 맵의 꼭대기와 충돌했는지 체크
{
	if (obj->y + y > map_size_y - 1) return 1;
	else return 0;
}

void kill(object * obj) // 생성된 오브젝트 삭제
{
	obj->alive = 0;
	map[obj->y][obj->x] = BACKGROUND;
}

void kill_all_monsters(void) // 모든 몬스터 삭제
{
	for (int i = 0; i < num_of_monster; i++) {
		kill(&monster[i]);
	}
}

void writeBuffer(int n, int x, int y, char* ch) // 버퍼 내용 입력 함수 (문자 1개)
{
	DWORD dw;
	COORD CursorPosition = { x, y };
	SetConsoleCursorPosition(g_hScreen[n], CursorPosition);
	WriteFile(g_hScreen[n], ch, 1, &dw, NULL);
}

void writeBufferString(int n, int x, int y, char* s) // 버퍼 내용 입력 함수 (문자열)
{
	DWORD dw;
	COORD CursorPosition = { x, y };
	SetConsoleCursorPosition(g_hScreen[n], CursorPosition);
	WriteFile(g_hScreen[n], s, strlen(s), &dw, NULL);
}

void clearBuffer(int screenIndex) // 버퍼 내용을 지워줌
{
	for (int i = 0; i < map_size_y; i++) {
		for (int j = 0; j < map_size_y; j++) {
			writeBuffer(screenIndex, 2 * j, i, " ");
			writeBuffer(screenIndex, 2 * j + 1, i, " ");
		}
	}
}


void readMapFile(void) // txt 파일에서 맵 정보를 불러옴
{
	FILE* fp;
	fp = fopen("map.txt", "r");
	for (int i = 0; i < map_size_y; i++) {
		for (int j = 0; j < map_size_x; j++) {
			fscanf(fp, "%d", &map_info[i][j]);
			switch (map_info[i][j]) {
			case 0: map[i][j] = BACKGROUND; break;
			case 1: map[i][j] = WALL; break;
			case 2: map[i][j] = PORTAL; break;
			case 3: map[i][j] = 'M'; break;
			case 4: map[i][j] = 'P'; break;
			case 5: map[i][j] = PIPE; break;
			case 6: map[i][j] = PIPE; break;
			case 7: 
				map[i][j] = PIPE;
				pipe_out_x = j;
				pipe_out_y = i;
				break;
			case 8: map[i][j] = POLE; break;
			default: map[i][j] = BACKGROUND; break;
			}
		}
	}
	fclose(fp);
}