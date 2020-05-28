#include <iostream>
#include <graphics.h>
#include <cmath>
#define PI 3.1415
using namespace std;
typedef pair<int, int> pii;
typedef pair<double, double> dPII;
struct Triangle{
    dPII a, b, c;
};
pii getMidPoint(pii a, pii b){
    return {a.first + b.first >> 1, a.second + b.second >> 1};
}
void drapTriangle(Triangle t, color_t color){
    setcolor(color);
    line(t.a.first, t.a.second, t.b.first, t.b.second);
    line(t.a.first, t.a.second, t.c.first, t.c.second);
    line(t.c.first, t.c.second, t.b.first, t.b.second);
}
color_t baseColor[3] = {RED, GREEN, BLUE};
const int N = 6;
void sierprinski(Triangle t, int depth, int colorIndex){
    drapTriangle(t, baseColor[colorIndex]);
    pii atmp = t.a;
    pii btmp = t.b;
    pii ctmp = t.c;
    if(depth == 0)  return ;
    Triangle NewTriangle;
    NewTriangle.a = atmp;
    NewTriangle.b = getMidPoint(atmp, btmp);
    NewTriangle.c = getMidPoint(atmp, ctmp);
    if(depth == N){
        sierprinski(NewTriangle, depth - 1, 0);
    }else   sierprinski(NewTriangle, depth - 1, colorIndex);
    NewTriangle.a = NewTriangle.b;
    NewTriangle.b = btmp;
    NewTriangle.c = getMidPoint(btmp, ctmp);
    if(depth == N){
        sierprinski(NewTriangle, depth - 1, 1);
    }else   sierprinski(NewTriangle, depth - 1, colorIndex);
    NewTriangle.a = getMidPoint(atmp, ctmp);
    NewTriangle.b = getMidPoint(btmp, ctmp);
    NewTriangle.c = ctmp;
    if(depth == N){
        sierprinski(NewTriangle, depth - 1, 2);
    }else   sierprinski(NewTriangle, depth - 1, colorIndex);
}
// x1=xcos(β)+ysin(β);
// y1=ycos(β)-xsin(β);
pii rotate(pii t, int degrees){
    float d = degrees * 1.0 / 180.0 * PI;
    return {t.first * cos(d) + t.second * sin(d), t.second * cos(d) - t.first * sin(d)};
}
pii add(pii a, pii b){
    return {a.first + b.first, a.second + b.second};
}
void drapBar(pii t, int Blen, int Bsize){
    setcolor(EGERGB(128, 118, 105));
    t.first -= Bsize / 2;
    for(int i = 0; i < Bsize; i ++ ){
        line(t.first + i, t.second, t.first + i, t.second + Blen);
    }
}
/*
    way???????-1 ? 1?
*/
void kochCurve(dPII st, dPII ed, int depth, color_t color, int way){
    dPII point1, point2, point3;
    point1 = {2.0 / 3 * st.first + 1.0 / 3 * ed.first, 2.0 / 3 * st.second + 1.0 / 3 * ed.second};
    point3 = {1.0 / 3 * st.first + 2.0 / 3 * ed.first, 1.0 / 3 * st.second + 2.0 / 3 * ed.second};
    point2 = {(st.first + ed.first) / 2.0 - way * sqrt(3.0) / 6 * (ed.second - st.second), 
                (st.second + ed.second) / 2.0 + way * sqrt(3.0) / 6 * (ed.first - st.first)};
    if(depth == 0){
        setcolor(color);
        line(st.first, st.second, point2.first, point1.second);
        line(point1.first, point1.second, point2.first, point2.second);
        line(point2.first, point2.second, point3.first, point3.second);
        line(point3.first, point3.second, ed.first, ed.second);
    }else{
        kochCurve(st, point1, depth - 1, color, way);
        kochCurve(point1, point2, depth - 1, color, way);
        kochCurve(point2, point3, depth - 1, color, way);
        kochCurve(point3, ed, depth - 1, color, way);
    }
}
void beautifulSnow(Triangle t, int depth){
    kochCurve(t.a, t.b, depth, WHITE, -1);
    kochCurve(t.a, t.c, depth, WHITE, 1);
    kochCurve(t.b, t.c, depth, WHITE, -1);
}
Triangle snowside[10];
void initSnow(){
    snowside[0] = {{50, 50}, {150, 50}, {100, 150}};
    snowside[1] = {{400, 100}, {440, 100}, {420, 140}};
    snowside[2] = {{}, {}, {}};
    snowside[3] = {{}, {}, {}};
    snowside[4] = {{}, {}, {}};
    snowside[5] = {{}, {}, {}};
}
int main(){
    initSnow();
    initgraph(1500, 900);
    Triangle triangle;
    triangle.a = {350, 100};
    triangle.b = {750, 100};
    triangle.c = {550, 400};
    dPII st = {800, 300};
    dPII ed = {1300, 300};
    int degrees = 115;
    int num = 1;
    // beautifulSnow(triangle, 6);
    while(true){
        if(num == 1){
            setcolor(EGERGB(0, 255, 255));
            double r = sqrt((triangle.a.first - triangle.c.first) * (triangle.a.first - triangle.c.first) + (triangle.a.second - triangle.c.second) * (triangle.a.second - triangle.c.second));
            circle(triangle.c.first, triangle.c.second, r);
            kochCurve(st, ed, 4, WHITE, 1);
            beautifulSnow(snowside[0], 4);
            beautifulSnow(snowside[1], 4);
            kochCurve({300, 300}, {400, 300}, 4, EGERGB(255, 255, 0), 1);
            kochCurve({300, 600}, {300, 800}, 4, EGERGB(160, 32, 240), 1);
            kochCurve({600, 300}, {1000, 750}, 4, EGERGB(255, 0, 255), -1);
            drapBar(triangle.c, 600, 5);
        }    
        sierprinski(triangle, N, 0);
        triangle.a.first -= triangle.c.first;
        triangle.a.second -= triangle.c.second;
        triangle.a = add(rotate(triangle.a, degrees), triangle.c);
        triangle.b.first -= triangle.c.first;
        triangle.b.second -= triangle.c.second;
        triangle.b = add(rotate(triangle.b, degrees), triangle.c);
        if(num == 3){
            Sleep(1000);
            cleardevice();
            num = 0;
        }
        delay_fps(400);
        num ++ ;
    }
    getch();
    closegraph();
    return 0;
}