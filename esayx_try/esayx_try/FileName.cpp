#include<stdio.h>
#include<conio.h>
#include<time.h>
#include<math.h>
#include<wchar.h>
#include<graphics.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <functional>

using namespace std;

#define WIDTH 1024								// 屏幕宽
#define HEIGHT 576								// 屏幕高
#define MAPW (WIDTH*4)								// 地图宽
#define MAPH (HEIGHT*4)							// 地图高
#define AINUM 100									// AI 数量
#define FNUM 2000									// FOOD 数量
#define DISTANCE(x1,y1,x2,y2)	(sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)))		//计算距离

/*类说明*/
class FOOD {
public:
	bool eat;
	COLORREF color;								// 颜色
	int x, y;									// 坐标
	char type;
	friend void draw();
};

class BALL {	
public:								
	bool life;									//生命
	COLORREF color;								//颜色
	int x, y;									//坐标
	float r;									//半径
	friend void draw();
	friend void move(BALL* ball);
};

class Button
{
private:
	int x; // 按钮左上角x坐标
	int y; // 按钮左上角y坐标
	int width; // 按钮宽度
	int height; // 按钮高度
	float scale; // 缩放比例，用于实现鼠标悬停效果
	bool isMouseOver; // 表示鼠标是否在按钮上方
	wstring text; // 按钮文本
	function<void()> onClick; // 点击按钮触发的函数

public:

	Button(int x, int y, int width, int height, const wstring& text, const function<void()>& onClick)
		: x(x), y(y), width(width), height(height), text(text), onClick(onClick), scale(1.0f), isMouseOver(false)
	{
	}

	// 检查鼠标是否在按钮上方
	void checkMouseOver(int mouseX, int mouseY)
	{
		isMouseOver = (mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height);

		if (isMouseOver) {
			scale = 0.9f; // 鼠标悬停时缩放按钮
		}
		else {
			scale = 1.0f; // 恢复按钮原始大小
		}
	}

	// 检查鼠标点击是否在按钮内，并执行函数
	bool checkClick(int mouseX, int mouseY)
	{
		if (mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height)
		{
			onClick(); // 执行按钮点击时的函数
			isMouseOver = false;
			scale = 1.0f;
			return true;
		}
		return false;
	}

	// 绘制按钮
	void draw()
	{
		int scaledWidth = width * scale; // 缩放后的按钮宽度
		int scaledHeight = height * scale; // 缩放后的按钮高度
		int scaledX = x + (width - scaledWidth) / 2; // 缩放后的按钮x坐标
		int scaledY = y + (height - scaledHeight) / 2; // 缩放后的按钮y坐标

		if (isMouseOver)
		{
			setlinecolor(RGB(0, 120, 215)); // 鼠标悬停时按钮边框颜色
			setfillcolor(RGB(229, 241, 251)); // 鼠标悬停时按钮填充颜色

		}
		else
		{
			setlinecolor(RGB(173, 173, 173)); // 按钮边框颜色
			setfillcolor(RGB(225, 225, 225)); // 按钮填充颜色
		}

		fillrectangle(scaledX, scaledY, scaledX + scaledWidth, scaledY + scaledHeight); // 绘制按钮
		settextcolor(BLACK); // 设置文本颜色为黑色
		setbkmode(TRANSPARENT); // 设置文本背景透明
		settextstyle(20 * scale, 0, _T("微软雅黑")); // 设置文本大小和字体
		//居中显示按钮文本
		int textX = scaledX + (scaledWidth - textwidth(text.c_str())) / 2; // 计算文本在按钮中央的x坐标
		int textY = scaledY + (scaledHeight - textheight(_T("微软雅黑"))) / 2; // 计算文本在按钮中央的y坐标
		outtextxy(textX, textY, text.c_str()); // 在按钮上绘制文本
	}

	
};

