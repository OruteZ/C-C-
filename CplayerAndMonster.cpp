
#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<string.h>
#include<Windows.h>
#include<math.h>



//맵의 좌표범위는 1부터 상수값까지
// 벽은 좌표상 0과 최댓값 + 1에 존재함
const int map_height = 50;
const int map_width = 230;
const int refrashRate = 50;
const int camera_height_range = 15;
const int camera_width_range = 50;
const int clearX = 220;
const int clearY = 10;

typedef enum {
    PLAYER, MONSTER, STAR
} UNIT_TYPE;

//커서 제거용 코드
typedef enum {
    NOCURSOR, SOLIDCURSOR, NORMALCURSOR
} CURSOR_TYPE;
void setcursortype(CURSOR_TYPE c) {
    CONSOLE_CURSOR_INFO CurInfo;
    switch (c) {
    case NOCURSOR:
        CurInfo.dwSize = 1;
        CurInfo.bVisible = FALSE;
        break;
    case SOLIDCURSOR:
        CurInfo.dwSize = 100;
        CurInfo.bVisible = TRUE;
        break;
    case NORMALCURSOR:
        CurInfo.dwSize = 20;
        CurInfo.bVisible = TRUE;
        break;
    }
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CurInfo);
}



//장애물, 몬스터, 플레이어가 공통적으로 갖는 자료형 unit 선언
//각 유닛의 좌표, 외형
struct unit {
    int x;
    int y;
    int BeforeRelativeX = -1;
    int BeforeRelativeY = -1;
    char look;
    UNIT_TYPE type;
};

//카메라 혼자만 갖는 자료형 camera 선언
//카메라의 x좌표, y좌표, 벽의 상하좌우 여부
struct camera {
    int x = 0;
    int y = 0;
    char wallRL = 'N';
    char wallUD = 'N';
}Camera;

//입력 좌표를 x,y로 이동시키는 함수
void gotoxy(int x, int y)
{
    COORD CursorPosition = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), CursorPosition);
}

