#include<graphics.h>
#include<conio.h>
#include<stdio.h>
#include<time.h>
#include<easyx.h>
#include <windows.h>				// 引用 Windows Multimedia API
#pragma comment(lib,"Winmm.lib")

/*==========================宏============================*/
#define L 700						//窗口大小700×700
#define L_START 28					//棋盘长度首坐标
#define L_END 	672					//棋盘长度末坐标
#define WIDTH 45					//线间距
#define rt_BLACK 1					//黑棋状态量为1 
#define rt_WHITE 2					//白棋状态量为2
/*========================全局变量========================*/
struct coordinate					//定义存放点坐标的结构类型
{
	int x;							//x,y为棋子坐标
	int y;
	int rt;							//棋子颜色
	int score;						//该位置的价值
};
static struct coordinate crossing[15][15];	//定义存放点坐标的结构二维数组
static struct coordinate crossed[15][15];	//保存已经该步节点
int num = 0;								//步数
int a, b;									//坐标在数组里面的下标
MOUSEMSG m;
FILE* fp = NULL;

void test();//测试函数无意义
void ready();//准备开始界面
void choicecolor();//选择所用颜色
void choiceb();//选择黑子开始
void choicew();//选择白子开始
void definecossing();//定义并保存落子位置，即交叉点
int get_mouse_area();//判断鼠标位置，返回鼠标状态，（是否单击）
void start();//开始下棋
int judge(int color);//判断输赢
void down_white(int x,int y);//落子白子
void down_black(int x,int y);//落黑子
int value_num(int x, int y, int color, int dirction);//计算在指定方向上的价值
void value_one(int x,int y,int color);//单个位置价值判断
int value_count(int chessnum,int position);
void value_whole();//判断并写入棋盘落子价值
void win_b();//黑棋胜利
void win_w();//白棋胜利
void flsh();//逐渐变暗效果
void beginorend();//结束或者重开
void end();//结束

