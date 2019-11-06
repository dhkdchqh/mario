
#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include <windows.h>
#pragma warning(disable : 4996)

#define N 10
#define BACKGROUND ' '
#define WALL '#'
#define PORTAL '@'

char map[N][3 * N];

int canJump;

typedef struct {
	int x, y;
	char ch;
} object;

int player_startPos_X = 0;
int player_startPos_Y = 0;

void gotoxy(int x, int);
void cursor(int n);
void move(int x, int y, object* obj);
void moveN(int x, int y, int n, object* obj);
void drawMap(void);
int isCollideWith(char block, int x, int y, object* obj);
int isCollideWithMap(int x, int y, object* obj);

int main(void)
{
	char key;
	object player;
	player.ch = 'P';
	player.x = player_startPos_X;
	player.y = player_startPos_Y;

	for (int j = 0; j < 3 * N; j++) {
		map[N - 1][j] = WALL;
	}
	map[N - 2][5] = WALL;
	map[N - 3][6] = WALL;
	map[N - 4][7] = WALL;
	map[N - 4][8] = WALL;
	map[N - 4][9] = WALL;
	map[N - 2][18] = PORTAL;

	cursor(0);
	drawMap();
	move(0, 0, &player);
	while (1) {
		if (kbhit()) {
			key = getch();
			switch (key) {
			case 'w':
				if (canJump) {
					moveN(0, -1, 3, &player);
				}
				break;
			case 'a':
				move(-1, 0, &player);
				break;
			case 'd':
				move(1, 0, &player);
				break;
			default:
				break;
			}
		}
		else {
			Sleep(50);
			move(0, 1, &player);
		}
	}

	return 0;
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

void move(int x, int y, object* obj)
{
	if (isCollideWithMap(x, y, obj)) return;
	if (isCollideWith(WALL, x, y, obj)) {
		if (y > 0) canJump = 1;
		return;
	}
	if (isCollideWith(WALL, x, y + 1, obj)) {
		canJump = 1;
	}
	gotoxy(obj->x, obj->y);
	printf("%c", BACKGROUND);
	if (isCollideWith(PORTAL, x, y, obj)) {
		obj->x = player_startPos_X;
		obj->y = player_startPos_Y;
		x = 0;
		y = 0;
	}
	obj->x += x;
	obj->y += y;
	gotoxy(obj->x, obj->y);
	printf("%c", obj->ch);
}

void moveN(int x, int y, int n, object* obj)
{
	for (int i = 0; i < n; i++) {
		Sleep(50);
		move(0, -1, obj);
		canJump = 0;
	}
}

void drawMap(void)
{
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < 3 * N; j++) {
			printf("%c", map[i][j]);
		}
		printf("\n");
	}
}

int isCollideWith(char block, int x, int y, object* obj)
{
	if ((map[obj->y + y][obj->x + x] == block)) return 1;
	else return 0;
}

int isCollideWithMap(int x, int y, object * obj)
{
	if (obj->x + x < 0 || obj->x + x > 3 * N - 1 || obj->y + y < 0 || obj->y + y > N - 1) return 1;
	else return 0;
}