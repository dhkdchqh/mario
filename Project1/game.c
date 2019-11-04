#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include <windows.h>
#pragma warning(disable : 4996)

#define N 10

char map[N][N];

typedef struct {
	int x;
	int y;
	char ch;
} object;

void gotoxy(int x, int);
void cursor(int n);
void move(int x, int y, object* obj);

int main(void)
{
	char key;
	object player;
	player.ch = 'P';
	player.x = 0;
	player.y = 0;
	cursor(0);
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			printf("X");
		}
		printf("\n");
	}
	move(0, 0, &player);
	while (1) {
		if (kbhit()) {
			key = getch();
			switch (key) {
			case 'w':
				move(0, -1, &player);
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

void move(int x, int y, object * obj)
{
	if (obj->x + x < 0 || obj->x + x > N - 1 || obj->y + y < 0 || obj->y + y > N - 1) return;
	gotoxy(obj->x, obj->y);
	printf("X");
	obj->x += x;
	obj->y += y;
	gotoxy(obj->x, obj->y);
	printf("%c", obj->ch);
}