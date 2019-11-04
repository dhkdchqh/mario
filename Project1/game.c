#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include <windows.h>
#pragma warning(disable : 4996)

#define N 10

char map[N][3 * N];
char background = ' ';
char wall = '@';
int collision;

typedef struct {
	int x;
	int y;
	char ch;
} object;


void gotoxy(int x, int);
void cursor(int n);
void move(int x, int y, object* obj);
void drawmap(void);

int main(void)
{
	object player;
	player.ch = 'P';
	player.x = 0;
	player.y = 0;

	for (int j = 0; j < 3 * N; j++) {
		map[N - 1][j] = wall;
	}
	map[N - 2][5] = wall;
	char key;
	cursor(0);
	drawmap();
	move(0, 0, &player);
	while (1) {
		if (kbhit()) {
			key = getch();
			switch (key) {
			case 'w':
				if (collision) {
					move(0, -3, &player);
					collision = 0;
				}
				break;
			case 'a':
				move(-1, 0, &player);
				break;
			case 's':
				move(0, 1, &player);
				break;
			case 'd':
				move(1, 0, &player);
				break;
			default:
				break;
			}
		}
	}
}

void gotoxy(int x, int y)
{
	COORD pos = { x,y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void cursor(int n) // 커서 보이기 & 숨기기
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
		return;
	}
	if (y > 0 && map[obj->y + y + 1][obj->x + x] == wall) {
		collision = 1;
	}
	gotoxy(obj->x, obj->y);
	printf("%c", background);
	obj->x += x;
	obj->y += y;
	gotoxy(obj->x, obj->y);
	printf("%c", obj->ch);
}

void drawmap(void)
{
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < 3 * N; j++) {
			printf("%c", map[i][j]);
		}
		printf("\n");
	}
}
