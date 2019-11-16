
#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include <windows.h>
#pragma warning(disable : 4996)

#define N 10
#define BACKGROUND ' '
#define WALL '#'
#define PORTAL '@'

static HANDLE g_hScreen[2];
int screenIndex = 0;

char map[N][3 * N];

int canJump = 0;
int jumping = 0;

int num_of_monster = 0;

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
object monster[100];

void gotoxy(int x, int);
void cursor(int n);
void move(int x, int y, object* obj);
void moveN(int x, int y, int n, object* obj);
void drawMap(void);
int isCollideWith(char block, int x, int y, object* obj);
int isCollideWithMap(int x, int y, object* obj);
void kill(object* obj);
void kill_all_monsters(void);

void readMapFile(void);
void init(void);
void playerControl(void);
void monsterControl(object* monster);
void drawTime(void);
void writeBuffer(int n, int x, int y, char* ch);


int main()
{
	int time2, cnt = 0;
	g_hScreen[0] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	g_hScreen[1] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	while (1) {
		init();
		while (1)
		{
			playerControl();
			if (time % 10 == 0) {
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
					move(0, 0, &player);
				}
				if (cnt == 4) {
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

void init(void) // 초기 상태
{
	kill_all_monsters();
	num_of_monster = 0;
	readMapFile();
	player.ch = 'P';
	player.x = player_startPos_X;
	player.y = player_startPos_Y;
	player.alive = 1;
	//for (int i = 0; i < 100; i++) {
	//	monster[i].alive = 0;
	//	monster[i].direction = 1;
	//}

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < 3 * N; j++) {
			if (map[i][j] == 'M') {
				monster[num_of_monster].ch = 'M';
				monster[num_of_monster].x = j;
				monster[num_of_monster].y = i;
				monster[num_of_monster].alive = 1;
				monster[num_of_monster].direction = 1;
				num_of_monster++;
			}
		}
	}
	//monster[0].ch = 'M';
	//monster[0].x = 6;
	//monster[0].y = N - 2;
	//monster[0].alive = 1;
	//monster[1].ch = 'M';
	//monster[1].x = 9;
	//monster[1].y = N - 5;
	//monster[1].alive = 1;
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
		default:
			break;
		}
	}
}

void monsterControl(object* monster) // 몬스터 움직임 제어
{
	if (isCollideWith(WALL, monster->direction, 0, monster) || isCollideWithMap(monster->direction, 0, monster) || map[monster->y+1][monster->x+monster->direction] != WALL) monster->direction = -monster->direction;
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

void move(int x, int y, object* obj) // 움직임 제어. 잡다한 충돌 이벤트도 처리함
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
			if (isCollideWith(player.ch, x, -y - 1, &monster[i])) {
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
void drawMap(void) // 맵의 내용을 화면에 출력
{
	int start = 0;
	screenIndex = !screenIndex;
	if (player.x > N / 2) start = player.x - N / 2;
	if (player.x > 2 * N + N / 2) start = 2 * N;
		for (int i = 0; i < N; i++) {
			for (int j = start; j < start + N; j++) {
				//writeBuffer(screenIndex, j - start, i, &map[i][j]);
				writeBuffer(screenIndex, 4 * (j - start), 2 * i, &map[i][j]);
				writeBuffer(screenIndex, 4 * (j - start) + 1, 2 * i, &map[i][j]);
				writeBuffer(screenIndex, 4 * (j - start), 2 * i + 1, &map[i][j]);
				writeBuffer(screenIndex, 4 * (j - start) + 1, 2 * i + 1, &map[i][j]);
				writeBuffer(screenIndex, 4 * (j - start) + 2, 2 * i, &map[i][j]);
				writeBuffer(screenIndex, 4 * (j - start) + 3, 2 * i, &map[i][j]);
				writeBuffer(screenIndex, 4 * (j - start) + 2, 2 * i + 1, &map[i][j]);
				writeBuffer(screenIndex, 4 * (j - start) + 3, 2 * i + 1, &map[i][j]);
			}
		}
	SetConsoleActiveScreenBuffer(g_hScreen[screenIndex]);
}

void mainScreen(void)
{
	printf("   ");


}


void drawTime(void)
{
	gotoxy(0, 0);
	printf("TIME : %d", time / 100);
}

int isCollideWith(char block, int x, int y, object * obj) // 다른 오브젝트의 블록과 충돌했는지 체크
{
	if ((map[obj->y + y][obj->x + x] == block)) return 1;
	else return 0;
}

int isCollideWithMap(int x, int y, object * obj) // 맵의 가장자리와 충돌했는지 체크
{
	if (obj->x + x < 0 || obj->x + x > 3 * N - 1 || obj->y + y < 0 || obj->y + y > N - 1) return 1;
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

void writeBuffer(int n, int x, int y, char* ch) // 버퍼 내용 입력 함수
{
	DWORD dw;
	COORD CursorPosition = { x, y };
	SetConsoleCursorPosition(g_hScreen[n], CursorPosition);
	WriteFile(g_hScreen[n], ch, 1, &dw, NULL);
}


void readMapFile(void)
{
	FILE* fp;
	int tmp;
	fp = fopen("map.txt", "r");
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < 3 * N; j++) {
			fscanf(fp, "%d", &tmp);
			switch (tmp) {
			case 0: map[i][j] = BACKGROUND; break;
			case 1: map[i][j] = WALL; break;
			case 2: map[i][j] = PORTAL; break;
			case 3: map[i][j] = 'M'; break;
			}
		}
	}
	fclose(fp);
}