//카메라의 중심에 따라 주변 환경을 출력하는 함수
//카메라를 return함
camera cameraSet(unit* units, camera Camera, int NumUnit)
{
    int i;
    //플레이어의 좌표를 카메라의 좌표에 대입
    Camera.x = units->x;
    Camera.y = units->y;


    //맵 양끝에 도달했을 경우 카메라가 더이상 이동하지않도록 좌표를 고정하고, WallRL 변수를 설정하는 코드
    //이전 WallRL과 다를경우에만 벽을 지움
    //x좌표가 카메라 범위보다 작을경우 왼쪽에 벽이있다는 뜻
    if (Camera.x <= camera_width_range)
    {
        Camera.x = camera_width_range;
        //if문 : 이전에 오른쪽에 벽이 작성되어있었을 경우 지운다
        if (Camera.wallRL == 'R') {
            for (i = 0; i < camera_height_range * 2; i++)
            {
                gotoxy(camera_width_range * 2, i);
                printf(" ");
            }
        }

        Camera.wallRL = 'L';
    }
    else if (Camera.x >= map_width - (camera_width_range - 2))
    {
        Camera.x = map_width - (camera_width_range - 2);
        if (Camera.wallRL == 'L') {
            for (i = 0; i < camera_height_range * 2; i++)
            {
                gotoxy(0, i);
                printf(" ");
            }
        }
        Camera.wallRL = 'R';
    }
    else
    {
        if (Camera.wallRL == 'L') {
            for (i = 0; i < camera_height_range * 2; i++)
            {
                gotoxy(0, i);
                printf(" ");
            }
        }
        if (Camera.wallRL == 'R') {
            for (i = 0; i < camera_height_range * 2; i++)
            {
                gotoxy(camera_width_range * 2, i);
                printf(" ");
            }
        }
        Camera.wallRL = 'N';
    }
    //맵 위아래에 도달했을 경우 카메라가 더이상 이동하지않도록 좌표를 고정하고, WallUD 변수를 설정하는 코드
    // //이전 WallUD과 다를경우에만 벽을 지움
    //y좌표가 카메라 범위보다 작을경우 위쪽에 벽이 있다는 뜻
    if (Camera.y <= camera_height_range)
    {
        Camera.y = camera_height_range;
        //if문 : 이전에 아래쪽에 벽이 작성되어있었을 경우 지운다
        if (Camera.wallUD == 'D') {
            for (i = 0; i < camera_width_range * 2 + 1; i++)
            {
                gotoxy(i, camera_height_range * 2 - 1);
                printf(" ");
            }
        }

        Camera.wallUD = 'U';
    }
    else if (Camera.y >= map_height - (camera_height_range - 2))
    {
        Camera.y = map_height - (camera_height_range - 2);
        if (Camera.wallUD == 'U') {
            for (i = 0; i < camera_width_range * 2 + 1; i++)
            {
                gotoxy(i, 0);
                printf(" ");
            }
        }
        Camera.wallUD = 'D';
    }
    else
    {
        if (Camera.wallUD == 'D') {
            for (i = 0; i < camera_width_range * 2 + 1; i++)
            {
                gotoxy(i, camera_height_range * 2 - 1);
                printf(" ");
            }
        }
        if (Camera.wallUD == 'U') {
            for (i = 0; i < camera_width_range * 2 + 1; i++)
            {
                gotoxy(i, 0);
                printf(" ");
            }
        }
        Camera.wallUD = 'N';
    }
    //units에 저장된 각 유닛들을 모두 참조하여 카메라의 범위 내로 해당되는 유닛을 선택하여, 화면 내에 출력하는 코드
    //카메라 좌표에 대한 각 유닛의 상대좌표를 구하고 상대좌표가 변화했을 시에만 삭제후 재 출력한다
    //상대좌표가
    for (i = 0; i < NumUnit; i++)
    {
        unit Unit = *(units + i);
        int DistanceX = abs(Unit.x - Camera.x);
        int DistanceY = abs(Unit.y - Camera.y);
        if (DistanceX <= camera_width_range && DistanceY <= camera_height_range)
        {
            int relativeX = Unit.x - Camera.x + camera_width_range;
            int relativeY = Unit.y - Camera.y + camera_height_range;
            if (relativeX != Unit.BeforeRelativeX || relativeY != Unit.BeforeRelativeY)
            {
                gotoxy(Unit.BeforeRelativeX, Unit.BeforeRelativeY);
                printf(" ");
            }
            gotoxy(relativeX, relativeY);
            printf("%c", Unit.look);
            (units + i)->BeforeRelativeX = relativeX;
            (units + i)->BeforeRelativeY = relativeY;
        }
        else
        {
            gotoxy(Unit.BeforeRelativeX, Unit.BeforeRelativeY);
            printf(" ");
            (units + i)->BeforeRelativeX = -1;
            (units + i)->BeforeRelativeY = -1;
        }

    }


    //저장된 WallUD와 WallRL에 따라서 벽을 작성하는 코드
    if (Camera.wallUD == 'D') {
        for (i = 0; i < camera_width_range * 2 + 1; i++)
        {
            gotoxy(i, camera_height_range * 2 - 1);
            printf("!");
        }
    }
    else if (Camera.wallUD == 'U') {
        for (i = 0; i < camera_width_range * 2 + 1; i++)
        {
            gotoxy(i, 0);
            printf("!");
        }
    }
    if (Camera.wallRL == 'L') {
        for (i = 0; i < camera_height_range * 2; i++)
        {
            gotoxy(0, i);
            printf("I");
        }
    }
    else if (Camera.wallRL == 'R') {
        for (i = 0; i < camera_height_range * 2; i++)
        {
            gotoxy(camera_width_range * 2, i);
            printf("I");
        }
    }

    return Camera;
}

void clearScreen()
{
    for (int i = 0; i <= camera_width_range * 2 + 1; i++)
    {
        for (int j = 0; j <= camera_height_range * 2; j++)
        {
            gotoxy(i, j);
            printf(" ");
        }
    }
}
//플레이어의 이동을 위해서 wasd를 입력받는 함수, 이외 입력받으면 NULL 출력
char getKey()
{
    if (_kbhit()) {
        return _getch();
    }
    return NULL;
}

//중력에 따라 플레이어 이동시키는 함수
void gravityMoving(unit* unit)
{
    if (unit->y < map_height) unit->y++;
}

//wasd입력받아서 플레이어의 좌표를 이동시키는 함수
void playerMoving(unit* player)
{
    char cSelect = '\0';
    cSelect = getKey();
    switch (cSelect)
    {
    case 'a':
        printf(" ");
        if (player->x > 1)
            (player->x)--;
        break;
    case 'w':
        printf(" ");
        if (player->y > 1)
            (player->y)--;
        break;
    case 's':
        printf(" ");
        if (player->y < map_height)
            (player->y)++;
        break;
    case 'd':
        printf(" ");
        if (player->x < map_width)
            (player->x)++;
        break;
    }
}

//정해진 AI에 따라 몬스터가 이동하는 함수
void monsterMoving(unit* units, int NumUnit)
{
    unit* player = units;
    unit* monster = units;
    int distanceX;
    int distanceY;
    for (int i = 1; i < NumUnit; i++)
    {
        monster = units + i;
        if (monster->type == MONSTER)
        {
            distanceX = (monster->x) - (player->x);
            distanceY = (monster->y) - (player->y);
            if (distanceX != 0) monster->x -= distanceX / abs(distanceX);
            if (distanceY != 0) monster->y -= distanceY / abs(distanceY);
        }
    }
}