void value_one(int x, int y, int color) {			//单个位置价值判断
	if (crossing[x][y].rt != 0) {
		return ;
	}else{
		int values,value1, value2, value3, value4;
		value1 = value_num(x, y, color, 1);
		value2 = value_num(x, y, color, 2);
		value3 = value_num(x, y, color, 3);
		value4 = value_num(x, y, color, 4);
		values = value1 + value2 + value3 + value4;
		crossing[x][y].score = values;
	}
}
int value_count(int chessnum, int position) {
	int chess_value = 0;
	if (position == 1) {
		switch (chessnum)
		{case 0 :
			chess_value += 1;
			break;
		case 1:
			chess_value += 10;
			break;
		case 2:
			chess_value += 100;
			break;
		case 3:
			chess_value += 1000;
			break;
		case 4:
			chess_value += 100000;
			break;
		}
	}else{
		switch (chessnum)
		{
		case 0:
			chess_value += 0;
			break;
		case 1:
			chess_value += 5;
			break;
		case 2:
			chess_value += 50;
			break;
		case 3:
			chess_value += 500;
			break;
		case 4:
			chess_value += 5000;
			break;
		}
	}
	return chess_value;
}
int value_num(int x, int y, int color, int dirction) {
	int uncolor;
	if (color == rt_BLACK) {
		uncolor = rt_WHITE;
	}
	else {
		uncolor = rt_BLACK;
	}
	int one_value = 0;//在指定方向上的价值
	int temp_value;
	int x0, y0, x1, y1, x2, y2;//x0,y0保存原始点位；x1，y1保存初始边界；x2，y2保存最终边界；
	int i;
	int l;				//规定方向最大距离
	int one_num;		//某个方向上的己方棋子数量
	l = 4; x0 = x; y0 = y;
	switch (dirction)	//该位置对我方而言的价值
	{
	case 1:	//上下方向  
	{
		x = x0; y = y0; l = 4;
		while (crossing[x][y - 1].rt != uncolor && y - 1 >= 0 && l > 0) {
			l--;
			y -= 1;
		}
		y1 = y;
		x = x0; y = y0; l = 4;
		while (crossing[x][y + 1].rt != uncolor && y + 1 <= 15 && l > 0) {
			l--;
			y += 1;
		}
		y2 = y;
		l = y2 - y1 + 1;
		if (l < 5) {
			one_value += 0;
		}
		else {
			for (i = 0; i < l - 4; i++) {
				one_num = 0;
				for (y = y1; y < y1 + 5; y++) {
					if (crossing[x][y].rt == color)one_num++;
				}

				if (one_num >= 4) {
					if (color == rt_BLACK) {
						down_black(x0, y0);
						win_b();
					}
					else if (color == rt_WHITE) {
						down_white(x0, y0);
						win_w();
					}
				}
				temp_value = value_count(one_num, 1);
				one_value += temp_value;
				y1++;
			}
		}
	}
	break;
	case 2://左右方向
	{
		x = x0; y = y0; l = 4;
		while (crossing[x - 1][y].rt != uncolor && x - 1 >= 0 && l > 0) {
			l--;
			x -= 1;
		}
		x1 = x;
		x = x0; y = y0; l = 4;
		while (crossing[x + 1][y].rt != uncolor && x + 1 <= 15 && l > 0) {
			l--;
			x += 1;
		}
		x2 = x;
		l = x2 - x1 + 1;
		if (l < 5) {
			one_value += 0;
		}
		else {
			for (i = 0; i < l - 4; i++) {
				one_num = 0;
				for (x = x1; x < x1 + 5; x++) {
					if (crossing[x][y].rt == color)one_num++;
				}
				if (one_num >= 4) {
					if (color == rt_BLACK) {
						down_black(x0, y0);
						win_b();
					}
					else if (color == rt_WHITE) {
						down_white(x0, y0);
						win_w();
					}
				}
				temp_value = value_count(one_num, 1);
				one_value += temp_value;

				x1++;
			}
		}
	}
	break;
	case 3://左上-右下方向
	{
		x = x0; y = y0; l = 4;
		while (crossing[x - 1][y - 1].rt != uncolor && x >= 0 && y >= 0 && l > 0) {
			l--;
			x--;
			y--;
		}
		x1 = x; y1 = y;
		x = x0; y = y0; l = 4;
		while (crossing[x + 1][y + 1].rt != uncolor && x <= 15 && y <= 15 && l > 0) {
			l--;
			x++;
			y++;
		}
		x2 = x; y2 = y;
		l = x2 - x1 + 1;
		if (l < 5) {
			one_value += 0;
		}
		else {
			for (i = 0; i < l - 4; i++) {
				one_num = 0;
				for (x = x1, y = y1; x < x1 + 5; x++, y++) {
					if (crossing[x][y].rt == color)one_num++;
				}
				if (one_num >= 4) {
					if (color == rt_BLACK) {
						down_black(x0, y0);
						win_b();
					}
					else if (color == rt_WHITE) {
						down_white(x0, y0);
						win_w();
					}
				}
				temp_value = value_count(one_num, 1);
				one_value += temp_value;

				x1++; y1++;
			}
		}
	}
	break;
	case 4://左下-右上方向
	{
		x = x0; y = y0; l = 4;
		while (crossing[x - 1][y + 1].rt != uncolor && x >= 0 && y <= 15 && l > 0) {
			l--;
			x--;
			y++;
		}
		x1 = x; y1 = y;
		x = x0; y = y0; l = 4;
		while (crossing[x + 1][y - 1].rt != uncolor && x >= 0 && y <= 15 && l > 0) {
			l--;
			x++;
			y--;
		}
		x2 = x; y2 = y;
		l = x2 - x1 + 1;
		if (l < 5) {
			one_value += 0;
		}
		else {
			for (i = 0; i < l - 4; i++) {
				one_num = 0;
				for (x = x1, y = y1; x < x1 + 5; x++, y--) {
					if (crossing[x][y].rt == color)one_num++;
				}
				if (one_num >= 4) {
					if (color == rt_BLACK) {
						down_black(x0, y0);
						win_b();
					}
					else if (color == rt_WHITE) {
						down_white(x0, y0);
						win_w();
					}
				}
				temp_value = value_count(one_num, 1);
				one_value += temp_value;

				x1++; y1--;
			}
		}
	}
	break;
	}
	switch (dirction) {		//该位置对对方而言的价值
	case 1: {
		x = x0; y = y0; l = 4;
		while (crossing[x][y - 1].rt != color && y - 1 >= 0 && l > 0) {
			l--;
			y -= 1;
		}
		y1 = y;
		x = x0; y = y0; l = 4;
		while (crossing[x][y + 1].rt != color && y + 1 <= 15 && l > 0) {
			l--;
			y += 1;
		}

		y2 = y;
		l = y2 - y1 + 1;
		if (l < 5) {
			one_value += 0;
		}
		else {
			for (i = 0; i < l - 4; i++) {
				one_num = 0;
				for (y = y1; y < y1 + 5; y++) {
					if (crossing[x][y].rt == uncolor)one_num++;
				}
				
				temp_value = value_count(one_num, 2);
				one_value += temp_value;

				y1++;
			}
		}
	}
		  break;
	case 2:
	{
		x = x0; y = y0; l = 4;
		while (crossing[x - 1][y].rt != color && x - 1 >= 0 && l > 0) {
			l--;
			x -= 1;
		}
		x1 = x;
		x = x0; y = y0; l = 4;
		while (crossing[x + 1][y].rt != color && x + 1 <= 15 && l > 0) {
			l--;
			x += 1;
		}
		x2 = x;
		l = x2 - x1 + 1;
		if (l < 5) {
			one_value += 0;
		}
		else {
			for (i = 0; i < l - 4; i++) {
				one_num = 0;
				for (x = x1; x < x1 + 5; x++) {
					if (crossing[x][y].rt == uncolor)one_num++;
				}
				
				temp_value = value_count(one_num, 2);
				one_value += temp_value;

				x1++;
			}
		}
	}
	break;
	case 3: {
		x = x0; y = y0; l = 4;
		while (crossing[x - 1][y - 1].rt != color && x >= 0 && y >= 0 && l > 0) {
			l--;
			x--;
			y--;
		}
		x1 = x; y1 = y;
		x = x0; y = y0; l = 4;
		while (crossing[x + 1][y + 1].rt != color && x <= 15 && y <= 15 && l > 0) {
			l--;
			x++;
			y++;
		}
		x2 = x; y2 = y;
		l = x2 - x1 + 1;
		if (l < 5) {
			one_value += 0;
		}
		else {
			for (i = 0; i < l - 4; i++) {
				one_num = 0;
				for (x = x1, y = y1; x < x1 + 5; x++, y++) {
					if (crossing[x][y].rt == uncolor)one_num++;
				}
				
				temp_value = value_count(one_num, 2);
				one_value += temp_value;

				x1++; y1++;
			}
		}
	}
		  break;
	case 4:
	{
		x = x0; y = y0; l = 4;
		while (crossing[x - 1][y + 1].rt != color && x >= 0 && y <= 15 && l > 0) {
			l--;
			x--;
			y++;
		}
		x1 = x; y1 = y;
		x = x0; y = y0; l = 4;
		while (crossing[x + 1][y - 1].rt != color && x <= 15 && y >= 0 && l > 0) {
			l--;
			x++;
			y--;
		}
		x2 = x; y2 = y;
		l = x2 - x1 + 1;
		if (l < 5) {
			one_value += 0;
		}
		else {
			for (i = 0; i < l - 4; i++) {
				one_num = 0;
				for (x = x1, y = y1; x < x1 + 5; x++, y--) {
					if (crossing[x][y].rt == uncolor)one_num++;
				}
				
				temp_value = value_count(one_num, 2);
				one_value += temp_value;

				x1++; y1--;
			}
		}
	}
	break;
	}
	return one_value;
}