/*函数说明*/
void move(BALL* ball);								// 玩家移动
void draw();										// 绘图
void start();										// 游戏开始
void setall();										// 初始化
void Opponent();									// AI
void Food();										// 食物
void delay(DWORD ms);								// 绝对延时
void end();

FOOD food[FNUM];														//食物
BALL mover = { 1,RGB(0,0,0),0,0,0 };									//玩家
BALL opponent[AINUM] = { 1,RGB(0,0,0),0,0,0 };							//AI
Button close(mover.x, mover.y ,100,50,_T("结束游戏"),[&](){ // 玩家主动终止游戏
	end();
	});		//强制结束

/*数值说明*/
DWORD* pBuffer;									// 显存指针
int eaten = 0;									// 吃 AI 的数量
int opponent_eaten = 0;									// AI 吃 AI的数量
float asp = 1;									// 缩放因子
float Time = 0;									// 时间

/*主函数*/
int main() {

	initgraph(WIDTH, HEIGHT);
	ExMessage msg;

	start();
	system("pause");

	setall();
	
	BeginBatchDraw();
	while (true)
	{
		cleardevice();
		move(&mover);
		Opponent();
		Food();
		draw();
		if (peekmessage(&msg)) // 检查是否有消息
		{
			int mouseX = msg.x; // 获取鼠标x坐标
			int mouseY = msg.y; // 获取鼠标y坐标

			switch (msg.message)
			{
			case WM_LBUTTONDOWN: // 鼠标左键按下事件
				close.checkClick(mouseX, mouseY); // 处理鼠标点击事件
				break;
			case WM_MOUSEMOVE: // 鼠标移动事件
				close.checkMouseOver(mouseX, mouseY); // 处理鼠标移动事件
				break;
			}
		}
		close.draw();
		FlushBatchDraw();							// 显示缓存的绘制内容
		delay(20);
	}
	EndBatchDraw();
	closegraph();
	return 0;
}

void move(BALL* ball) {
	if (ball->r <= 0)
		ball->life = false;
	
	
	// 判定游戏是否接束
	if (ball->life == false) {						
		HWND hwnd = GetHWnd();
		
		if (MessageBox(hwnd, _T("需要再来一局吗"), _T("游戏失败"), MB_OKCANCEL | MB_ICONEXCLAMATION) == IDOK)
		{
			
			closegraph();
			system("D:\\vs_c++_project\\esayx_try\\x64\\Debug\\esayx_try");
			
		}
		else {
			closegraph();
			exit(0);
		}
		
	}
	
	// 是否吃掉所有 AI
	if (eaten + opponent_eaten == AINUM)					
	{
		HWND hwnd = GetHWnd();
		if (MessageBox(hwnd, _T("需要再来一局吗"), _T("游戏胜利"), MB_OKCANCEL | MB_ICONEXCLAMATION) == IDOK)
		{
			closegraph();
			system("D:\\vs_c++_project\\esayx_try\\x64\\Debug\\esayx_try");
			
		}
		else {
			closegraph();
			exit(0);
		}
	}
	
	// 玩家吃 AI 判定
	for (int i = 0; i < AINUM; i++) {				
		if (ball->r >= opponent[i].r) {
			if (opponent[i].life == 0)	continue;
			if (DISTANCE(ball->x, ball->y, opponent[i].x, opponent[i].y) < (4 / 5.0 * (ball->r + opponent[i].r))) {
				opponent[i].life = 0;					//AI被吃
				ball->r = sqrt(opponent[i].r * opponent[i].r + ball->r * ball->r);
				eaten++;
			}
		}
	}
	
	// 玩家吃食物
	for (int n = 0; n < FNUM; n++) {				
		if (food[n].eat == 0)	continue;
		if (DISTANCE(ball->x, ball->y, food[n].x, food[n].y) < ball->r) {
			ball->r += 4 / ball->r;				// 增加面积
			food[n].eat = 0;						// 食物被吃
		}
	}

	// 记录偏移量
	static int mx = 0, my = 0;		
	
	if (GetAsyncKeyState(VK_UP) && (ball->y - ball->r > 0 && ball->y <= (MAPH - ball->r + 10))) {
		ball->y -= 4;
		my += 4;
	}

	if (GetAsyncKeyState(VK_DOWN) && (ball->y - ball->r >= -10 && ball->y < (MAPH - ball->r))) {
		ball->y += 4;
		my -= 4;
	}

	if (GetAsyncKeyState(VK_LEFT) && ball->x - ball->r > 0 && (ball->x <= (MAPW - ball->r + 10))) {
		ball->x -= 4;
		mx += 4;
	}

	if (GetAsyncKeyState(VK_RIGHT) && ball->x - ball->r >= -10 && (ball->x < (MAPW - ball->r))) {
		ball->x += 4;
		mx -= 4;
	}
	
	//坐标修正
	setorigin(mx, my);							

}