//새로운유닛을 추가하는 함수. 플레이어 또는 몬스터
void addnewUnit(int* PNumUnit, int X, int Y, char Look, UNIT_TYPE type, unit* units)
{
    (*PNumUnit)++;
    int k = *PNumUnit;
    //if (k > 1) units = (unit*)(realloc(units, sizeof(unit) * k));
    int t = k - 1;
    (units + t)->x = X;
    (units + t)->y = Y;
    (units + t)->look = Look;
    (units + t)->type = type;
}

int mainmenu()
{
    gotoxy(camera_width_range / 2 - 3, camera_height_range / 4);
    printf("1. 게임 시작");
    gotoxy(camera_width_range / 2 - 3, camera_height_range * 2 / 4);
    printf("2. 게임 종료");
    gotoxy(camera_width_range / 2 - 3, camera_height_range * 3 / 4);
    printf("숫자키를 입력하세요");
    char cSelect = '\0';
    while (1)
    {
        cSelect = getKey();
        switch (cSelect)
        {
        case '1':
            return 1;
        case '2':
            return 2;
        }
    }
}

int Check(int NumUnit, unit* units)
{
    unit* monster = units;
    int playerX = units->x;
    int playerY = units->y;

    if (clearX == playerX && clearY == playerY) return 1;
    for (int i = 2; i < NumUnit; i++)
    {
        monster = units + i;
        if (monster->x == playerX && monster->y == playerY) return 2;
    }
    return 0;
}

int playgame()
{
    int NumUnit = 0;
    unit* units = (unit*)malloc(sizeof(unit) * 30);
    int clear = 0; // 0 진행중 1 탈락 2 클리어
    addnewUnit(&NumUnit, 1, map_height, 'K', PLAYER, units);
    addnewUnit(&NumUnit, clearX, clearY, '*', STAR, units);
    addnewUnit(&NumUnit, 20, map_height, 'M', MONSTER, units);
    addnewUnit(&NumUnit, 40, map_height, 'M', MONSTER, units);
    addnewUnit(&NumUnit, 60, map_height, 'M', MONSTER, units);
    addnewUnit(&NumUnit, 80, map_height, 'M', MONSTER, units);
    addnewUnit(&NumUnit, 100, map_height, 'M', MONSTER, units);
    addnewUnit(&NumUnit, 120, map_height, 'M', MONSTER, units);
    addnewUnit(&NumUnit, 140, map_height, 'M', MONSTER, units);
    addnewUnit(&NumUnit, 160, map_height, 'M', MONSTER, units);
    addnewUnit(&NumUnit, 180, map_height, 'M', MONSTER, units);
    addnewUnit(&NumUnit, 200, map_height, 'M', MONSTER, units);
    addnewUnit(&NumUnit, 20, map_height / 2, 'M', MONSTER, units);
    addnewUnit(&NumUnit, 40, map_height / 2, 'M', MONSTER, units);
    addnewUnit(&NumUnit, 60, map_height / 2, 'M', MONSTER, units);
    addnewUnit(&NumUnit, 80, map_height / 2, 'M', MONSTER, units);
    addnewUnit(&NumUnit, 100, map_height / 2, 'M', MONSTER, units);
    addnewUnit(&NumUnit, 120, map_height / 2, 'M', MONSTER, units);
    addnewUnit(&NumUnit, 140, map_height / 2, 'M', MONSTER, units);
    addnewUnit(&NumUnit, 160, map_height / 2, 'M', MONSTER, units);
    addnewUnit(&NumUnit, 180, map_height / 2, 'M', MONSTER, units);
    addnewUnit(&NumUnit, 200, map_height / 2, 'M', MONSTER, units);
    Camera = cameraSet(units, Camera, NumUnit);
    int cnt = 1;

    while (1)
    {
        playerMoving(units);
        if (cnt % (refrashRate / 5) == 0) monsterMoving(units, NumUnit);
        //if (cnt % (refrashRate/2)== 0) gravityMoving(units);
        Camera = cameraSet(units, Camera, NumUnit);
        clear = Check(NumUnit, units);
        if (clear != 0)
        {
            if (clear == 2)
            {
                gotoxy(camera_width_range - 10, camera_height_range);
                printf("클리어를 축하드립니다!");
                break;
            }
            else
            {
                break;
            }
        }
        cnt++;
        //gotoxy(0, 12);
        //printf("%d\n", cnt);
        //gotoxy(40, 10);
        //printf("안내창");
        Sleep(1000 / refrashRate);
    }
    free(units);
    return clear;
}

int main() {
    setcursortype(NOCURSOR);
    int selection, clear;
    while (1)
    {
        selection = mainmenu();
        clearScreen();
        if (selection == 1) clear = playgame();
        else break;

        clearScreen();
        if (clear == 1)
        {
            gotoxy(camera_width_range - 4, 0);
            printf("클리어 했습니다");
        }
        else
        {
            gotoxy(camera_width_range - 3, 0);
            printf("실패했습니다");
        }
    }
    return 0;
}