void end() {
	Sleep(400);
	flsh();
	loadimage(NULL, _T("bg.png"), 700, 700, true);
	setbkmode(TRANSPARENT);							//设置显示字体样式
	settextcolor(BLACK);
	settextstyle(60, 40, _T("楷体"));
	outtextxy(80, 80, _T("人机对弈五子棋"));
	settextstyle(30, 20, _T("楷体"));
	outtextxy(440, 180, _T("——石贇"));
	settextstyle(25, 20, _T("楷体"));
	outtextxy(200, 450, _T("拜拜~欢迎下次再来呀"));
	Sleep(2000);
	exit(0);
	return;

}
void win_w() {
	Sleep(4000);
	m = GetMouseMsg();
	setbkcolor(RGB(125, 125, 125));			//灰色
	cleardevice();
	settextcolor(WHITE);
	settextstyle(90, 60, _T("楷体"));
	outtextxy(170, 305, _T("白棋胜"));
	beginorend();
}
void win_b() {
	Sleep(4000);
	m = GetMouseMsg();
	setbkcolor(RGB(125, 125, 125));			//灰色
	cleardevice();
	settextcolor(BLACK);
	settextstyle(90, 60, _T("楷体"));
	outtextxy(170, 305, _T("黑棋胜"));
	beginorend();
}
void beginorend() {
	fclose(fp);
	Sleep(1000);
	flsh();			//窗口逐渐变暗
	loadimage(NULL, _T("bg.png"), 700, 700, true);
	settextcolor(BLACK);

	settextstyle(60, 40, _T("楷体"));
	outtextxy(80, 80, _T("人机对弈五子棋"));
	settextstyle(30, 20, _T("楷体"));
	outtextxy(440, 180, _T("——石贇"));

	settextstyle(30, 20, _T("楷体"));
	outtextxy(220, 440, _T("再来一局"));
	outtextxy(220, 480, _T("不想玩了"));
	int mouse_set;
	while (true) {
		m = GetMouseMsg();
		mouse_set = 0;
		if (m.x >= 220 && m.x <= 455
			&&
			m.y >= 440 && m.y <= 470)
			mouse_set = 1;
		else if (
			m.x >= 220 && m.x <= 455
			&&
			m.y >= 480 && m.y <= 510
			)
			mouse_set = 2;
		settextcolor(WHITE);
		switch (mouse_set)
		{
		case 0:
			settextcolor(BLACK);
			outtextxy(220, 440, _T("再来一局"));
			outtextxy(220, 480, _T("不想玩了"));
			break;
		case 1:
			outtextxy(220, 440, _T("再来一局"));
			if (m.mkLButton) {
				choicecolor();
				return;
			}
			break;
		case 2:
			outtextxy(220, 480, _T("不想玩了"));
			if (m.mkLButton) {
				end();
				return;
			}
			break;
		}
	}

}
void flsh() {
	int r, g, b;
	DWORD* pbWnd = GetImageBuffer();         // 获取绘图窗口的显存指针
	for (int light = 128; light >= 100; light--)
	{
		for (int i = 0; i < 700 * 700; i++)
		{
			// 计算原图片每个点的颜色，实现逐渐变暗的效果
			r = (int)((GetRValue(pbWnd[i]) * light) >> 7);
			g = (int)((GetGValue(pbWnd[i]) * light) >> 7);
			b = (int)((GetBValue(pbWnd[i]) * light) >> 7);
			pbWnd[i] = RGB(r, g, b);
		}
		FlushBatchDraw();     // 使之前的显存操作生效
		Sleep(40);
	}
}
int judge(int color) {
	int x, y, x0, y0, x1, y1, x2, y2;//x0,y0保存a,b的值,x1,y1保存起始段点,x2,y2保存末端点
	int l = 4;
	x0 = a; y0 = b;
	//上下判断
	x = x0, y = y0;
	for (l = 0; crossing[x][y - 1].rt == color && l < 4 && y>0; l++) {
		y--;
	};
	x1 = x; y1 = y;
	x = x0, y = y0;
	for (l = 0; crossing[x][y + 1].rt == color && l < 4 && y < 14; l++) {
		y++;
	}
	x2 = x; y2 = y;
	if (y2 - y1 + 1 >= 5) {
		return 1;
	}
	//左右判断
	x = x0, y = y0;
	for (l = 0; crossing[x - 1][y].rt == color && l < 4 && x>0; l++) {
		x--;
	};
	x1 = x; y1 = y;
	x = x0, y = y0;
	for (l = 0; crossing[x + 1][y].rt == color && l < 4 && x < 14; l++) {
		x++;
	}
	x2 = x; y2 = y;
	if (x2 - x1 + 1 >= 5) {
		return 1;
	}
	//左上-右下判断
	x = x0, y = y0;
	for (l = 0; crossing[x - 1][y - 1].rt == color && l < 4 && y>0 && x > 0; l++) {
		y--;
		x--;
	};
	x1 = x; y1 = y;
	x = x0, y = y0;
	for (l = 0; crossing[x + 1][y + 1].rt == color && l < 4 && y < 14 && x < 14; l++) {
		y++;
		x++;
	}
	x2 = x; y2 = y;
	if (y2 - y1 + 1 >= 5) {
		return 1;
	}
	//左下-右上判断
	x = x0, y = y0;
	for (l = 0; crossing[x + 1][y - 1].rt == color && l < 4 && y>0 && x < 14; l++) {
		y--;
		x++;
	};
	x1 = x; y1 = y;
	x = x0, y = y0;
	for (l = 0; crossing[x - 1][y + 1].rt == color && l < 4 && y < 14 && x>0; l++) {
		y++;
		x--;
	}
	x2 = x; y2 = y;
	if (y2 - y1 + 1 >= 5) {
		return 1;
	}
	return 0;
}

  
void ready() { //准备界面
	initgraph(L, L);								//初始化窗口
	loadimage(NULL, _T("bg.png"), 700, 700, true);
	setbkmode(TRANSPARENT);							//设置显示字体样式
	settextcolor(BLACK);
	settextstyle(60, 40, _T("楷体"));
	outtextxy(80,80,_T("人机对弈五子棋"));
	settextstyle(30, 20, _T("楷体"));
	outtextxy(440, 180, _T("——石贇"));
	settextstyle(45, 25, _T("华文新魏"));
	outtextxy(220, 450, _T("开始游戏"));	
}

