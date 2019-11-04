
#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include <windows.h>
#pragma warning(disable : 4996)

#define N 10

char map[N][3 * N];
char background = ' ';
char wall = '#';
char portal = '@';
int collision;

typedef struct {
	int x, y;
	char ch;
} object;

int player_x_start = 0;
int player_y_start = 0;

void gotoxy(int x, int);
void cursor(int n);
void move(int x, int y, object* obj);
void drawMap(void);

int main(void)
{
	object player;
	player.ch = 'P';
	player.x = player_x_start;
	player.y = player_y_start;

	for (int j = 0; j < 3 * N; j++) {
		map[N - 1][j] = wall;
	}
	map[N - 2][5] = wall;
	map[N - 3][6] = wall;
	map[N - 4][7] = wall;
	map[N - 4][8] = wall;
	map[N - 4][9] = wall;
	map[N - 2][18] = portal;
	char key;
	cursor(0);
	drawMap();
	move(0, 0, &player);
	while (1) {
		if (kbhit()) {
			key = getch();
			switch (key) {
			case 'w':
				if (collision) {
					move(0, -1, &player);
					collision = 0;
					move(0, -1, &player);
					collision = 0;
					move(0, -1, &player);
					collision = 0;
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
			Sleep(100);
			move(0, 1, &player);
		}
	}
}

void gotoxy(int x, int y)
{
	COORD pos = { x,y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void cursor(int n) // Ä¿¼­ º¸ÀÌ±â & ¼û±â±â
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
	if (obj->x + x < 0 || obj->x + x > 3 * N - 1 || obj->y + y < 0 || obj->y + y > N - 1) return;
	if (map[obj->y + y][obj->x + x] == wall) {
		if (y > 0) collision = 1;
		return;
	}
	if (y > 0 && map[obj->y + y + 1][obj->x + x] == wall) {
		collision = 1;
	}
	gotoxy(obj->x, obj->y);
	printf("%c", background);
	if (map[obj->y + y][obj->x + x] == portal) {
		obj->x = player_x_start;
		obj->y = player_y_start;
		x = 0;
		y = 0;
	}
	//printf("O");
	obj->x += x;
	obj->y += y;
	gotoxy(obj->x, obj->y);
	printf("%c", obj->ch);
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
