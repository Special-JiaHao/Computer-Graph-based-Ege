#include <iostream>
#include <graphics.h>
#include <vector>
#include <cstring>
#include <cmath>
#include <queue>
#define random(x) (rand()%255)
using namespace std;
typedef mouse_msg msg;
typedef pair<int, int> pii;
char VERSION[100] = "Value1.0";     // 版本号
int DEVICE_HEIGHT, DEVICE_WIDTH;    // 设备尺寸

int OFFSET;          // 功能栏尺寸
int PITCKUP;         // 当前画图模式
msg NOWSTATE;        // 当前鼠标位置
pii LOCATION[10];    // 记录提示栏位置
color_t ColorSet[1000];      //  调色板
color_t FunctionITEM[100];   //  功能栏主题
int COLORSIZE;               //  调色板颜色种类
pii FunctionArea[3];         //  偏移信息
char NAME[20];               //  用户名
char SHAPE[10][20] = {"", "line", "rectangle", "circle", "clear / theme", "Current color", "floodfill", "Size", "Pencial", " "};   //  功能
pii LABEL[10];       //  功能提示位置信息
int NowSize;
int ColorWidth = 10;
struct FULLAREA{     //  填充点信息集合
   int size;
   pii point[1000];
   color_t color[1000];
} FULLpoint; 
struct DrawMessage{   // 记录已画图形信息
   pii st, ed;
   int r;
   int shape;
   int size;
   color_t color;
};
struct point{
   pii st, ed;
   int color;
   int size;
   bool operator < (point other) const{
      return st.first < other.st.first;
   }
};
int dic[30] = {0, 1, 1, 1, -1, -2, 2, -3, 3, -4, 4, -5, 5};
vector<point> PencialPoint;
void floodFill(pii point, color_t setColor, int way);
void buildItem(int index);
void reDraw(DrawMessage message);
void reappear(vector<DrawMessage> graphic, int color);
void drawPattern(DrawMessage message);
msg drawline(vector<DrawMessage> &graphic, int &color);
msg drawbar(vector<DrawMessage> &graphic, int &color);
msg drawCircle(vector<DrawMessage> &graphic, int &color);
msg fullColor(msg point, int color);
msg pencial(vector<DrawMessage> &graphic, int color);
void reBuild();

