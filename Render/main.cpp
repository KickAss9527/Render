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
#include "lxMath.h"
#include "engine.h"
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
static int slices = 16;
static int stacks = 16;

/* GLUT callback Handlers */

void myDisplay ()
{
    POINT4D cam_pos = {25,33,0,1};
    VECTOR4D cam_dir = {0,0,0,1};
    VECTOR4D vscale = {1,1,1,1}, vpos = {0,0,0,1}, vrot = {0,0,0,1};
    CAM4DV1 cam;
    OBJECT4DV1 obj;
    Init_CAM4DV1(&cam, &cam_pos, &cam_dir, 50,500,90, 500,500);
#ifdef __APPLE__
    Load_OBJECT4DV1_PLG(&obj,"/MyFiles/Work/GitProject/Render/Render/tower1.plg", &vscale, &vpos, &vrot);
#else
    Load_OBJECT4DV1_PLG(&obj,"C:\\Users\\Administrator\\Desktop\\git\\Render\\Render\\tower1.plg", &vscale, &vpos, &vrot);
#endif
    obj.world_pos.z=60;

    Model_To_World_OBJECT4DV1(&obj);
    Build_CAM4DV1_Matrix_Euler(&cam, CAM_ROT_SEQ_ZYX);
    Remove_Backfaces_OBJECT4DV1(&obj, &cam);
    World_To_Camera_OBJECT4DV1(&cam, &obj);
    Camera_To_Perspective_OBJECT4DV1(&obj, &cam);
    Perspective_To_Screen_OBJECT4DV1(&obj, &cam);

    glClear (GL_COLOR_BUFFER_BIT);//清空屏幕上的颜色
    glColor3f (1.0, 1.0, 0.0);//设置当前画笔颜色




    for(int poly=0; poly<obj.num_polys; poly++)
    {

        if(!(obj.plist[poly].state & POLY4DV1_STATE_ACTIVE) ||
           (obj.plist[poly].state & POLY4DV1_STATE_CLIPPED) ||
           (obj.plist[poly].state & POLY4DV1_STATE_BACKFACE))
        {
            continue;
        }

        int vindex_0 = obj.plist[poly].vert[0];
        int vindex_1 = obj.plist[poly].vert[1];
        int vindex_2 = obj.plist[poly].vert[2];
        printf("(%f,%f), (%f,%f),(%f,%f)\n",
               obj.vlist_trans[vindex_0].x,obj.vlist_trans[vindex_0].y,
               obj.vlist_trans[vindex_1].x,obj.vlist_trans[vindex_1].y,
               obj.vlist_trans[vindex_2].x,obj.vlist_trans[vindex_2].y);
        int w = 250;
        glBegin (GL_LINES);
        glVertex2f((obj.vlist_trans[vindex_0].x-w)/w, (w-obj.vlist_trans[vindex_0].y)/w);
        glVertex2f((obj.vlist_trans[vindex_1].x-w)/w, (w-obj.vlist_trans[vindex_1].y)/w);
        glEnd ();

        glBegin (GL_LINES);
        glVertex2f((obj.vlist_trans[vindex_2].x-w)/w, (w-obj.vlist_trans[vindex_2].y)/w);
        glVertex2f((obj.vlist_trans[vindex_1].x-w)/w, (w-obj.vlist_trans[vindex_1].y)/w);
        glEnd ();

        glBegin (GL_LINES);
        glVertex2f((obj.vlist_trans[vindex_2].x-w)/w, (w-obj.vlist_trans[vindex_2].y)/w);
        glVertex2f((obj.vlist_trans[vindex_0].x-w)/w, (w-obj.vlist_trans[vindex_0].y)/w);
        glEnd ();

    }

glFlush();//要加上,不然会很慢的,作用是,保证前面的OpenGL命令立即执行,而不是在缓冲区中等着
}

int main(int argc, char *argv[])
{



glutInit(&argc, argv);//初始化,必须在调用其他GLUT函数前调用一下
glutInitDisplayMode (GLUT_RGBA | GLUT_SINGLE);//设定模式,RGBA色彩,和单缓冲区
glutInitWindowPosition (100, 100);//设置窗口位置,如果设-1,-1就是默认位置
glutInitWindowSize (500, 500);//设置窗口大小
glutCreateWindow ("hello word!");//创建名称为"hello word!"的窗口,窗口创建后不会立即显示到屏幕上,要调用后面的glutMainLoop()才会显示
glutDisplayFunc(myDisplay);//调用绘制函数使它显示在刚创建的窗口上
glutMainLoop();//消息循环,窗口关闭才会返回
return 0;
}
