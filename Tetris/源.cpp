#include <graphics.h>//图形界面库
#include <conio.h>
#include <time.h>
#include<stdio.h>
#include<stdlib.h>
const int width = 10;
const int height = 15;
const int unit = 20;
int rank,pattern;
int score=0, max_score;
TCHAR max_s[5];
TCHAR s[5];
TCHAR Rank[5];
enum control//枚举 旋转，向左，向右，向下，下沉，暂停，退出等功能
{
	control_rotate,
	control_left, control_right, control_down,control_sink,
	control_pause,
	control_quit
};

enum draw//枚举对方块的三种显示操作
{
	show,//显示方块
	clear,//消除
	fix//固定
};
struct block
{
	WORD dir[4];//方块的四种状态
	COLORREF color;//方块颜色
}blocks[7] = { { 0x0F00, 0x4444, 0x0F00, 0x4444, RED },
{ 0x0660, 0x0660, 0x0660, 0x0660, BLUE },
{ 0x4460, 0x02E0, 0x0622, 0x0740, MAGENTA },
{ 0x2260, 0x0E20, 0x0644, 0x0470, YELLOW },
{ 0x0C60, 0x2640, 0x0C60, 0x2640, CYAN },
{ 0x0360, 0x4620, 0x0360, 0x4620, GREEN },
{ 0x4E00, 0x4C40, 0x0E40, 0x4640, BROWN } };
struct block_info
{
	unsigned char id;//方块id，给每个方块固定的编号
	char x, y;//方块坐标
	unsigned char dir : 2;//方向
}cur_block, next_block;
unsigned char world[width][height] = { 0 };
void init();//初始化界面
void quit();
void newgame();
void gameover();
control get_char();//获取控制命令
void dispatch_control(control _control);//执行各个控制命令
void newblock();//生成新方块
bool check_block(block_info _block);//检测方块是否可以放下
void draw_unit(int x, int y, COLORREF c, draw _draw);//画单元方块
void draw_block(block_info _block, draw _draw = show);//画方块
void rotate();
void left();
void right();
void down();
void sink(int &score);//下沉方块
void main()
{
	score = 0;
	printf("\n\n\n\n\n\n\n\n				");
	printf("请输入数字1~10来选择游戏难度，数字越大方块下落速度越快\n");
	printf("				  	  	请选择游戏难度：");
	scanf("%d", &rank);
	printf("\n\n\n\n				  	  请输入数字1或2来选择游戏模式\n");
	printf("\n\n				    在方块直接降落模式中按G键方块可直接沉底\n");
	printf("				  	    	1:方块逐行降落，\n			  	  	    	2:方块直接降落\n			  			   游戏模式:");
	scanf("%d", &pattern);
	switch (rank) {
	case 1: {FILE *fp; fp = fopen("max_score1.txt", "r"); fscanf(fp, "%d", &max_score); }break;
	case 2: {FILE *fp; fp = fopen("max_score2.txt", "r"); fscanf(fp, "%d", &max_score); }break;
	case 3: {FILE *fp; fp = fopen("max_score3.txt", "r"); fscanf(fp, "%d", &max_score); }break;
	case 4: {FILE *fp; fp = fopen("max_score4.txt", "r"); fscanf(fp, "%d", &max_score); }break;
	case 5: {FILE *fp; fp = fopen("max_score5.txt", "r"); fscanf(fp, "%d", &max_score); }break;
	case 6: {FILE *fp; fp = fopen("max_score6.txt", "r"); fscanf(fp, "%d", &max_score); }break;
	case 7: {FILE *fp; fp = fopen("max_score7.txt", "r"); fscanf(fp, "%d", &max_score); }break;
	case 8: {FILE *fp; fp = fopen("max_score8.txt", "r"); fscanf(fp, "%d", &max_score); }break;
	case 9: {FILE *fp; fp = fopen("max_score9.txt", "r"); fscanf(fp, "%d", &max_score); }break;
	case 10: {FILE *fp; fp = fopen("max_score10.txt", "r"); fscanf(fp, "%d", &max_score); }break;//读取最高分
	}
	init();
	control c;
	while (true)
	{
		c = get_char();
		dispatch_control(c);
		if (c == control_quit)
		{
			HWND wnd = GetHWnd(); //获取窗口的句柄 ，为了实现暂停和退出功能
			if (MessageBox(wnd, _T("退出游戏将丢失您的游戏进度，您确认要退出游戏吗？"), _T("提醒"), MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
				quit();
		}
		if (c == control_pause)
		{
			HWND wnd = GetHWnd();
			if (MessageBox(wnd, _T("点击确认继续游戏"), _T("已暂停"),MB_ICONQUESTION) != IDOK)
				;
		}
	}
}
void init()//初始化界面
{
	initgraph(640, 480);
	srand((unsigned)time(NULL));
	setbkmode(TRANSPARENT);//设置图案填充的背景色为透明
	settextstyle(14, 0, _T("宋体"));
	outtextxy(20, 330, _T("操作说明"));
	outtextxy(20, 350, _T("上：旋转"));
	outtextxy(20, 370, _T("左：左移"));
	outtextxy(20, 390, _T("右：右移"));
	outtextxy(20, 410, _T("下：下移"));
	outtextxy(20, 430, _T("空格：暂停"));
	outtextxy(20, 450, _T("ESC：退出"));
	outtextxy(450, 200, _T("等级："));
	outtextxy(450, 300, _T("最高得分："));
	outtextxy(450, 400, _T("当前得分："));
	_stprintf(max_s, _T("%d"), max_score);//输出score数值，先将数字格式化输出为字符串
	_stprintf(s, _T("%d"), score);
	_stprintf(Rank, _T("%d"), rank);
	outtextxy(530, 300, max_s);
	outtextxy(530, 400, s);
	outtextxy(530, 200,Rank);
	setorigin(220, 20);
	rectangle(-1,-1, width * unit, height * unit);
	rectangle((width + 1) * unit - 1, -1, (width + 5) * unit, 4 * unit);
	newgame();//开始游戏
}
void quit()
{
	closegraph();//关闭图形界面
	exit(0);//关闭命令窗口
}
void newgame()
{
	//清空游戏区
	setfillcolor(BLACK);
	solidrectangle(0, 0, width * unit - 1, height * unit - 1);
	ZeroMemory(world, width*height);
	// 生成下一个方块
	next_block.id = rand() % 7;
	next_block.dir = rand() % 4;
	next_block.x = 11;
	next_block.y = 14;
	newblock();
}
void gameover()
{
	if (score > max_score) {
		max_score = score;
		switch (rank) {
		case 1: {FILE *fp; fp = fopen("max_score1.txt", "w"); fprintf(fp, "%d", max_score); }break;
		case 2: {FILE *fp; fp = fopen("max_score2.txt", "w"); fprintf(fp, "%d", max_score); }break;
		case 3: {FILE *fp; fp = fopen("max_score3.txt", "w"); fprintf(fp, "%d", max_score); }break;
		case 4: {FILE *fp; fp = fopen("max_score4.txt", "w"); fprintf(fp, "%d", max_score); }break;
		case 5: {FILE *fp; fp = fopen("max_score5.txt", "w"); fprintf(fp, "%d", max_score); }break;
		case 6: {FILE *fp; fp = fopen("max_score6.txt", "w"); fprintf(fp, "%d", max_score); }break;
		case 7: {FILE *fp; fp = fopen("max_score7.txt", "w"); fprintf(fp, "%d", max_score); }break;
		case 8: {FILE *fp; fp = fopen("max_score8.txt", "w"); fprintf(fp, "%d", max_score); }break;
		case 9: {FILE *fp; fp = fopen("max_score9.txt", "w"); fprintf(fp, "%d", max_score); }break;
		case 10: {FILE *fp; fp = fopen("max_score10.txt", "w"); fprintf(fp, "%d", max_score); }break;//如果当前分数高于最高分，修改最高分
		}
		_stprintf(max_s, _T("%d"), max_score);
		clearrectangle(310, 280, 330, 320);
		outtextxy(310, 280, max_s);
		newgame();
	}

	HWND wnd = GetHWnd(); 
	if (MessageBox(wnd, _T("游戏结束。\n您想重新来一局吗？"), _T("游戏结束"), MB_YESNO | MB_ICONQUESTION) == IDYES) {
		score = 0;
		_stprintf(s, _T("%d"), score);
		clearrectangle(310, 380, 330, 420);
		outtextxy(310, 380, s);
		newgame();
	}
	else
		quit();
}
DWORD m_oldtime;//typedef unsigned long DWORD
control get_char()//获取控制命令
{
	while (true)
	{
	
		DWORD newtime = GetTickCount();//返回从操作系统启动所经过的毫秒数，为了控制下落速度
		if (newtime - m_oldtime >= 500/rank)
		{
			down();
			m_oldtime = newtime;
		}
		if (kbhit())//检查当前是否有键盘输入，若有则返回一个非0值，否则返回0
		{
			if (pattern == 1) {
				switch (getch())
				{
				case 'w':
				case 'W':return control_rotate;
				case 'a':
				case 'A':return control_left;
				case 'd':
				case 'D':return control_right;
				case 's':
				case 'S':return control_down;
				case 27:return control_quit;
				case ' ':return control_pause;
				case 'G':
				case'g':return control_sink;
				case 0:
				case 0xE0:
					switch (getch())
					{
					case 72:return control_rotate;
					case 75:return control_left;
					case 77:return control_right;
					case 80:return control_down;
					}
				}
			}
			else if (pattern == 2) {
				switch (getch())
				{
				case 'w':
				case 'W':return control_rotate;
				case 'a':
				case 'A':return control_left;
				case 'd':
				case 'D':return control_right;
				case 's':
				case 'S':return control_sink;
				case 27:return control_quit;
				case ' ':return control_pause;
				case 0:
				case 0xE0:
					switch (getch())
					{
					case 72:return control_rotate;
					case 75:return control_left;
					case 77:return control_right;
					case 80:return control_sink;;
					}
				}
			}
		}
	}
}
void dispatch_control(control _control)
{
	switch (_control)
	{
	case control_rotate:rotate(); break;
	case control_left:left(); break;
	case control_right:right(); break;
	case control_down:down(); break;
	case control_sink:sink(score); break;
	case control_quit:break;
	}
}
void newblock()
{
	cur_block.id = next_block.id, next_block.id = rand() % 7;
	cur_block.dir = next_block.dir, next_block.dir = rand() % 4;
	cur_block.x = (width - 4) / 2;
	cur_block.y = height + 2;//方块降落位置
	// 下移新方块直到有局部显示
	WORD c = blocks[cur_block.id].dir[cur_block.dir];
	while ((c & 0xF) == 0)//1111
	{
		cur_block.y--;
		c >>= 4;//右移四位
	}
	draw_block(cur_block);
	setfillcolor(BLACK);
	solidrectangle((width + 1) * unit, 0, (width + 5) * unit - 1, 4 * unit - 1);
	draw_block(next_block);
	m_oldtime = GetTickCount();//计时器，判断自动下落
}
void draw_unit(int x, int y, COLORREF c, draw _draw)
{
	// 计算单元方块对应的屏幕坐标
	int left = x * unit;
	int top = (height - y - 1) * unit;
	int right = (x + 1) * unit - 1;
	int bottom = (height - y) * unit - 1;
	switch (_draw)
	{
	case show:
		// 画普通方块
		setlinecolor(0x006060);
		roundrect(left + 1, top + 1, right - 1, bottom - 1, 5, 5);
		setlinecolor(0x003030);
		roundrect(left, top, right, bottom, 8, 8);
		setfillcolor(c);
		setlinecolor(LIGHTGRAY);
		fillrectangle(left + 2, top + 2, right - 2, bottom - 2);
		break;
	case fix:
		//画固定的方块
		setfillcolor(RGB(GetRValue(c) * 2 / 3, GetGValue(c) * 2 / 3, GetBValue(c) * 2 / 3));
		setlinecolor(DARKGRAY);
		fillrectangle(left + 1, top + 1, right - 1, bottom - 1);
		break;
	case clear:
		//消除方块
		setfillcolor(BLACK);
		solidrectangle(x * unit, (height - y - 1) * unit, (x + 1) * unit - 1, (height - y) * unit - 1);
		break;
	}
}
void draw_block(block_info _block, draw _draw)//画方块
{
	WORD b = blocks[_block.id].dir[_block.dir];
	int x, y;

	for (int i = 0; i < 16; i++, b <<= 1)
		if (b & 0x8000)//1000 0000 0000 0000   如果第一位是1，则画出对应位置的方块
		{
			x = _block.x + i % 4;
			y = _block.y - i / 4;
			if (y < height)
				draw_unit(x, y, blocks[_block.id].color, _draw);
		}
}
bool check_block(block_info _block)
{
	WORD b = blocks[_block.id].dir[_block.dir];
	int x, y;

	for (int i = 0; i < 16; i++, b <<= 1)
		if (b & 0x8000)
		{
			x = _block.x + i % 4;
			y = _block.y - i / 4;
			if ((x < 0) || (x >= width) || (y < 0))
				return false;

			if ((y < height) && (world[x][y]))//检测游戏是否结束
				return false;
		}

	return true;
}
void rotate()//旋转
{
	int dx;
	block_info tmp = cur_block;
	tmp.dir++; if (check_block(tmp)) { dx = 0; goto rotate; }//首先检测方块是否可以进行旋转操作
	tmp.x = cur_block.x - 1; if (check_block(tmp)) { dx = -1; goto rotate; }
	tmp.x = cur_block.x + 1; if (check_block(tmp)) { dx = 1; goto rotate; }
	tmp.x = cur_block.x - 2; if (check_block(tmp)) { dx = -2; goto rotate; }
	tmp.x = cur_block.x + 2; if (check_block(tmp)) { dx = 2; goto rotate; }
	return;
rotate:
	draw_block(cur_block, clear);
	cur_block.dir++;
	cur_block.x += dx;
	draw_block(cur_block);

}
void left()//左移
{
	block_info tmp = cur_block;
	tmp.x--;
	if (check_block(tmp))
	{
		draw_block(cur_block, clear);
		cur_block.x--;
		draw_block(cur_block);
	}
}
void right()//右移
{
	block_info tmp = cur_block;
	tmp.x++;
	if (check_block(tmp))
	{
		draw_block(cur_block, clear);
		cur_block.x++;
		draw_block(cur_block);
	}
}
void down()//下降
{
	block_info tmp = cur_block;
	tmp.y--;
	if (check_block(tmp))
	{
		draw_block(cur_block, clear);
		cur_block.y--;
		draw_block(cur_block);
	}
	else
		sink(score);//如果已经无法下降，则进行一次固定操作
}
void sink(int &score)//下沉，由于此步骤会对分数进行操作，所以把分数设置为参数传入函数
{
	int i, x, y;
	draw_block(cur_block, clear);
	block_info tmp = cur_block;
	tmp.y--;
	while (check_block(tmp))//如果还为沉底，直接沉底固定
	{
		cur_block.y--;
		tmp.y--;
	}
	draw_block(cur_block, fix);
	WORD b = blocks[cur_block.id].dir[cur_block.dir];
	for (i = 0; i < 16; i++, b <<= 1)
		if (b & 0x8000)
		{
			if (cur_block.y - i / 4 >= height)
			{
				gameover();//如果方块的固定位置超出高度，结束游戏
				return;
			}
			else
				world[cur_block.x + i % 4][cur_block.y - i / 4] = 1;
		}// 检查是否需要消掉行，并标记
	unsigned char remove = 0;//前十二位来储存方块，低 4 位用来标记方块涉及的 4 行是否有消除行为
	int score_rank = 0;
	for (y = cur_block.y; y >= max(cur_block.y - 3, 0); y--)
	{
		i = 0;
		for (x = 0; x < width; x++)
			if (world[x][y] == 1)
				i++;

		if (i == width)
		{
			remove = remove|(1 << (cur_block.y - y));
			setfillcolor(LIGHTGREEN);
			setlinecolor(LIGHTGREEN);
			setfillstyle(BS_HATCHED, HS_DIAGCROSS);
			fillrectangle(0, (height - y - 1) * unit + unit / 2 - 5, width * unit - 1, (height - y - 1) * unit + unit / 2 + 5);
			setfillstyle(BS_SOLID);//消除的动画效果
			score_rank++;//统计分数
		}
	}
	if (remove)//如果产生整行消除
	{
		setbkmode(TRANSPARENT);
		settextstyle(14, 0, _T("宋体"));
		score = score+score_rank*rank;
		TCHAR s[5];
		_stprintf(s, _T("%d"), score);//输出score数值，先将数字格式化输出为字符串
		clearrectangle(310,380,330,420);
		outtextxy(310, 380, s);

		Sleep(300);
		IMAGE img;
		//擦掉之前被标记的行
		for (i = 0; i < 4; i++, remove >>= 1)//逐行遍历，有几行满了就消除几行
		{
			if (remove & 1)
			{
				for (y = cur_block.y - i + 1; y < height; y++)
					for (x = 0; x < width; x++)
					{
						world[x][y - 1] = world[x][y];
						world[x][y] = 0;
					}
				getimage(&img, 0, 0, width * unit, (height - (cur_block.y - i + 1)) * unit);
				putimage(0, unit, &img);
			}
		}
	}
	newblock();
}