/*
	---------------------------
	改进填充算法：
		point 开始填充的点
		setColor 设置的颜色
		way 填充的方式（八填充与四填充， 默认为4填充）
		优化区域填充扫描线算法
	---------------------------
*/
void floodFill(pii point, color_t setColor, int way = 4){
	int dic[8][2] = {
		{-1, 0}, {0, 1}, {1, 0}, {0, -1}, {-1, 1}, {1, 1}, {1, -1}, {-1, -1}
	};
   color_t egeColor = getpixel(point.first, point.second);
	queue<pii> qu;
	if(getpixel(point.first, point.second) == setColor)		return ;
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
			if(gain == egeColor){
				qu.push({xx, yy});
				putpixel_f(xx, yy, setColor);
			}	
		}
	}
}
/*
   ---------------------------
   dao ：画图定位器（适用于刷新）
   ---------------------------
*/
void reDraw(DrawMessage message){
   setcolor(message.color);
   pii st = message.st, ed = message.ed;
   switch(message.shape){
      case 1 : {
         for(int i = 0; i < (message.size - 1) * 2 + 1; i ++ ){
            line(st.first + dic[i], st.second + dic[i], ed.first + dic[i], ed.second + dic[i]);
         }
         break;
      }
      case 2 : {
         for(int i = 0; i < (message.size - 1) * 2 + 1; i ++ ){
            rectangle(st.first + dic[i], st.second + dic[i], ed.first + dic[i], ed.second + dic[i]);
         }
         break;
      }
      case 3 : {
         for(int i = 0; i < message.size; i ++ ){
            circle(st.first , st.second, message.r + i);
            circle(st.first , st.second, message.r - i);
         }
         break;
      }
   }
}
/*
   ---------------------------
   重构图形（适用于刷新）
   ---------------------------
*/
void reappear(vector<DrawMessage> graphic, int color){
   for(int i = 0; i < graphic.size(); i ++ ){
      reDraw(graphic[i]);
   }
   buildItem(color);
   // delay_fps(2000);
}
/*
   ---------------------------
   dao ：画图定位器
   ---------------------------
*/
void drawPattern(DrawMessage message){
   setcolor(message.color);
   pii st = message.st, ed = message.ed;
   switch(message.shape){
      case 1 : { 
         for(int i = 0; i < (message.size - 1) * 2 + 1; i ++ ){
            line(st.first + dic[i], st.second + dic[i], ed.first + dic[i], ed.second + dic[i]);
         }
         break; 
      }
      case 2 : {
         for(int i = 0; i < (message.size - 1) * 2 + 1; i ++ ){
            rectangle(st.first + dic[i], st.second + dic[i], ed.first + dic[i], ed.second + dic[i]);
         }
         break;
      }
      case 3 : {
         int r = sqrt((message.st.first - message.ed.first) * (message.st.first - message.ed.first) + (message.st.second - message.ed.second) * (message.st.second - message.ed.second));
         for(int i = 0; i < message.size; i ++ ){
            circle(st.first , st.second, r + i);
            circle(st.first , st.second, r - i);
         }
      }
   }
}
/*
   ---------------------------
   生成线段信息，转发给drawPattern
   ---------------------------
*/
msg drawline(vector<DrawMessage> &graphic, int &color){
   int flag = 0;
   msg last, tmp, now;
   DrawMessage hp;
   int Shape = 1;
   while(is_run()){
      while(mousemsg()){
         tmp = getmouse();
         NOWSTATE = tmp;
         reappear(graphic, color); 
      }
      if((tmp.x <=OFFSET) || (tmp.y <= OFFSET)){
         if(tmp.is_down()){
            if(tmp.x <= OFFSET && (tmp.y >= PITCKUP*OFFSET && tmp.y <= (PITCKUP+1)*OFFSET))   continue;
            if(tmp.x <= OFFSET)  return tmp;
            color = (tmp.x - OFFSET) / ColorWidth;
            floodFill({1,OFFSET*4 + 10}, color == -1 ? WHITE : ColorSet[color]);
         }    
         continue;
      }
      if(tmp.is_move() && flag == 1){
         hp.st = {now.x, now.y}, hp.ed = {last.x, last.y};
         hp.shape = Shape;
         hp.size = NowSize;
         hp.color = BLACK;
         hp.size = NowSize;
         drawPattern(hp);
         now = tmp;
         hp.st = {now.x, now.y};
         hp.size = NowSize;
         hp.color = color == -1 ? WHITE : ColorSet[color];
         drawPattern(hp);
      }   
      if(tmp.is_down() && !flag){
         flag = 1;
         last = tmp;
         now = last;
      }
      if(tmp.is_up() && flag){
         hp.st = {now.x, now.y}, hp.ed = {last.x, last.y};
         hp.shape = Shape;
         hp.size = NowSize;
         hp.color = BLACK;

         now = tmp;
         
         hp.st = {now.x, now.y}, hp.ed = {last.x, last.y};
         hp.shape = Shape;
         hp.size = NowSize;
         hp.color = color == -1 ? WHITE : ColorSet[color];
         drawPattern(hp);
         graphic.push_back(hp);
         flag = 0;
      }
      reappear(graphic, color); 
   }
   return now;
}
/*
   ---------------------------
   生成矩形信息，转发给drawPattern
   ---------------------------
*/
msg drawbar(vector<DrawMessage> &graphic, int &color){
   int flag = 0;
   msg now, tmp, last;
   DrawMessage hp;
   int Shape = 2;
   while(is_run()){
      while(mousemsg()){
         tmp = getmouse();
         NOWSTATE = tmp;
         reappear(graphic, color); 
      }
      if((tmp.x <=OFFSET) || (tmp.y <= OFFSET)){
         if(tmp.is_down()){
            if(tmp.x <= OFFSET && (tmp.y >= PITCKUP*OFFSET && tmp.y <= (PITCKUP+1)*OFFSET))   continue;
            if(tmp.x <= OFFSET)  return tmp;
            color = (tmp.x - OFFSET) / ColorWidth;
            floodFill({1,OFFSET*4 + 10}, color == -1 ? WHITE : ColorSet[color]);
         }    
         continue;
      }
      if(tmp.is_move() && flag == 1){
         hp.st = {now.x, now.y}, hp.ed = {last.x, last.y};
         hp.shape = Shape;
         hp.size = NowSize;
         hp.color = BLACK;
         drawPattern(hp);
         now = tmp;
         hp.st = {now.x, now.y};
         hp.color = color == -1 ? WHITE : ColorSet[color];
         drawPattern(hp);
      }
      if(tmp.is_down() && !flag){
         flag = 1;
         last = tmp;
         now = tmp;
      }
      if(tmp.is_up() && flag){
         flag = 0;
         hp.st = {now.x, now.y}, hp.ed = {last.x, last.y};
         hp.shape = Shape;
         hp.size = NowSize;
         hp.color = BLACK;
         drawPattern(hp);
         now = tmp;

         hp.st = {now.x, now.y};
         hp.shape = Shape;
         hp.color = color == -1 ? WHITE : ColorSet[color];
         drawPattern(hp);
         graphic.push_back(hp);
      }
      reappear(graphic, color);
   }
   return now;
}
/*
   ---------------------------
   生成圆信息，转发给drawPattern
   ---------------------------
*/
msg drawCircle(vector<DrawMessage> &graphic, int &color){
   int flag = 0;
   msg now, tmp, last;
   DrawMessage hp;
   int Shape = 3;
   int r;
   while(is_run()){
      while(mousemsg()){
         tmp = getmouse();
         NOWSTATE = tmp;
         reappear(graphic, color); 
      }
      if((tmp.x <=OFFSET) || (tmp.y <= OFFSET)){
         if(tmp.is_down()){
            if(tmp.x <= OFFSET && (tmp.y >= PITCKUP*OFFSET && tmp.y <= (PITCKUP+1)*OFFSET))   continue;
            if(tmp.x <= OFFSET)  return tmp;
            color = (tmp.x - OFFSET) / ColorWidth;
            floodFill({1,OFFSET*4 + 10}, color == -1 ? WHITE : ColorSet[color]);
         }    
         continue;
      }
      if(tmp.is_move() && flag == 1){
         hp.ed = {now.x, now.y}, hp.st = {last.x, last.y};
         hp.shape = Shape;
         hp.size = NowSize;
         hp.color = BLACK;
         drawPattern(hp);
         now = tmp;
         hp.ed = {now.x, now.y};
         hp.shape = Shape;
         hp.color = color == -1 ? WHITE : ColorSet[color];
         drawPattern(hp);
      }
      if(tmp.is_down() && !flag){
         flag = 1;
         last = tmp;
         now = tmp;
      }
      if(tmp.is_up() && flag){
         flag = 0;
         // r = sqrt((now.x - last.x) * (now.x - last.x) + (now.y - last.y) * (now.y - last.y));
         // if(last.x + r >= DEVICE_WIDTH || last.x - r <= OFFSET || last.y - r <= OFFSET || last.y + r >= DEVICE_HEIGHT)  continue;
         hp.ed = {now.x, now.y}, hp.st = {last.x, last.y};
         hp.shape = Shape;
         hp.size = NowSize;
         hp.color = BLACK;

         drawPattern(hp);
         now = tmp;
         // if(color != -1) hp.color = ColorSet[color];
         // else  hp.color = WHITE;
         hp.ed = {now.x, now.y}; hp.r = sqrt((now.x - last.x) * (now.x - last.x) + (now.y - last.y) * (now.y - last.y));
         hp.shape = Shape; hp.color = color == -1 ? WHITE : ColorSet[color];
         drawPattern(hp);
         graphic.push_back(hp);
      }
      reappear(graphic, color);
   }
   return now;
}
/*
   ---------------------------
   产生填充信息，转发给drawPattern
   ---------------------------
*/
msg fullColor(vector<DrawMessage> &graphic, int color){
   int flag = 0;
   msg now;
   DrawMessage hp;
   cout << "----" << endl;
   int index = -1;
   while(is_run()){
      while(mousemsg()){
         now = getmouse();
         NOWSTATE = now;
         reappear(graphic, getpixel(1, OFFSET*4 + 10));
      }
      
      if((now.x <=OFFSET) || (now.y <= OFFSET)){
         cout << now.x << " " << now.y << endl; 
         if(now.is_down()){
            if(now.x <= OFFSET && (now.y >= PITCKUP*OFFSET && now.y <= (PITCKUP+1)*OFFSET))  continue;   
            // if(now.x <= OFFSET)  return now;
            color = (now.x - OFFSET) / ColorWidth;
            floodFill({1,OFFSET*4 + 10}, color == -1 ? WHITE : ColorSet[color]);
         }    
         continue;
      }
      if(now.is_down()){
         cout << now.x << "+++++++++++++" << now.y << endl;
         floodFill({now.x, now.y}, color == -1 ? WHITE : ColorSet[color]);
         Sleep(20);
      }
      // if(now.is_down()){
      //    if((now.x <=OFFSET) || (now.y <= OFFSET)){
      //       if(now.x <= OFFSET && (now.y >= PITCKUP*OFFSET && now.y <= (PITCKUP+1)*OFFSET))   continue;
      //       if(now.x <= OFFSET)  return now;
      //       else{
      //          index = (now.x - OFFSET) / ColorWidth;
      //          floodFill({1,OFFSET*4 + 10}, index == -1 ? WHITE : ColorSet[index]);
      //       }
      //    }else{
      //       floodFill({now.x, now.y}, index == -1 ? WHITE : ColorSet[index]);
      //       FULLpoint.point[FULLpoint.size] = {now.x, now.y};
      //       FULLpoint.color[FULLpoint.size++] = index == -1 ? WHITE : ColorSet[index];
      //    }
      // }

      // reappear(graphic, getpixel(1, OFFSET*4 + 10));
   }
   return now;
}
/*
   ---------------p------------
   铅笔功能
   ---------------------------
*/
msg pencial(vector<DrawMessage> &graphic, int color){
   int flag = 0;
   msg tmp, last;
   while(is_run()){
      
      while(mousemsg()){
         tmp = getmouse();
         NOWSTATE = tmp;
         reappear(graphic, color);
      }
      if((tmp.x <=OFFSET) || (tmp.y <= OFFSET)){
         if(tmp.is_down()){
            if(tmp.x <= OFFSET && (tmp.y >= PITCKUP*OFFSET && tmp.y <= (PITCKUP+1)*OFFSET))   continue;
            if(tmp.x <= OFFSET)  return tmp;
            color = (tmp.x - OFFSET) / ColorWidth;
            floodFill({1,OFFSET*4 + 10}, color == -1 ? WHITE : ColorSet[color]);
         }    
         continue;
      }
      if(tmp.is_down()){
         flag = 1;
         last = tmp;
      }    
      if(tmp.is_up())      flag = 0;
      if(flag){
         setcolor(color == -1 ? WHITE : ColorSet[color]);
         for(int i = 0; i < (NowSize - 1) * 2 + 1; i ++ ){
            line(last.x + dic[i], last.y + dic[i], tmp.x + dic[i], tmp.y + dic[i]);
         }
         // line(last.x, last.y, tmp.x, tmp.y);
         // cout << tmp.x << " " << tmp.y << endl;
         // PencialPoint[{{last.x, last.y}, {tmp.x, tmp.y}}] = color;
         PencialPoint.push_back({{last.x, last.y}, {tmp.x, tmp.y}, color, NowSize});

         last = tmp;
      }
      reappear(graphic, color); 
   }
   return tmp;
}
/*
   ---------------------------
   重构功能栏（适用于刷新）
      解决画图溢出而遮盖功能栏的问题
   ---------------------------
*/
void buildItem(int index){
   for(int i = 0; i*OFFSET < DEVICE_HEIGHT; i ++ ){
      // floodfill(1, i * OFFSET+10, WHITE);
      setcolor(FunctionITEM[i]);    
      rectangle(0, i * OFFSET, OFFSET, (i + 1 ) * OFFSET);
      
   }
   color_t currentColor = index == -1 ? EGERGB(250, 250, 30) : ColorSet[index];
   setcolor(currentColor);
   line(30, 30, 90, 90);
   rectangle(31, OFFSET + 30, 90, OFFSET + 90);
   circle(OFFSET / 2, OFFSET * 2 + OFFSET / 2, 30);
   int x = OFFSET, add = ColorWidth, k;
   for(int i = x, k = 0; i < DEVICE_WIDTH; i += add, k ++ ){
      setfillcolor(ColorSet[k]);
      bar(i, 0, i + add, OFFSET);
   }
   for(int i = x, k = 0; i < DEVICE_WIDTH; i += add, k ++ ){
      setfillcolor(ColorSet[k]);
      bar(i, 0, i + add, OFFSET);
   }
   // for(map<point, int>::iterator it = PencialPoint.begin(); it != PencialPoint.end(); it ++ ){
   //    setcolor((*it).second == -1 ? WHITE : ColorSet[(*it).second]);
   //    line((*it).first.st.first, (*it).first.st.second, (*it).first.ed.first, (*it).first.ed.second);
   // }
   for(vector<point>::iterator it = PencialPoint.begin(); it != PencialPoint.end(); it ++ ){
      setcolor((*it).color == -1 ? WHITE : ColorSet[(*it).color]);
      for(int i = 0; i < ((*it).size - 1) * 2 + 1; i ++ ){
         line((*it).st.first + dic[i], (*it).st.second + dic[i], (*it).ed.first + dic[i], (*it).ed.second + dic[i]);
      }
      // line((*it).st.first, (*it).st.second, (*it).ed.first, (*it).ed.second);
   }
   //设置LABEL
   setcolor(EGERGB(250, 250, 30));
   for(int i = 1; i <= 8; i ++ ){
      xyprintf(LABEL[i].first, LABEL[i].second, "%s", SHAPE[i]);
   }
   // cout << PITCKUP + 1 << endl;
   setcolor(WHITE);  xyprintf(LABEL[PITCKUP+1].first, LABEL[PITCKUP+1].second, "%s", SHAPE[PITCKUP+1]);

   floodFill({1,OFFSET*4 + 10}, index == -1 ? WHITE : ColorSet[index]);
   // floodFill({1,OFFSET*PITCKUP + 10}, WHITE);      /选中状态
   setcolor(index == -1 ? WHITE : ColorSet[index]);
   xyprintf(OFFSET + 20, OFFSET + 40, "Username : %s ", NAME);
   
   xyprintf(LOCATION[0].first, LOCATION[0].second, "x : %08d, y : %08d", NOWSTATE.x, NOWSTATE.y);
   xyprintf(LOCATION[1].first, LOCATION[1].second,  "is_down : %s", NOWSTATE.is_down() ? "True " : "False");
   xyprintf(LOCATION[2].first, LOCATION[2].second,  "is_up : %s", NOWSTATE.is_up() ? "True " : "False");
   // setcolor(index == -1 ? WHITE : ColorSet[index]);
   xyprintf(LOCATION[3].first, LOCATION[3].second,  "NowSize : %d ", NowSize);
   setcolor(WHITE);
   xyprintf(OFFSET + 20, OFFSET + 20, "Version : %s", VERSION);
   // cout << FULLpoint.size << "======" << endl;
   for(int i = 0; i < FULLpoint.size; i ++ ){
      floodFill(FULLpoint.point[i], FULLpoint.color[i]);
   }
   delay_fps(3000);
}
/*
   ---------------------------
   清理画板以及更换主题功能
   ---------------------------
*/
void reBuild(){
   int r, g, b;
   for(int i = 0; i*OFFSET < DEVICE_HEIGHT; i ++ ){
      r = random(255), g = random(255), b = random(255);
      setcolor(EGERGB(r, g, b));    
      FunctionITEM[i] = EGERGB(r, g, b);
      rectangle(0, i * OFFSET, OFFSET, (i + 1 ) * OFFSET);
   }
   // setcolor(EGERGB(250, 250, 30));
   // line(30, 30, 90, 90);
   // rectangle(31, OFFSET + 30, 90, OFFSET + 90);
   // circle(OFFSET / 2, OFFSET * 2 + OFFSET / 2, 30);
   // int x = OFFSET, add = ColorWidth, k;
   // for(int i = x, k = 0; i < DEVICE_WIDTH; i += add, k ++ ){
   //    setfillcolor(ColorSet[k]);
   //    bar(i, 0, i + add, OFFSET);
   // }
   // //设置VERSION USENAME
   // xyprintf(OFFSET + 20, OFFSET + 40, "Username : %s ", NAME);
   // // xyprintf(OFFSET + 20, OFFSET + 40, "username ： %s", NAME);
   // setcolor(WHITE);
   // xyprintf(OFFSET + 20, OFFSET + 20, "Version : %s", VERSION);
   // //设置LABEL
   // setcolor(EGERGB(250, 250, 30));
   // for(int i = 1; i <= 6; i ++ ){
   //    xyprintf(LABEL[i].first, LABEL[i].second, "%s", SHAPE[i]);
   // }
   // floodFill({1,OFFSET*4 + 10}, WHITE);     //选中状态
}
/*
   ---------------------------
   画图主函数
   ---------------------------
*/
void work(){
   msg now;
   int state = -1;
   int index = -1;
   int flag = 0;
   vector<DrawMessage> graphic;
   while(is_run()){
      while(mousemsg()){
         now = getmouse();
         NOWSTATE = now;
      }
      if(now.is_down()){
         // NowSize = ((NowSize + 1) % 5 + 1);
         if(now.x >= OFFSET && now.x <= DEVICE_WIDTH && now.y <= OFFSET && now.y >= 0){
            index = (now.x - OFFSET) / ColorWidth;
            floodFill({1,OFFSET*4 + 10}, index == -1 ? WHITE : ColorSet[index]);
         } 
         if(now.x <= OFFSET){
            // if(state == now.y / OFFSET)   continue;
            floodFill({1,OFFSET*state + 10}, BLACK);
            state = now.y / OFFSET;
            PITCKUP = state;
            cout << state << endl;
            setcolor(WHITE);  xyprintf(LABEL[PITCKUP+1].first, LABEL[PITCKUP+1].second, "%s", SHAPE[PITCKUP+1]);
            cout << state << endl;
            switch(state){
               case 0 : { now = drawline(graphic, index); break; }
               case 1 : { now = drawbar(graphic, index); break;}
               case 2 : { now = drawCircle(graphic, index); break;}
               case 3 : { cleardevice(); reBuild(); graphic.clear(); PencialPoint.clear(); break;}
               case 5 : { now = fullColor(graphic, index);cout << "++++++ " << endl; break;}
               case 6 : { NowSize = (NowSize % 5 + 1); Sleep(100); break;}
               case 7 : { now = pencial(graphic, index); break;}
            }
         }
      }
      // if(now.is_move()) {
      //    NowSize = ((NowSize + 1) % 5 + 1);
      // }  
      reappear(graphic, index);
      // delay_fps(1200);
   }
}
/*
   ---------------------------
   画板初始化
   ---------------------------
*/
void init(){
   OFFSET =  min(min(DEVICE_WIDTH/6.0, DEVICE_HEIGHT/6.0), 120.0);
/*
   带边框效果
   setcolor(EGERGB(254, 130, 60));
   line(OFFSET, 0, OFFSET ,DEVICE_HEIGHT);
   line(OFFSET, OFFSET, DEVICE_WIDTH, OFFSET);
   rectangle(0, 0, DEVICE_WIDTH, DEVICE_HEIGHT);
*/
   // randomize();
   int r, g, b;
   cout << "---------------------------------------------" << endl;
   cout << "进行初始化功能栏：" << endl;
   for(int i = 0; i*OFFSET < DEVICE_HEIGHT && i <= 8; i ++ ){
      r = random(255), g = random(255), b = random(255);
      setcolor(EGERGB(r, g, b));    
      FunctionITEM[i] = EGERGB(r, g, b);
      rectangle(0, i * OFFSET, OFFSET, (i + 1 ) * OFFSET);
      cout << "-- 生成 ：" << SHAPE[i+1] << "功能栏" << endl;
   }
   cout << "---------------------------------------------" << endl;
   setcolor(EGERGB(250, 250, 30));    //鲜艳黄
   cout << "-- 初始化功能提示" << endl;
   line(30, 30, 90, 90);
   rectangle(31, OFFSET + 30, 90, OFFSET + 90);
   circle(OFFSET / 2, OFFSET * 2 + OFFSET / 2, 30);
   //初始化LABEL
   LABEL[1] = {10, OFFSET - 20}; LABEL[2] = {10, OFFSET * 2 - 20};
   LABEL[3] = {10, OFFSET * 3 - 20}; LABEL[4] = {15, OFFSET * 4 - 65};
   LABEL[5] = {15, OFFSET * 5 - 65}; LABEL[6] = {30, OFFSET * 6 - 65}; 
   LABEL[7] = {40, OFFSET * 7 - 65}; LABEL[8] = {35, OFFSET * 8 - 65};
   for(int i = 1; i <= 8; i ++ ){
      xyprintf(LABEL[i].first, LABEL[i].second, "%s", SHAPE[i]);
      cout << "---- 提示 :" << SHAPE[i] << endl; 
   }
   int x = OFFSET, add = ColorWidth;
   COLORSIZE = 0;
   cout << "初始化调色板：" << endl;
   for(int i = x; i < DEVICE_WIDTH; i += add, COLORSIZE ++ ){
      r = random(255), g = random(255), b = random(255);
      ColorSet[COLORSIZE] = EGERGB(r, g, b);
      setfillcolor(ColorSet[COLORSIZE]);
      cout << "---- 生成 " << EGERGB(r, g, b) << endl;
      bar(i, 0, i + add, OFFSET);
   }
   // floodFill({1,OFFSET*4 + 10}, WHITE);    //选中状态
   setcolor(EGERGB(250, 250, 30));
   xyprintf(OFFSET + 20, OFFSET + 40, "Username : %s ", NAME);
   setcolor(WHITE);
   xyprintf(OFFSET + 20, OFFSET + 20, "Version : %s", VERSION);
   LOCATION[0] = {OFFSET + 20, OFFSET + 60};
   LOCATION[1] = {OFFSET + 20, OFFSET + 80};
   LOCATION[2] = {OFFSET + 20, OFFSET + 100};
   LOCATION[3] = {OFFSET + 20, OFFSET + 120};
   xyprintf(OFFSET + 20, OFFSET + 60,  "x : %08d, y : %08d", 0, 0);
   xyprintf(OFFSET + 20, OFFSET + 80,  "is_down : %s", "False");
   xyprintf(OFFSET + 20, OFFSET + 100,  "is_up : %s", "False");
   NowSize = 1;
   xyprintf(OFFSET + 20, OFFSET + 120,  "NowSize : %d", NowSize);
   delay_fps(3000);
   FunctionArea[0] = {OFFSET, OFFSET};
   work();
}
int main(){
   // cout << "请输入画板的大小（width-height[系统默认720*720为最小大小]）：";
   // cin >> DEVICE_HEIGHT >> DEVICE_WIDTH;
   // if(DEVICE_HEIGHT < 720 || DEVICE_WIDTH < 720)  {DEVICE_HEIGHT = 720, DEVICE_WIDTH = 720};
   DEVICE_HEIGHT = 2000, DEVICE_WIDTH = 2200;
   // initgraph(DEVICE_WIDTH, DEVICE_HEIGHT);
   cout << "输入你的姓名：";
   cin >> NAME;
   // DEVICE_HEIGHT = 2000, DEVICE_WIDTH = 2000;
   setinitmode(INIT_NOBORDER);
   initgraph(-1, -1);
   setbkcolor(BLACK);
   cout << "进行初始化：" << endl;
   init();
   closegraph();
   return 0;
}