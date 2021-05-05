#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<string.h>
#include<Windows.h>
#include<math.h>


//���� ��ǥ������ 1���� ���������
// ���� ��ǥ�� 0�� �ִ� + 1�� ������
const int map_height = 50;
const int map_width = 230;
const int refrashRate = 50;
const int camera_height_range = 15;
const int camera_width_range = 50;

typedef enum {
    PLAYER, MONSTER
} UNIT_TYPE;

//Ŀ�� ���ſ� �ڵ�
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



//��ֹ�, ����, �÷��̾ ���������� ���� �ڷ��� unit ����
//�� ������ ��ǥ, ����
struct unit {
    int x;
    int y;
    int BeforeRelativeX = -1;
    int BeforeRelativeY = -1;
    char look;
    UNIT_TYPE type;
};

//ī�޶� ȥ�ڸ� ���� �ڷ��� camera ����
//ī�޶��� x��ǥ, y��ǥ, ���� �����¿� ����
struct camera {
    int x = 0;
    int y = 0;
    char wallRL = 'N';
    char wallUD = 'N';
}Camera;

//�Է� ��ǥ�� x,y�� �̵���Ű�� �Լ�
void gotoxy(int x, int y)
{
    COORD CursorPosition = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), CursorPosition);
}

//ī�޶��� �߽ɿ� ���� �ֺ� ȯ���� ����ϴ� �Լ�
//ī�޶� return��
camera cameraSet(unit* units, camera Camera, int NumUnit)
{
    int i;
    //�÷��̾��� ��ǥ�� ī�޶��� ��ǥ�� ����
    Camera.x = units->x;
    Camera.y = units->y;


    //�� �糡�� �������� ��� ī�޶� ���̻� �̵������ʵ��� ��ǥ�� �����ϰ�, WallRL ������ �����ϴ� �ڵ�
    //���� WallRL�� �ٸ���쿡�� ���� ����
    //x��ǥ�� ī�޶� �������� ������� ���ʿ� �����ִٴ� ��
    if (Camera.x <= camera_width_range)
    {
        Camera.x = camera_width_range;
        //if�� : ������ �����ʿ� ���� �ۼ��Ǿ��־��� ��� �����
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
    //�� ���Ʒ��� �������� ��� ī�޶� ���̻� �̵������ʵ��� ��ǥ�� �����ϰ�, WallUD ������ �����ϴ� �ڵ�
    // //���� WallUD�� �ٸ���쿡�� ���� ����
    //y��ǥ�� ī�޶� �������� ������� ���ʿ� ���� �ִٴ� ��
    if (Camera.y <= camera_height_range)
    {
        Camera.y = camera_height_range;
        //if�� : ������ �Ʒ��ʿ� ���� �ۼ��Ǿ��־��� ��� �����
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
    //units�� ����� �� ���ֵ��� ��� �����Ͽ� ī�޶��� ���� ���� �ش�Ǵ� ������ �����Ͽ�, ȭ�� ���� ����ϴ� �ڵ�
    //ī�޶� ��ǥ�� ���� �� ������ �����ǥ�� ���ϰ� �����ǥ�� ��ȭ���� �ÿ��� ������ �� ����Ѵ�
    //�����ǥ��
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


    //����� WallUD�� WallRL�� ���� ���� �ۼ��ϴ� �ڵ�
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

//�÷��̾��� �̵��� ���ؼ� wasd�� �Է¹޴� �Լ�, �̿� �Է¹����� NULL ���
char getKey()
{
    if (_kbhit()) {
        return _getch();
    }
    return NULL;
}

//�߷¿� ���� �÷��̾� �̵���Ű�� �Լ�
void gravityMoving(unit* unit)
{
    if (unit->y < map_height) unit->y++;
}

//wasd�Է¹޾Ƽ� �÷��̾��� ��ǥ�� �̵���Ű�� �Լ�
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

//������ AI�� ���� ���Ͱ� �̵��ϴ� �Լ�
void monsterMoving(unit* units, int NumUnit)
{
    for (int i = 1; i < NumUnit; i++)
    {
        unit* monster = units + i;
        if (monster->type == MONSTER)
        {
            int random = rand() % 4;
            switch (random)
            {
            case 0:
                if (monster->x < map_width)
                    (monster->x)++;
                break;
            case 1:
                if (monster->y < map_height)
                    (monster->y)++;
                break;
            case 2:
                if (monster->x > 1)
                    (monster->x)--;
                break;
            case 3:
                if (monster->y > 1)
                    (monster->y)--;
                break;
            }
        }
    }
}

//���ο������� �߰��ϴ� �Լ�. �÷��̾� �Ǵ� ����
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

int main() {
    int NumUnit = 0;
    setcursortype(NOCURSOR);
    unit* units = (unit*)malloc(sizeof(unit) * 30);

    addnewUnit(&NumUnit, 1, map_height, 'K', PLAYER, units);
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
        if ((cnt % refrashRate) == 0) monsterMoving(units, NumUnit);
        if (cnt % (refrashRate / 2) == 0) gravityMoving(units);
        Camera = cameraSet(units, Camera, NumUnit);
        cnt++;
        gotoxy(1, 1);
        printf("%d", cnt);
        //gotoxy(40, 10);
        //printf("�ȳ�â");
        Sleep(1000 / refrashRate);
    }
    free(units);
    return 0;
}