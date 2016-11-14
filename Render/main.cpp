/*
 * GLUT Shapes Demo
 *
 * Written by Nigel Stewart November 2003
 *
 * This program is test harness for the sphere, cone
 * and torus shapes in GLUT.
 *
 * Spinning wireframe and smooth shaded shapes are
 * displayed until the ESC or q key is pressed.  The
 * number of geometry stacks and slices can be adjusted
 * using the + and - keys.
 */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <windows.h>
#include <GL/glut.h>
#endif

#include <stdlib.h>

static int slices = 16;
static int stacks = 16;

/* GLUT callback Handlers */

void myDisplay (void)
{
glClear (GL_COLOR_BUFFER_BIT);//清空屏幕上的颜色
glColor3f (1.0, 1.0, 0.0);//设置当前画笔颜色
glBegin (GL_LINES);
glVertex2f (-0.5, 0.0);
glVertex2f (0.5, 0.0);
glEnd ();
glFlush();//要加上,不然会很慢的,作用是,保证前面的OpenGL命令立即执行,而不是在缓冲区中等着
}

int main(int argc, char *argv[])
{
glutInit(&argc, argv);//初始化,必须在调用其他GLUT函数前调用一下
glutInitDisplayMode (GLUT_RGBA | GLUT_SINGLE);//设定模式,RGBA色彩,和单缓冲区
glutInitWindowPosition (100, 100);//设置窗口位置,如果设-1,-1就是默认位置
glutInitWindowSize (500, 500);//设置窗口大小
glutCreateWindow ("hello word!");//创建名称为"hello word!"的窗口,窗口创建后不会立即显示到屏幕上,要调用后面的glutMainLoop()才会显示
glutDisplayFunc (myDisplay);//调用绘制函数使它显示在刚创建的窗口上
glutMainLoop();//消息循环,窗口关闭才会返回
return 0;
}
