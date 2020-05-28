/*
	开启一个ege窗口   		initgrapg(x, y)   x,y 表示长宽
	关闭ege窗口      	    closegraph()
	设置画笔的颜色   		setcolor(GREEN) 或者使用 setcolor(EGERGB(R, G, B))
	直线的画法  			line(stx, sty, edx, edy)
	圆的画法				circle(x, y, r)
	实心填充				setfillcolor(EGERGB(r, g, b))
	实心矩形画法			bar(x1, y1, x2, y2)   (x1, y1) 和 (x2, y2) 分别表示矩形左上方和右下方的点的坐标
	填充				   floodfill(x, y, EGEcolor(r, g, b))  (x, y)表示开始填充的坐标
	实心椭圆               fillellipse(x, y, a, b)
	设置背景颜色            setbkcolor(EGERGB(r, g, b))
	清除屏幕                cleardevice()
	
	光栅显示（像素过程，有限个像素，像素的坐标都是整形）
	一.直线段扫描转化算法（提高效率，把乘法消除）
		1.DDA（数值微分法）
			增量思想
				|K| <= 1 时使用x增量法   y = ax + b (x每次加a)
				|k| > 1 时使用y增量法    x = ax + b (y每次加a)
			效率改进
				浮点加法改成整数加法， 直线方程类别
		2.中点画线法
			运用直线一般方程 ax + by + c = 0; (可以化浮点为整型)
		3.Bresenham算法
	二.多边形的扫描转化和区域填充
		解决问题：怎么样在离散的像素集上表示一个连续的二维图形
		1.多边形的扫描转化
			顶点表示
			点阵表示
			x扫描线算法（y扫描）
				配对区间填充（交点必须为偶数个，涉及到交点的取舍）
					（1）若共享顶点的两条边分别落在扫描线的两边，交点只算一个
					（2）若共享顶点在扫描线的同一边，交点算两个或者零个
					求交计算量过大
					算法改进
						扫描线
						增量
		2.区域填充
			区域联通（四向联通，八向联通）
	三.反走样技术
		解决问题：由于像素离散化而导致的走样现象
		1.采用分辨率更高的显示屏幕（分辨率）
		2.非加权区域采样方法（权值一样）
		3.加权区域采样方法
			直线段对于一个像素亮度的贡献值正比于相交区域与像素中心的距离
	四.直线段裁剪算法
		COnhen-Suther I and算法（编码裁剪算法）


	五.二维图像的变换
		仿射变换
			x' = ax + by + m
			y' = cx + dy + n
			即可表示为:[x', y'] = [x, y, 1] * [[a, b, m], [c, d, n]]
				对于向量（x, y, 1），可以在几何意义上理解为在第三维为常数的平面上的一个二维向量（齐次坐标表示法-->为了使用矩阵运算）
		平移，比例，旋转（相乘，矩阵乘法不满足交换律）
	六.窗口和视区的变换

*/

