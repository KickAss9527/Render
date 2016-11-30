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
int sSize = 800;


void drawLine(POINT4D_PTR p0, POINT4D_PTR p1)
{
    int w = 250;
    POINT2D np0 = {(p0->x-w)/w, (w-p0->y)/w};
    POINT2D np1 = {(p1->x-w)/w, (w-p1->y)/w};
    glBegin (GL_LINES);
    glVertex2f(np0.x, np0.y);
    glVertex2f(np1.x, np1.y);
    glEnd ();
}
/* GLUT callback Handlers */
void drawTrangleBottomPlane(POINT4D_PTR pt, POINT4D_PTR pl, POINT4D_PTR pr)
{
    float dl = (pl->x - pt->x)/(pl->y - pt->y);
    float dr = (pr->x - pt->x)/(pr->y - pt->y);
    float xl=pt->x, xr=pt->x;
    for (int y=pt->y; y<=pl->y; y++)
    {
        POINT4D p0 = {static_cast<float>(xl+0.5), static_cast<float>(y)};
        POINT4D p1 = {static_cast<float>(xr+0.5), static_cast<float>(y)};
        drawLine(&p0, &p1);
        xl += dl;
        xr += dr;
    }
}

void drawTrangleTopPlane(POINT4D_PTR pb, POINT4D_PTR pl, POINT4D_PTR pr)
{
    float dl = (pb->x - pl->x)/(pb->y - pl->y);
    float dr = (pb->x - pr->x)/(pb->y - pr->y);
    float xl=pl->x, xr=pr->x;
    for (int y=pl->y; y<=pb->y; y++)
    {
        POINT4D p0 = {static_cast<float>(xl+0.5), static_cast<float>(y)};
        POINT4D p1 = {static_cast<float>(xr+0.5), static_cast<float>(y)};
        drawLine(&p0, &p1);
        xl += dl;
        xr += dr;
    }
}

void drawTrangle(POINT4D_PTR p0, POINT4D_PTR p1, POINT4D_PTR p2)
{
    if (p0->y == p1->y)
    {
        if (p2->y >= p0->y)//top plane
        {
            drawTrangleTopPlane(p2, p0, p1);
        }
        else//
        {
            drawTrangleBottomPlane(p2, p0, p1);
        }
    }
    else if (p0->y == p2->y)
    {
        if (p1->y >= p0->y)//
        {
            drawTrangleTopPlane(p1, p0, p2);
        }
        else//
        {
            drawTrangleBottomPlane(p1, p0, p2);
        }
    }
    else if (p2->y == p1->y)
    {
        if (p0->y >= p2->y)//
        {
            drawTrangleTopPlane(p0, p2, p1);
        }
        else//
        {
            drawTrangleBottomPlane(p0, p2, p1);
        }
    }
    else
    {
        POINT4D_PTR pt, pm, pb;
        if(p0->y < p1->y && p1->y < p2->y)
        {
            pt = p0;
            pm = p1;
            pb = p2;
        }
        else if(p0->y < p2->y && p2->y < p1->y)
        {
            pt = p0;
            pm = p2;
            pb = p1;
        }
        else if(p1->y < p0->y && p0->y < p2->y)
        {
            pt = p1;
            pm = p0;
            pb = p2;
        }
        else if(p1->y < p2->y && p2->y < p0->y)
        {
            pt = p1;
            pm = p2;
            pb = p0;
        }
        else if(p2->y < p0->y && p0->y < p1->y)
        {
            pt = p2;
            pm = p0;
            pb = p1;
        }
        else if(p2->y < p1->y && p1->y < p0->y)
        {
            pt = p2;
            pm = p1;
            pb = p0;
        }

        int xline = (pm->y - pt->y)*(pb->x - pt->x)/(pb->y - pt->y);
        xline += pt->x + 0.5;

        POINT4D pTmp = {static_cast<float>(xline), pm->y,1,1};
        drawTrangleBottomPlane(pt, pm, &pTmp);
        drawTrangleTopPlane(pb, pm, &pTmp);
    }
}


