#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>

#define ROW 22 
#define COL 42 

#define KONG 0 //标记空
#define WALL 1 //标记墙
#define FOOD 2 //标记食物
#define HEAD 3 //标记蛇头
#define BODY 4 //标记蛇身

#define UP 72 //方向键：上
#define DOWN 80 //方向键：下
#define LEFT 75 //方向键：左
#define RIGHT 77 //方向键：右
#define SPACE 32 //暂停
#define ESC 27 //退出

//蛇头
struct Snake
{
	int len; //记录蛇身长度
	int x; //蛇头横坐标
	int y; //蛇头纵坐标
}snake;

//蛇身
struct Body
{
	int x; //蛇身横坐标
	int y; //蛇身纵坐标
}body[ROW * COL]; //开辟足以存储蛇身的结构体数组

int face[ROW][COL]; //标记游戏区各个位置的状态

//隐藏光标
void HideCursor();
//光标跳转
void CursorJump(int x, int y);
//初始化界面
void InitInterface();
//颜色设置
void color(int c);
//从文件读取最高分
void ReadFromFile();
//更新最高分到文件
void WriteToFile();
//初始化蛇
void InitSnake();
//随机生成食物
void RandFood();
//判断得分与结束
void JudgeFunc(int x, int y);
//打印蛇与覆盖蛇
void DrawSnake(int flag);
//移动蛇
void MoveSnake(int x, int y);
//执行按键
void run(int x, int y);
//游戏主体逻辑函数
void Game();

int max, grade; //全局变量
int main()
{
#pragma warning (disable:4996) //消除警告
	max = 0, grade = 0; //初始化变量
	system("title 贪吃蛇"); //设置cmd窗口的名字
	system("mode con cols=84 lines=23"); //设置cmd窗口的大小
	HideCursor(); //隐藏光标
	ReadFromFile(); //从文件读取最高分到max变量
	InitInterface(); //初始化界面
	InitSnake(); //初始化蛇
	srand((unsigned int)time(NULL)); //设置随机数生成起点
	RandFood(); //随机生成食物
	DrawSnake(1); //打印蛇
	Game(); //开始游戏
	return 0;
}

//隐藏光标
void HideCursor()
{
	CONSOLE_CURSOR_INFO curInfo; //光标信息的结构体，包括dwsize
	curInfo.dwSize = 1; 
	curInfo.bVisible = FALSE; 
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE); //获取控制台句柄
	SetConsoleCursorInfo(handle, &curInfo); //设置光标信息
}
//光标跳转到(x,y)的位置
void CursorJump(int x, int y)
{
	COORD pos; //定义光标位置的结构体变量
	pos.X = x; //横坐标
	pos.Y = y; //纵坐标
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE); //获取控制台句柄
	SetConsoleCursorPosition(handle, pos); //设置光标位置
}

//初始化界面
void InitInterface()
{
	color(6); //颜色设置为黄色
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			if (j == 0 || j == COL - 1)
			{
				face[i][j] = WALL; 
				CursorJump(2 * j, i);//逻辑坐标 (j, i)游戏内部的数据结构坐标
									//屏幕坐标(2 * j, i)：实际在控制台显示的位置
				printf("■");
			}
			else if (i == 0 || i == ROW - 1)
			{
				face[i][j] = WALL; 
				CursorJump(2 * j, i);
				printf("■");
			}
			else
			{
				face[i][j] = KONG;
			}
		}
	}
	color(7); //颜色设置为白色
	CursorJump(0, ROW);
	printf("当前得分:%d", grade);
	CursorJump(COL, ROW);
	printf("最高得分:%d", max);
}

//初始化蛇
void InitSnake()
{
	snake.len = 3;
	snake.x = COL / 2;
	snake.y = ROW / 2;
	body[0].x = COL / 2 - 1;
	body[0].y = ROW / 2;
	body[1].x = COL / 2 - 2;
	body[1].y = ROW / 2;
	body[2].x = COL / 2 - 2;
	body[2].y = ROW / 2;
	face[snake.y][snake.x] = HEAD;
	face[body[0].y][body[0].x] = BODY;
	face[body[1].y][body[1].x] = BODY;
	face[body[2].y][body[2].x] = BODY;
}


//颜色设置
void color(int c)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c); 
}

//从文件读取最高分
void ReadFromFile()
{
	FILE* pf = fopen("max.txt", "r"); 
	fseek(pf, 0, SEEK_SET); 
	fread(&max, sizeof(int), 1, pf); 
	fclose(pf); 
	pf = NULL;
}

//更新最高分到文件
void WriteToFile()
{
	if(max<grade){
		max = grade;
		FILE * pf = fopen("max.txt", "w");
		if (pf == NULL)
		{
			printf("保存最高得分记录失败\n");
		}
		fwrite(&grade, sizeof(int), 1, pf);
		fclose(pf);
		pf = NULL;
	}
}


//随机生成食物
void RandFood()
{
	int i, j;
	do
	{
		i = rand() % ROW;
		j = rand() % COL;
	} while (face[i][j] != KONG); 
	face[i][j] = FOOD; 
	color(12); 
	CursorJump(2 * j, i); 
	printf("★");
}

