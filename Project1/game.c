
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

int canJump = 0;
int jumping = 0;

typedef struct {
	int x, y;
	char ch;
	int collision;
	int alive;
} object;

int player_startPos_X = 0;
int player_startPos_Y = N - 2;
int time;
char key;
object player;
object monster;

void gotoxy(int x, int);
void cursor(int n);
void move(int x, int y, object* obj);
void moveN(int x, int y, int n, object* obj);
void drawMap(void);
int isCollideWith(char block, int x, int y, object* obj);
int isCollideWithMap(int x, int y, object* obj);
void kill(object* obj);

void init(void);
void playerControl(void);
void monsterControl(void);
void drawTime(void);


int main()
{
	int time2, cnt = 0;
	init();

	while (1)
	{
		drawTime();
		playerControl();
		if (time % 10 == 0) {
			system("cls");
			drawMap();
			if (!jumping) {
				move(0, 1, &player);
			}
		}
		if (jumping) {
			if (!cnt) {
				time2 = time;
			}
			if ((time - time2) % 10 == 0) {
				move(0, -1, &player);
				cnt++;
			}
			if (cnt == 3) {
				jumping = 0;
				cnt = 0;
			}
		}
		if (monster.alive && time % 100 == 0) {
			monsterControl(&monster);
		}
		Sleep(10);
		time += 10;
	}
	return 0;
}

void init(void)
{
	player.ch = 'P';
	player.x = player_startPos_X;
	player.y = player_startPos_Y;
	player.alive = 1;
	monster.ch = 'M';
	monster.x = 6;
	monster.y = N - 2;
	monster.alive = 1;
	time = 0;
	for (int j = 0; j < 3 * N; j++) {
		map[N - 1][j] = WALL;
	}
	map[N - 2][5] = WALL;
	map[N - 3][6] = WALL;
	map[N - 4][7] = WALL;
	map[N - 4][8] = WALL;
	map[N - 4][9] = WALL;
	map[N - 6][10] = WALL;
	map[N - 3][18] = PORTAL;

	cursor(0);
	drawMap();
	move(0, 0, &player);
	move(0, 0, &monster);
}

void playerControl(void)
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
		default:
			break;
		}
	}
}

void monsterControl(object* monster)
{
	static int x = 1;
	if (isCollideWith(WALL, x, 0, monster) || isCollideWithMap(x, 0, monster)) x = -x;
	move(x, 0, monster);
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

void move(int x, int y, object * obj)
{
	if (isCollideWithMap(x, y, obj)) {
		return;
		obj->collision = 1;
	}
	if (isCollideWith(WALL, x, y, obj)) {
		if (y > 0) canJump = 1;
		obj->collision = 1;
		return;
	}
	if (isCollideWith(WALL, x, y + 1, obj)) {
		canJump = 1;
	}
	if (obj->ch == player.ch && isCollideWith(monster.ch, x, y + 1, obj)) {
		move(0, -1, obj);
		kill(&monster);
		return;
	}
	map[obj->y][obj->x] = BACKGROUND;
	if (isCollideWith(PORTAL, x, y, obj) || isCollideWith(monster.ch, x, y, obj)) {
		//init();
		//return;
		obj->x = player_startPos_X;
		obj->y = player_startPos_Y;
		x = 0;
		y = 0;
	}
	obj->x += x;
	obj->y += y;
	map[obj->y][obj->x] = obj->ch;

}

void moveN(int x, int y, int n, object * obj)
{
	for (int i = 0; i < n; i++) {
		//Sleep(50);
		move(0, -1, obj);
		canJump = 0;
	}
}

void drawMap(void)
{
	int start = 0;
	if (player.x > N / 2) start = player.x - N / 2;
	if (player.x > 2 * N + N / 2) start = 2 * N;
	if (start + N)
		for (int i = 0; i < N; i++) {
			for (int j = start; j < start + N; j++) {
				printf("%c%c", map[i][j], map[i][j]);
			}
			printf("\n");
		}
}

void drawTime(void)
{
	gotoxy(0, 0);
	printf("TIME : %d", time / 100);
}

int isCollideWith(char block, int x, int y, object * obj)
{
	if ((map[obj->y + y][obj->x + x] == block)) return 1;
	else return 0;
}

int isCollideWithMap(int x, int y, object * obj)
{
	if (obj->x + x < 0 || obj->x + x > 3 * N - 1 || obj->y + y < 0 || obj->y + y > N - 1) return 1;
	else return 0;
}

void kill(object * obj)
{
	obj->alive = 0;
	map[obj->y][obj->x] = BACKGROUND;
}