void myDisplay ()
{
    POINT4D cam_pos = {31,31,0,1};
    VECTOR4D cam_dir = {0,0,0,1};
    VECTOR4D vscale = {1,1,1,1}, vpos = {0,0,0,1}, vrot = {0,0,0,1};
    CAM4DV1 cam;
    OBJECT4DV1 obj;
    Init_CAM4DV1(&cam, &cam_pos, &cam_dir, 50,sSize,90, sSize,sSize);
#ifdef __APPLE__
    Load_OBJECT4DV1_PLG(&obj,"/MyFiles/Work/GitProject/Render/Render/tower1.plg", &vscale, &vpos, &vrot);
#else
    Load_OBJECT4DV1_PLG(&obj,"C:\\Users\\Administrator\\Desktop\\git\\Render\\Render\\tower1.plg", &vscale, &vpos, &vrot);
#endif
    obj.world_pos.z=160;

    Model_To_World_OBJECT4DV1(&obj);
    Build_CAM4DV1_Matrix_Euler(&cam, CAM_ROT_SEQ_ZYX);
    Remove_Backfaces_OBJECT4DV1(&obj, &cam);
    World_To_Camera_OBJECT4DV1(&cam, &obj);
    Camera_To_Perspective_OBJECT4DV1(&obj, &cam);
    Perspective_To_Screen_OBJECT4DV1(&obj, &cam);

    glClear (GL_COLOR_BUFFER_BIT);//«Âø’∆¡ƒª…œµƒ—’…´





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

        glColor3f (0.3, 0.3, 0.3);//…Ë÷√µ±«∞ª≠± —’…´
        drawTrangle(&obj.vlist_trans[vindex_0],&obj.vlist_trans[vindex_1],&obj.vlist_trans[vindex_2]);
        glColor3f (1.0, 1.0, 0.0);//…Ë÷√µ±«∞ª≠± —’…´
        drawLine(&obj.vlist_trans[vindex_0], &obj.vlist_trans[vindex_1]);
        drawLine(&obj.vlist_trans[vindex_2], &obj.vlist_trans[vindex_1]);
        drawLine(&obj.vlist_trans[vindex_2], &obj.vlist_trans[vindex_0]);
    }

    glFlush();//“™º”…œ,≤ª»ªª·∫‹¬˝µƒ,◊˜”√ «,±£÷§«∞√ÊµƒOpenGL√¸¡Ó¡¢º¥÷¥––,∂¯≤ª «‘⁄ª∫≥Â«¯÷–µ»◊≈
}




int main(int argc, char *argv[])
{
    VECTOR4D sun_pos = {0, 10000, 0, 0};
    RGBAV1 c0 = {0};
    RGBAV1 c1;
    c1.r = 255;
    c1.g = 255;
    RGBAV1 c2 = {0};
    int sun_light = Init_Light_LIGHTV1(1,
                                       LIGHTV1_STATE_ON,
                                       LIGHTV1_ATTR_POINT,
                                       c0, c1, c2,
                                       &sun_pos, NULL,
                                       0, 1, 0,
                                       0, 0, 0);

    glutInit(&argc, argv);//≥ı ºªØ,±ÿ–Î‘⁄µ˜”√∆‰À˚GLUT∫Ø ˝«∞µ˜”√“ªœ¬
    glutInitDisplayMode (GLUT_RGBA | GLUT_SINGLE);//…Ë∂®ƒ£ Ω,RGBA…´≤ ,∫Õµ•ª∫≥Â«¯
    glutInitWindowPosition (100, 100);//…Ë÷√¥∞ø⁄Œª÷√,»Áπ˚…Ë-1,-1æÕ «ƒ¨»œŒª÷√
    glutInitWindowSize (sSize, sSize);//…Ë÷√¥∞ø⁄¥Û–°
    glutCreateWindow ("hello word!");//¥¥Ω®√˚≥∆Œ™"hello word!"µƒ¥∞ø⁄,¥∞ø⁄¥¥Ω®∫Û≤ªª·¡¢º¥œ‘ æµΩ∆¡ƒª…œ,“™µ˜”√∫Û√ÊµƒglutMainLoop()≤≈ª·œ‘ æ
    glutDisplayFunc(myDisplay);//µ˜”√ªÊ÷∆∫Ø ˝ πÀ¸œ‘ æ‘⁄∏’¥¥Ω®µƒ¥∞ø⁄…œ
    glutMainLoop();//œ˚œ¢—≠ª∑,¥∞ø⁄πÿ±’≤≈ª·∑µªÿ
    return 0;
}