void Opponent() {
	
	for (int i = 0; i < AINUM; i++) {	
		// AI 吃玩家			
		if (opponent[i].r > mover.r) {
			if (DISTANCE(mover.x, mover.y, opponent[i].x, opponent[i].y) < (opponent[i].r + mover.r)) {
				opponent[i].r = sqrt(opponent[i].r * opponent[i].r + mover.r * mover.r);
				mover.life = 0;
				mover.r = 0;
			}
		}
		
		// AI 吃 AI
		for (int j = 0; j < AINUM; j++) {			
			if (opponent[i].r > opponent[j].r) {
				if (opponent[j].life == 0) continue;
				if (DISTANCE(opponent[i].x, opponent[i].y, opponent[j].x, opponent[j].y) < (opponent[i].r + opponent[j].r)) {
					opponent[i].r = sqrt(opponent[i].r * opponent[i].r + opponent[j].r * opponent[j].r);
					opponent[j].life = 0;
					opponent[j].r = 0;
					opponent_eaten++;
				}
			}
		}
		double min_DISTANCE = 100000;
		int min = -1;
		
		// AI 靠近 AI
		for (int k = 0; k < AINUM; k++) {			
			if (opponent[i].r > opponent[k].r && opponent[k].life != 0) {
				if (DISTANCE(opponent[i].x, opponent[i].y, opponent[k].x, opponent[k].y) < min_DISTANCE) {
					min_DISTANCE = DISTANCE(opponent[i].x, opponent[i].y, opponent[k].x, opponent[k].y);
					min = k;
				}
			}
		}

		if ((min != -1) && (rand() % 2 == 1)) {
			if (rand() % 2) {
				if (opponent[i].x < opponent[min].x)
					opponent[i].x++;
				else
					opponent[i].x--;
			}
			else {
				if (opponent[i].y < opponent[min].y)
					opponent[i].y++;
				else
					opponent[i].y--;
			}
		}

		// AI 吃食物
		for (int n = 0; n < FNUM; n++) {			
			if (food[n].eat == 0) continue;
			if (DISTANCE(opponent[i].x, opponent[i].y, food[n].x, food[n].y) < opponent[i].r) {
				opponent[i].r += 4 / opponent[i].r;
				food[n].eat = 0;
			}
		}
	}
}

void Food() {
	// 食物刷新
	for (int i = 0; i < FNUM; i++) {				
		if (food[i].eat == 0) {
			food[i].eat = 1;
			food[i].color = RGB(rand() % 256, rand() % 256, rand() % 256);
			food[i].x = rand() % MAPW;
			food[i].y = rand() % MAPH;
			food[i].type = rand() % 10 + 1;
		}
	}
}