//判断得分与结束
void JudgeFunc(int x, int y)
{
	if (face[snake.y + y][snake.x + x] == FOOD)
	{
		snake.len++; 
		grade = grade + 10; 
		color(7); //颜色设置为白色
		CursorJump(0, ROW);
		printf("当前得分:%d", grade); 
		RandFood(); 
	}
	else if (face[snake.y + y][snake.x + x] == WALL || face[snake.y + y][snake.x + x] == BODY)
	{

		system("cls"); 
		color(7); 
		CursorJump(2 * (COL / 3), ROW / 2 - 3);
			printf("你的最终成绩为%d", grade);
		WriteToFile();	
		CursorJump(2 * (COL / 3), ROW / 2 );
			printf("你的最高成绩为%d", max);
		while (1) 
		{
			char ch;
			CursorJump(2 * (COL / 3), ROW / 2+3 );
			printf("按1再来一局****按2退出游戏");
			scanf("%c", &ch);
			if (ch == '1' )
			{
				system("cls");
				main();
			}
			else if (ch == '2')
			{
				CursorJump(2 * (COL / 3), ROW / 2 + 5);
				exit(0);
			}
		}
	}
}

//打印蛇与覆盖蛇
void DrawSnake(int flag)
{
	if (flag == 1) 
	{
		color(10); //颜色设置为绿色
		CursorJump(2 * snake.x, snake.y);
		printf("●"); 
		for (int i = 0; i < snake.len; i++)
		{
			CursorJump(2 * body[i].x, body[i].y);
			printf("○"); 
		}
	}
	else //覆盖蛇
	{
		if (body[snake.len - 1].x != 0) 
		{
			CursorJump(2 * body[snake.len - 1].x, body[snake.len - 1].y);
			printf("  ");
		}
	}
}
//移动蛇
void MoveSnake(int x, int y)
{
	DrawSnake(0); 
	face[body[snake.len - 1].y][body[snake.len - 1].x] = KONG; 
	face[snake.y][snake.x] = BODY; 
	//蛇移动后各个蛇身位置坐标需要更新
	for (int i = snake.len - 1; i > 0; i--)
	{
		body[i].x = body[i - 1].x;
		body[i].y = body[i - 1].y;
	}
	//蛇移动后蛇头位置信息变为第0个蛇身的位置信息
	body[0].x = snake.x;
	body[0].y = snake.y;
	//蛇头的位置更改
	snake.x = snake.x + x;
	snake.y = snake.y + y;
	DrawSnake(1); 
}
//执行按键
void run(int x, int y)
{
	int t = 0;
	while (1)
	{
		if (t == 0)
			t = 5000; //这里t越小，蛇移动速度越快（可以根据次设置游戏难度）
		while (--t)
		{
			if (kbhit() != 0) //若键盘被敲击，则退出循环
				break;
		}
		if (t == 0) //键盘未被敲击
		{
			JudgeFunc(x, y); //判断到达该位置后，是否得分与游戏结束
			MoveSnake(x, y); //移动蛇
		}
		else //键盘被敲击
		{
			break; 
		}
	}
}
//游戏函数
void Game()
{
	int n = RIGHT; //开始游戏时，默认向后移动
	int tmp = 0; //记录蛇的移动方向
	goto first; //第一次进入循环先向默认方向前进
	while (1)
	{
		n = getch(); //读取键值
		//在执行前，需要对所读取的按键进行调整
		switch (n)
		{
		case UP:
		case DOWN: 
			if (tmp != LEFT && tmp != RIGHT) 
			{
				n = tmp; 
			}
			break;
		case LEFT:
		case RIGHT: 
			if (tmp != UP && tmp != DOWN) 
			{
				n = tmp; 
			}
		case SPACE:
		case ESC:
		case 'r':
		case 'R':
			break; //这四个无需调整
		default:
			n = tmp; //其他键无效，默认为上一次蛇移动的方向
			break;
		}
	first: //第一次进入循环先向默认方向前进
		switch (n)
		{
		case UP: //方向键：上
			run(0, -1); //向上移动（横坐标偏移为0，纵坐标偏移为-1）
			tmp = UP; //记录当前蛇的移动方向
			break;
		case DOWN: //方向键：下
			run(0, 1); //向下移动（横坐标偏移为0，纵坐标偏移为1）
			tmp = DOWN; //记录当前蛇的移动方向
			break;
		case LEFT: //方向键：左
			run(-1, 0); //向左移动（横坐标偏移为-1，纵坐标偏移为0）
			tmp = LEFT; //记录当前蛇的移动方向
			break;
		case RIGHT: //方向键：右
			run(1, 0); //向右移动（横坐标偏移为1，纵坐标偏移为0）
			tmp = RIGHT; //记录当前蛇的移动方向
			break;
		case SPACE: //暂停
			system("pause>nul"); //暂停后按任意键继续
			break;
		case ESC: //退出
			system("cls"); 
			color(7); 
			CursorJump(COL - 8, ROW / 2);
			printf("  游戏结束  ");
			CursorJump(COL - 8, ROW / 2 + 2);
			exit(0);
		case 'r':
		case 'R': //重新开始
			system("cls"); 
			main(); //重新执行主函数
		}
	}
}