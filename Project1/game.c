
#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include <windows.h>
#pragma warning(disable : 4996)

#define N 10
#define BACKGROUND ' '
#define WALL '#'
#define PORTAL '@'
#define num_of_monster 10

static HANDLE g_hScreen[2];
int screenIndex = 0;

char map[N][3 * N];

int canJump = 0;
int jumping = 0;

typedef struct {
	int x, y;
	char ch;
	int collision;
	int alive;
	int direction;
} object;

int player_startPos_X = 0;
int player_startPos_Y = N - 2;
int time;
char key;
object player;
object monster[num_of_monster];

void gotoxy(int x, int);
void cursor(int n);
void move(int x, int y, object* obj);
void moveN(int x, int y, int n, object* obj);
void drawMap(void);
int isCollideWith(char block, int x, int y, object* obj);
int isCollideWithMap(int x, int y, object* obj);
void kill(object* obj);
void kill_all_monsters(void);

void init(void);
void playerControl(void);
void monsterControl(object* monster);
void drawTime(void);
void WriteBuffer(int n, int x, int y, char* ch);


int main()
{
	int time2, cnt = 0;
	g_hScreen[0] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	g_hScreen[1] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	while (1) {
		init();
		while (1)
		{
			//drawTime();
			playerControl();
			if (time % 10 == 0) {
				//system("cls");
				drawMap();
			}
			if (time % 20 == 0) {
				if (!jumping) {
					move(0, 1, &player);
				}
			}
			if (jumping) {
				if (!cnt) {
					time2 = time;
				}
				if ((time - time2) % 20 == 0) {
					move(0, -1, &player);
					cnt++;
				}
				if (cnt == 3) {
					jumping = 0;
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
				break;
			}
		}
	}
	return 0;
}

void init(void) // �ʱ� ����
{
	kill_all_monsters();
	player.ch = 'P';
	player.x = player_startPos_X;
	player.y = player_startPos_Y;
	player.alive = 1;
	for (int i = 0; i < num_of_monster; i++) {
		monster[i].alive = 0;
		monster[i].direction = 1;
	}
	monster[0].ch = 'M';
	monster[0].x = 6;
	monster[0].y = N - 2;
	monster[0].alive = 1;
	monster[1].ch = 'M';
	monster[1].x = 13;
	monster[1].y = N - 2;
	monster[1].alive = 1;
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
	for (int i = 0; i < num_of_monster; i++) {
		if (monster[i].alive) {
			move(0, 0, &monster[i]);
		}
	}
}

void playerControl(void) // �÷��̾� Ű �Է� üũ
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

void monsterControl(object* monster) // ���� ������ ����
{
	if (isCollideWith(WALL, monster->direction, 0, monster) || isCollideWithMap(monster->direction, 0, monster)) monster->direction = -monster->direction;
	move(monster->direction, 0, monster);
}

void gotoxy(int x, int y) // Ŀ�� ��ġ�� �̵���Ų��.
{
	COORD pos = { x,y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void cursor(int n) // 0 : Ŀ�� ����� / 1 : Ŀ�� ����
{
	HANDLE hConsole;
	CONSOLE_CURSOR_INFO ConsoleCursor;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	ConsoleCursor.bVisible = n;
	ConsoleCursor.dwSize = 1;
	SetConsoleCursorInfo(hConsole, &ConsoleCursor);
}

void move(int x, int y, object* obj) // ������ ����. ����� �浹 �̺�Ʈ�� ó����
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
	if (obj->ch == player.ch && isCollideWith('M', x, y + 1, obj)) {
		for (int i = 0; i < num_of_monster; i++) {
			if (isCollideWith(monster[i].ch, x, y+1, obj)) {
				kill(&monster[i]);
			}
		}
		move(0, -1, obj);
		return;
	}
	map[obj->y][obj->x] = BACKGROUND;
	if (obj->ch == player.ch && (isCollideWith(PORTAL, x, y, obj) || isCollideWith('M', x, y, obj))) {
		kill(&player);
		return;
	}
	if (obj->ch == 'M' && isCollideWith(player.ch, x, y, obj)) {
		kill(&player);
		return;
	}
	obj->x += x;
	obj->y += y;
	map[obj->y][obj->x] = obj->ch;

}
void drawMap(void) // ���� ������ ȭ�鿡 ���
{
	int start = 0;
	screenIndex = !screenIndex;
	if (player.x > N / 2) start = player.x - N / 2;
	if (player.x > 2 * N + N / 2) start = 2 * N;
	if (start + N)
		for (int i = 0; i < N; i++) {
			for (int j = start; j < start + N; j++) {
				WriteBuffer(screenIndex, 4 * (j - start), 2 * i, &map[i][j]);
				WriteBuffer(screenIndex, 4 * (j - start) + 1, 2 * i, &map[i][j]);
				WriteBuffer(screenIndex, 4 * (j - start), 2 * i + 1, &map[i][j]);
				WriteBuffer(screenIndex, 4 * (j - start) + 1, 2 * i + 1, &map[i][j]);
				WriteBuffer(screenIndex, 4 * (j - start) + 2, 2 * i, &map[i][j]);
				WriteBuffer(screenIndex, 4 * (j - start) + 3, 2 * i, &map[i][j]);
				WriteBuffer(screenIndex, 4 * (j - start) + 2, 2 * i + 1, &map[i][j]);
				WriteBuffer(screenIndex, 4 * (j - start) + 3, 2 * i + 1, &map[i][j]);
			}
		}
	SetConsoleActiveScreenBuffer(g_hScreen[screenIndex]);
}


void drawTime(void)
{
	gotoxy(0, 0);
	printf("TIME : %d", time / 100);
}

int isCollideWith(char block, int x, int y, object * obj) // �ٸ� ������Ʈ�� ��ϰ� �浹�ߴ��� üũ
{
	if ((map[obj->y + y][obj->x + x] == block)) return 1;
	else return 0;
}

int isCollideWithMap(int x, int y, object * obj) // ���� �����ڸ��� �浹�ߴ��� üũ
{
	if (obj->x + x < 0 || obj->x + x > 3 * N - 1 || obj->y + y < 0 || obj->y + y > N - 1) return 1;
	else return 0;
}

void kill(object * obj) // ������ ������Ʈ ����
{
	obj->alive = 0;
	map[obj->y][obj->x] = BACKGROUND;
}

void kill_all_monsters(void) // ��� ���� ����
{
	for (int i = 0; i < num_of_monster; i++) {
		kill(&monster[i]);
	}
}

void WriteBuffer(int n, int x, int y, char* ch) // ���� ���� �Է� �Լ�
{
	DWORD dw;
	COORD CursorPosition = { x, y };
	SetConsoleCursorPosition(g_hScreen[n], CursorPosition);
	WriteFile(g_hScreen[n], ch, 1, &dw, NULL);
}