void draw() {

	clearcliprgn();
	
	// 改变笔的颜色、状态
	setlinestyle(PS_SOLID | PS_JOIN_BEVEL, 20);		
	setlinecolor(RGB(0, 100, 0));

	line(-20, MAPH + 20, -20, -20);				// 左竖
	line(-20, MAPH + 20, MAPW + 20, MAPH + 20);		// 上横
	line(-20, -20, MAPW + 20, -20);				// 下横
	line(MAPW + 20, -20, MAPW + 20, MAPH + 20);		// 右竖

	setfillcolor(GREEN);

	if (mover.x - 0.5 * WIDTH / asp < -20)
		floodfill(-20 - 11, mover.y, RGB(0, 100, 0));
	if (mover.x + 0.5 * WIDTH / asp > MAPW + 20)
		floodfill(MAPW + 20 + 11, mover.y, RGB(0, 100, 0));
	if (mover.y - 0.5 * HEIGHT / asp < -20)
		floodfill(mover.x, -20 - 11, RGB(0, 100, 0));
	if (mover.y + 0.5 * HEIGHT / asp > MAPH + 20)
		floodfill(mover.x, MAPH + 20 + 11, RGB(0, 100, 0));

	setlinecolor(WHITE);
	setlinestyle(PS_NULL);
	
	// 画出食物
	for (int i = 0; i < FNUM; i++) {				

		if (food[i].eat == 0) continue;
		setfillcolor(food[i].color);
		
		// 形状
		switch (food[i].type) {					
		case 1:		solidellipse(food[i].x, food[i].y, food[i].x + 2, food[i].y + 4); break;
		case 2:		solidellipse(food[i].x, food[i].y, food[i].x + 4, food[i].y + 2);	break;
		case 3:		solidrectangle(food[i].x, food[i].y, food[i].x + 4, food[i].y + 2); break;
		case 4:		solidrectangle(food[i].x, food[i].y, food[i].x + 2, food[i].y + 4); break;
		case 5:		solidroundrect(food[i].x, food[i].y, food[i].x + 2, food[i].y + 4, 2, 2); break;
		case 6:		solidroundrect(food[i].x, food[i].y, food[i].x + 4, food[i].y + 2, 2, 2); break;
		case 7:		solidroundrect(food[i].x, food[i].y, food[i].x + 4, food[i].y + 2, 4, 2); break;
		case 8:		solidroundrect(food[i].x, food[i].y, food[i].x + 4, food[i].y + 2, 2, 4); break;
		case 9:		solidroundrect(food[i].x, food[i].y, food[i].x + 4, food[i].y + 2, 1, 1); break;
		case 10:	fillcircle(food[i].x, food[i].y, 4); break;
		}
	}
	
	// 画 AI
	for (int i = 0; i < AINUM; i++) {				
		if (opponent[i].life == 0) continue;
		setfillcolor(opponent[i].color);
		fillcircle(opponent[i].x, opponent[i].y, int(opponent[i].r + 0.5));
	}
	
	// 画玩家
	setfillcolor(mover.color);						
	fillcircle(mover.x, mover.y, int(mover.r + 0.5));
	
	// 小地图
	IMAGE map(150, 100);							
	SetWorkingImage(&map);
	setbkcolor(RGB(120, 165, 209));					// 浅灰色背景

	cleardevice();
	
	// 画 AI（小地图）
	for (int i = 0; i < AINUM; i++)				
	{
		if (opponent[i].life == 0) continue;
		setfillcolor(opponent[i].color);
		fillcircle(opponent[i].x * 150 / WIDTH / 4, opponent[i].y * 100 / HEIGHT / 4, int(opponent[i].r / 28 + 0.5));
	}
	// 画玩家（小地图）
	setfillcolor(mover.color);						
	fillcircle(mover.x * 150 / WIDTH / 4, mover.y * 100 / HEIGHT / 4, int(mover.r / 28 + 0.5));
	setlinecolor(RGB(0, 100, 0));
	
	// 恢复绘图背景
	SetWorkingImage();							
	putimage(mover.x + int(0.5 * WIDTH) - 150, mover.y - int(0.5 * HEIGHT), 150, 100, &map, 0, 0);												// 画出小地图
	setlinecolor(LIGHTBLUE);
	setlinestyle(PS_SOLID | PS_JOIN_BEVEL, 4);
	line(mover.x + int(0.5 * WIDTH) - 151, mover.y - int(0.5 * HEIGHT), mover.x + int(0.5 * WIDTH) - 151, mover.y - int(0.5 * HEIGHT) + 99);	// 地图边框线
	line(mover.x + int(0.5 * WIDTH) - 151, mover.y - int(0.5 * HEIGHT) + 99, mover.x + int(0.5 * WIDTH), mover.y - int(0.5 * HEIGHT) + 99);		// 地图边框线

	setlinestyle(PS_NULL);																														// 恢复笔
	TCHAR str[32];
	swprintf_s(str, _T("质量:%.1fg  击杀:%d"), mover.r, eaten);
	settextcolor(BLUE);																															// 改字体
	outtextxy(mover.x - int(0.5 * WIDTH), mover.y - int(0.5 * HEIGHT), str);
	settextcolor(BLUE);
	outtextxy(mover.x - 36, mover.y - 8, _T("你"));
}