void start() {//开始下棋
	int mouse_set;//存放鼠标位置状态值
	
	while (true) {
		mouse_set = 0;
		m = GetMouseMsg();
		if (m.x >= 200 && m.x <= 455
			&&
			m.y>=420&&m.y<=520)
		mouse_set = 1;
			
		if (mouse_set==1)
		{	
			settextcolor(WHITE);
			outtextxy(220, 450, _T("开始游戏"));
			if (m.mkLButton) {
				choicecolor();
			return;
			}
		}
		else {
			settextcolor(BLACK);
			outtextxy(220, 450, _T("开始游戏"));
		}

	}

}

void down_white(int x, int y) {
	setlinecolor(WHITE);
	setfillcolor(WHITE);
	fillcircle(crossing[x][y].x, crossing[x][y].y, 15);
	crossing[x][y].rt = rt_WHITE;
	char txt[10];
	sprintf_s(txt, 10,"%d:%d   \t\t",x,y);
	fprintf(fp, txt);
}
void down_black(int x, int y) {
	setlinecolor(BLACK);
	setfillcolor(BLACK);
	fillcircle(crossing[x][y].x, crossing[x][y].y, 15);
	crossing[x][y].rt = rt_BLACK;
	char txt[10];
	sprintf_s(txt, 10, "%d:%d   \t\t", x, y);
	fprintf(fp, txt);
}
void choicecolor() {					//选择用黑棋还是白棋
	int mouse_set = 0;
	loadimage(NULL, _T("bg.png"), 700, 700, true);
	settextcolor(BLACK);
	
	settextstyle(60, 40, _T("楷体"));
	outtextxy(80, 80, _T("人机对弈五子棋"));
	settextstyle(30, 20, _T("楷体"));
	outtextxy(440, 180, _T("——石贇"));

	settextstyle(30, 20, _T("楷体"));
	outtextxy(220, 440, _T("黑棋（先手）"));
	outtextxy(220, 480, _T("白棋（后手）"));

	while (true) {
		m = GetMouseMsg();
		mouse_set = 0;
		if (m.x >= 220 && m.x <= 455
			&&
			m.y >= 440 && m.y <= 470)
			mouse_set = 1;
		else if (
			m.x >= 220 && m.x <= 455
			&&
			m.y >= 480 && m.y <= 510
			)
			mouse_set = 2;
		settextcolor(WHITE);
		switch (mouse_set)
		{
		case 0:
			settextcolor(BLACK);
			outtextxy(220, 440, _T("黑棋（先手）"));
			outtextxy(220, 480, _T("白棋（后手）"));
			break;
		case 1:
			outtextxy(220, 440, _T("黑棋（先手）"));
			if (m.mkLButton) {
				choiceb();
				return;
			}
			break;
		case 2:
			outtextxy(220, 480, _T("白棋（后手）"));
			if (m.mkLButton) {
				choicew();
				return;
			}
			break;
		}
	}
}
int get_mouse_area() {	//判断鼠标位置，返回鼠标状态，（是否单击）		//应该可以优化
	int i, j, x0,y0;

	for (i=0;i<15;i++)
	{
		for (j = 0; j < 15; j++) {
			x0 = 28 + 46 * i;
			y0 = 28 + 46 * j;
			if (m.x > x0 - 15 && m.x < x0 + 15
				&&
				m.y>y0 - 15 && m.y < y0 + 15
				) {
				if (m.uMsg == WM_LBUTTONDOWN) {
					a = i;
					b = j;
					return 1;
				}
			}
		}
	}
	return 0;
}
void choicew() {					//白棋后手
	loadimage(NULL, _T("chessboard.jpg"), 700, 700, true);//棋盘
	definecossing();//保存交点
	int pos;		//鼠标状态
	down_black(7, 7);
	int i, j = 0;
	int x, y;
	num = 1;

	while (true) {
		x = 0; y = 0;
		m = GetMouseMsg();
		pos = get_mouse_area();
		if (num % 2 != 0) {
			switch (m.uMsg)
			{
			case WM_LBUTTONDOWN:
				if (pos == 1) {
					if (crossing[a][b].rt == rt_BLACK || crossing[a][b].rt == rt_WHITE) {
						continue;
					}
					else
					{
						x = a; y = b;
						down_white(x, y);
						num++;
					}
					if (judge(rt_WHITE)) {
						win_w();
						return;
					}
				}
			}
		}
		else {
			for (i = 0; i < 15; i++) {
				for (j = 0; j < 15; j++) {
					crossing[i][j].score = 0;		//清空内存防止有遗留内容
					value_one(i, j, rt_BLACK);
				}
			}
			/*=======================================================================待优化，可能棋盘下满仍未分出胜负或者crossing[0][0]已经被占用======================*/
/*			x = y = 0;
			crossing[x][y].score = -1000;
	*/
			for (i = 0; i < 15; i++) {					//寻找价值最高点
				for (j = 0; j < 15; j++) {
					if (crossing[x][y].score < crossing[i][j].score) {
						x = i; y = j;
					}
				}
			}

			a = x; b = y;
			down_black(a, b);
			num++;
			if (judge(rt_BLACK)) {
				m = GetMouseMsg();
				win_b();
				return;
			}
		}
	}

}
void choiceb() {			//黑棋先手
	int i, j;
	int x, y;
	num = 0;
	loadimage(NULL, _T("chessboardnum.jpg"), 700, 700, true);//棋盘
 	definecossing();//保存交点
	int pos;		//鼠标状态
	fopen_s(&fp,"log.txt", "w+");
	fprintf(fp, "回合\t黑棋\t白棋\n");
	while (true) {
		x, y = 0;
		m = GetMouseMsg();
		pos = get_mouse_area();
		if (num % 2 == 0) {
			switch (m.uMsg)
			{
				case WM_LBUTTONDOWN:
					if (pos==1)
					{
						if (crossing[a][b].rt == rt_BLACK || crossing[a][b].rt == rt_WHITE) {
							continue;
						}
						else {
							num++;
							char temp[10];
							sprintf_s(temp, 10, "%d \t\t", num/2);
							fprintf(fp, temp);
							down_black(a,b);
							if (judge(rt_BLACK)) {
								win_w();
								return;
							}
						}
					}
			} 
		}
		else {
			for (i = 0; i < 15; i++) {			//对每一个位置的价值清空		
				for (j = 0; j < 15; j++) {
					crossing[i][j].score = 0;
				}
			}
			/*================================
			==待优化，无需计算每一个空点位置==
			==待优化，无需计算每一个空点位置==
			==待优化，无需计算每一个空点位置==
			==================================*/
			for (i = 0; i < 15; i++) {			//判断每一个位置的价值
				for (j = 0; j < 15; j++) {
					value_one(i,j,rt_WHITE);
				}
			}
			x = y = 0;
			for (i = 0; i < 15; i++) {			//找出价值最高的点
				for (j = 0; j < 15; j++) {
    					if (crossing[x][y].score < crossing[i][j].score&&crossing[i][j].rt!=rt_BLACK && crossing[i][j].rt != rt_WHITE) {
						x = i; y = j;
					}
				}
			}
			a = x; b = y;
			down_white(a, b);
			fprintf(fp, "\n");
			num++;   
			/*if (judge(rt_WHITE)) {
				win_w();
				return;
			}*/
		}
	}

}
void definecossing() {		//定义保存交点
	int x, y, i, j;
	for (i= 0,x=L_START; i < 15; i++,x+=WIDTH+1)
	{
		for (j = 0, y = L_START; j < 15; j++, y += WIDTH+1) {
			crossing[i][j] = { x,y,0,0 };
		}
	}
}


void test() {
	initgraph(L, L);
	loadimage(NULL,_T("chessboard.jpg"),700,700,true);
	
	int x, y, i, j;
	for (i = 0, x = L_START; i < 15; i++, x += WIDTH+1)
	{
		for (j = 0, y = L_START; j < 15; j++, y += WIDTH+1) {
			crossing[i][j] = { x,y,0,0 };
			setlinecolor(WHITE);
			setfillcolor(WHITE);
			fillcircle(crossing[i][j].x, crossing[i][j].y, 1);
		}
	}
system("PAUSE");
}


int main() {
	
	ready();
	start();
	//test();
}