#include <graphics.h>
#include <iostream>
#include <cstring>
#include <queue>
#include <cmath>
using namespace std;
typedef pair<int, int> pii;
template<typename T>
class Point{
public:
	T x, y;
	Point(){x = 0, y = 0;}
	Point(T x, T y)	{this->x = x; this->y = y;};
	operator pair<int, int> (){
		pii res;
		res.first = x, res.second = y; 
		return res;
	}
	void to_string(){
		cout << "Location: (";
		cout << this->x << "," << this->y  << ")"<< endl;
	}
	void setValue(pii value){
		this->x = value.first;
		this->y = value.second;
	}
};
/*
	Way为变换方式：
		1表示不做变换
		xPro 和 yPro 分别为x, y拉伸变换   
		xAdd 和 yAdd 分别为x, y平移变换
*/
class  Way{
public:
	double xPro, yPro, zPro;
	double xAdd, yAdd, zAdd; 
	Way(){
		this->xAdd = 0.0, this->xPro = 0.0;
		this->yAdd = 0.0, this->yPro = 0.0;
		this->zAdd = 0.0, this->zPro = 0.0;
	}
	Way(double xa, double xp = 0, double ya = 0, double yp = 0, double za = 0, double zp = 0){
		this->xAdd = xa, this->xPro = xp;
		this->yAdd = ya, this->yPro = yp;
		this->zAdd = za, this->zPro = zp;
	}
};
const color_t baseColor[3] = {RED, GREEN, BLUE};
const int N = 10;
double base[N];
double change[N][N];
/*
	---------------------------
	坐标的交换：
		way参数 -->  交换方式
		way = 0   不交换
		way > 0   x小的放前面
		way < 0   y小的放前面
	---------------------------
*/
void Myswap(pii &a, pii &b, int way){   
	if(way > 0 && a.first > b.first){	
		pii tmp = a;
		a = b;
		b = tmp;
	}else if(way < 0 && a.second >= b.second){ 
		pii tmp = a;
		a = b;
		b = tmp;
	}
}
/*
	---------------------------
	dda算法
	---------------------------
*/
template<typename T>
void drawLine(const Point<T> &point1, const Point<T> &point2, color_t color){    //定义point1为
	pii a, b;
	//此处可以四舍五入
	a = {int(point1.x), int(point1.y)}; 
	b = {int(point2.x), int(point2.y)};
	double k = (a.second - b.second) * 1.0 / (a.first - b.first);  //斜率
	int distance, x, y;
	if(a.first == b.first || a.second == b.second){
		setcolor(color);
		line(a.first, a.second, b.first, b.second);
		return ;
	}
	double last;
	if(abs(k) >= 1.0){	//y增量法
		Myswap(a, b, 1);
		distance = b.first - a.first;
		last = a.first - k;   //第一个点包含a, 最后一个点不包含b
		x = a.first;
		for(int i = 0; i < distance; i ++ ){
			// y = ( last + k - int(last + k) ) >= 0.5 ? int(last + k) + 1 : int(last + k);
			y = int(last + k + 0.5);
			last += k;
			putpixel(x + i, y, color);
		}
	}else{	//x增量法
		Myswap(a, b, -1);
		cout << a.first << " " << a.second << endl;
		cout << b.first << " " << b.second << endl; 
		distance =  b.second - a.second;
		k = 1.0 / k;
		last = a.second - k;
		y = a.second;
		
		for(int i = 0; i < distance; i ++ ){
			// x = last + k - int(last + k) >= 0.5 ? int(last + k) + 1 : int(last + k);
			x = int(last + k + 0.5);
			last += k;
			putpixel(x, y+i, color); 
		}
	}
}
/*
	---------------------------
	填充算法：
		point 开始填充的的颜色
		egeColor 边界的颜色
		setColor 设置的颜色
		way 填充的方式（八填充与四填充， 默认为4填充）
		优化区域填充扫描线算法
	---------------------------
*/
void floodFill(pii point, color_t egeColor, color_t setColor, int way = 4){
	int dic[8][2] = {
		{-1, 0}, {0, 1}, {1, 0}, {0, -1}, {-1, 1}, {1, 1}, {1, -1}, {-1, -1}
	};
	queue<pii> qu;
	if(getpixel(point.first, point.second) == egeColor)		return ;
	putpixel_f(point.first, point.second, setColor);
	qu.push(point);
	pii now;
	int xx, yy;
	while(!qu.empty()){
		now = qu.front();
		qu.pop();
		for(int i = 0; i < way; i ++ ){
			xx = now.first + dic[i][0];
			yy = now.second + dic[i][1];
			color_t gain = getpixel(xx, yy);
			if(gain != egeColor && gain != setColor){
				qu.push({xx, yy});
				putpixel_f(xx, yy, setColor);
			}	
		}
	}
}
/*
	---------------------------
	矩阵叉乘
	(1*n)  * (n*m)
	---------------------------
*/
void martrixMul(int n, int m, double aim[]){   
	memset(aim, 0, sizeof aim);
	for(int i = 0; i < n; i ++ ){
		for(int j = 0; j < m; j ++ ){
			aim[i] += base[j] * change[j][i]; 
		}
	}
}
/*
	---------------------------
	二维点平移比例变换
	point 需要变换的点的坐标
	way   变换的方式
	---------------------------
*/
pii TwoDmoveto(const pii &point, Way way){
	base[0] = point.first,	base[1] = point.second,	base[2] = 1;
	memset(change, 0, sizeof change);
	change[0][0] = way.xPro, change[1][1] = way.yPro; change[2][2] = 1;
	change[2][0] = way.xAdd, change[2][1] = way.yAdd;
	double res[3];
	martrixMul(3, 3, res);
	return {res[0], res[1]};
}
/*
	---------------------------
	二维旋转变换
	point  需要旋转的点
	k      顺时针旋转的度数
	---------------------------
*/
pii TwoDchange(const pii &point, int k){
	base[0] = point.first,	base[1] = point.second,	base[2] = 1;
	memset(change, 0, sizeof change);
	change[0][0] = cos(k * PI / 180), change[1][1] = cos(k * PI / 180); change[2][2] = 1;	
	change[1][0] = -sin(k * PI / 180), change[0][1] = sin(k * PI / 180);
	double res[3];
	martrixMul(3, 3, res);
	return {res[0], res[1]};
}
/*
	---------------------------
	绕着point，半径为 r, 顺时针旋转旋转（以水平向右为基准0度）
	画图专用
	---------------------------
*/
pii cyclePoint(const pii &point, int r, Way way = {0, 0, 0, 0, 0, 0}){
	base[0] = r, base[1] = r, base[2] = 1;
	memset(change, 0, sizeof change);
	change[0][0] =  way.xPro, change[1][1] = way.yPro;
	change[2][0] = way.xAdd, change[2][1] = way.yAdd;
	change[2][2] = 1;
	double res[3];
	martrixMul(3, 3, res);
	return {res[0] + point.first, res[1] + point.second};
}
/*
	---------------------------
	三维点平移比例变换
	point 需要变换的点的坐标
	way   变换的方式
	---------------------------
*/
void build(int x, int y, int weight){
    setcolor(baseColor[0]);
    line(x, y, x, y-weight);   //z轴
    setcolor(baseColor[1]);
    line(x, y, x+weight, y);  //x轴
    double px, py;
    px = x - sqrt(weight*weight/2.0);
    py = y + sqrt(weight*weight/2.0);
    setcolor(baseColor[2]);
    line(x, y, px, py);  //y轴
}
int main(){
	// initgraph(1200, 1200);












	// initgraph(1200, 1200);
	// setbkcolor(BLACK);
	// build(500, 500, 400);
	// int r = random(255), g = random(255), b = random(255);
	// /*
	// 立方体信息：
	// 	长： 200
	// 	宽： 100
	// 	高： 100
	// */
	// int hp = sqrt(100 * 100 / 2.0);
	// Point<int> point1(500, 500), point2(700, 500), point3(500 - hp, 500 + hp), point4(700 - hp, 500 + hp);
	// Point<int> point5(500, 400), point6(700, 400), point7(500 - hp, 400 + hp), point8(700 - hp, 400 + hp);
	// ege_point point[8] = {{500, 500}, {700, 500}, {500 - hp, 500 + hp}, {700 - hp, 500 + hp},
	// 					{500, 400}, {700, 400}, {500 - hp, 400 + hp}, {700 - hp, 400 + hp}};
	// setcolor(EGERGB(r, g, b));
	// line(point1.x, point1.y, point2.x, point2.y);
	// line(point3.x, point3.y, point4.x, point4.y);
	// line(point1.x, point1.y, point3.x, point3.y);
	// line(point2.x, point2.y, point4.x, point4.y);

	// line(point5.x, point5.y, point6.x, point6.y);
	// line(point7.x, point7.y, point8.x, point8.y);
	// line(point5.x, point5.y, point7.x, point7.y);
	// line(point6.x, point6.y, point8.x, point8.y);

	// line(point1.x, point1.y, point5.x, point5.y);
	// line(point3.x, point3.y, point7.x, point7.y);
	//  line(point2.x, point2.y, point6.x, point6.y);
	// line(point4.x, point4.y, point8.x, point8.y);



	// setcolor(WHITE);
	// circle(500, 500, 40);
	// drawLine(point1, point2, EGERGB(r, g, b));
	// drawLine(point3, point4, EGERGB(r, g, b));
	// drawLine(point1, point3, EGERGB(r, g, b));
	// drawLine(point2, point4, WHITE);
	// drawLine(point5, point6, EGERGB(r, g, b));
	// drawLine(point7, point8, EGERGB(r, g, b));
	// drawLine(point5, point7, EGERGB(r, g, b));
	// drawLine(point6, point8, EGERGB(r, g, b));
	// drawLine(point1, point5, EGERGB(r, g, b));
	// drawLine(point2, point6, EGERGB(r, g, b));
	// drawLine(point3, point7, EGERGB(r, g, b));s
	// drawLine(point4, point8, EGERGB(r, g, b));
	// setcolor(GREEN);	
	// circle(point3.x, point3.y, 40);

	// getch();
	// closegraph();
	// return 0;

	// 二维图案：(二维图像变换)
	pii base1, base2 = {500, 500};
	int bcircle, scircle;
	cout << "提示信息： -->  进入界面以后持续按空格 " << endl;
	cout << "参考数据： 500 500 150 60 12" << endl;
	cout << "输入正中心坐标以及小圆的环绕半径（大圆半径）：";
	cin >> base2.first >> base2.second >> bcircle;
	cout << "输入小圆的半径：";
	cin >> scircle;
	cout << "请输入密度(取1-360之间的数)：";
	int p;
	cin >> p; 
	initgraph(1600, 1600);
	setbkcolor(BLACK);
	int k = 0;
	pii now;
	Way way = {0, 0, 0, 0};
	Way way1 = {0, 0, 0, 0};
	int cnt = 0;
	while(cnt <= 360){
		cnt += p;
		way1.xPro = cos(cnt * PI / 180);
		way1.yPro = sin(cnt * PI / 180);
		base1 = cyclePoint(base2, bcircle, way1);
		k = 0;
		while(k <= 360){
			k += p;
			way.xPro = cos(k * PI / 180);
			way.yPro = sin(k * PI / 180);
			now = cyclePoint(base1, scircle ,way);
			int r = random(255), g = random(255), b = random(255);
			setcolor(EGERGB(r, g, b));
			circle(now.first, now.second, scircle);
		}
		getch();
	}
	closegraph();
	return 0;

}