// 玩家主动终止游戏
void end() {
	HWND hwnd = GetHWnd();
	MessageBox(hwnd, _T("游戏记录不会保存"), _T("游戏结束"), MB_OK | MB_ICONEXCLAMATION);	// 结束
	closegraph();
	exit(0);
}

void setall() {
	// 随机数
	srand((unsigned)time(NULL));					

	mover.color = RGB(rand() % 256, rand() % 256, rand() % 256);		// 随机颜色
	mover.life = 1;														// 赋初值1
	mover.x = int(WIDTH * 0.5);
	mover.y = int(HEIGHT * 0.5);
	mover.r = 20;
	
	// AI 的属性
	for (int i = 0; i < AINUM; i++) {				
		opponent[i].life = 1;
		opponent[i].color = RGB(rand() % 256, rand() % 256, rand() % 256);
		opponent[i].r = float(rand() % 10 + 10);
		opponent[i].x = rand() % (MAPW - int(opponent[i].r + 0.5)) + int(opponent[i].r + 0.5);
		opponent[i].y = rand() % (MAPH - int(opponent[i].r + 0.5)) + int(opponent[i].r + 0.5);
	}
	
	// 食物的属性
	for (int i = 0; i < FNUM; i++)					
	{
		food[i].eat = 1;
		food[i].color = RGB(rand() % 256, rand() % 256, rand() % 256);
		food[i].x = rand() % MAPW;
		food[i].y = rand() % MAPH;
		food[i].type = rand() % 10 + 1;
	}
	
	// 获取显存指针
	pBuffer = GetImageBuffer(NULL);					

	setbkcolor(WHITE);							// 白色背景
	cleardevice();								// 初始化背景

	settextcolor(LIGHTRED);						// 改字体
	setbkmode(TRANSPARENT);
	settextstyle(16, 0, _T("宋体"));

}

// 绝对延时
void delay(DWORD ms)								
{
	static DWORD oldtime = GetTickCount();

	while (GetTickCount() - oldtime < ms)
		Sleep(1);

	oldtime = GetTickCount();
}

void start()
{
	setbkcolor(RGB(0,0,0));							// 背景
	cleardevice();								// 初始化背景
	settextcolor(RGB(255,255,255));							// 改字体
	setbkmode(TRANSPARENT);

	settextstyle(128, 0, _T("宋体"));
	outtextxy(120, 40, _T("球球大作战"));

	settextstyle(32, 0, _T("宋体"));
	settextcolor(RGB(248,232,157));
	outtextxy(170, 240, _T("   ↑ 上移　 ↓ 下移　 ← 左移　 → 右移  "));

	settextcolor(RGB(113,175,164));
	outtextxy(112, 340, _T("躲避大球　　　追补小球　　　捕食敌人　　　增强实力"));

	settextcolor(WHITE);
	settextstyle(20, 0, _T("宋体"));
	outtextxy(810, 10, _T("65"));
	outtextxy(810, 30, _T("张信菲"));
	outtextxy(810, 50, _T("20232241014"));	

}