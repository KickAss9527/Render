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
CAM4DV1 gCam;
RENDERLIST4DV1 gRend_list;
bool isDrawWireframe = false;

void drawLine(POINT4D_PTR p0, POINT4D_PTR p1)
{
    int w = sSize*0.5;
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

void drawPoly(RENDERLIST4DV1_PTR rend_list)
{
    for(int poly=0; poly<rend_list->num_polys; poly++)
    {
        POLYF4DV1 curr_poly = *rend_list->poly_ptrs[poly];
        if(!(curr_poly.state & POLY4DV1_STATE_ACTIVE) ||
           (curr_poly.state & POLY4DV1_STATE_CLIPPED) ||
           (curr_poly.state & POLY4DV1_STATE_BACKFACE))
        {
            continue;
        }
        
        unsigned int r, g, b;
        int color = curr_poly.lcolor;
        RGB888FROM24BIT(color, &r, &g, &b);
        
        printf("(%d, %d, %d)\n", r, g, b);
        glColor3f (r/255.0, g/255.0, b/255.0);//…Ë÷√µ±«∞ª≠± —’…´
        drawTrangle(&curr_poly.tvlist[0],&curr_poly.tvlist[1],&curr_poly.tvlist[2]);
        glColor3f (1.0, 1.0, 0.0);//…Ë÷√µ±«∞ª≠± —’…´
        
        if (isDrawWireframe)
        {
            drawLine(&curr_poly.tvlist[0], &curr_poly.tvlist[1]);
            drawLine(&curr_poly.tvlist[2], &curr_poly.tvlist[1]);
            drawLine(&curr_poly.tvlist[2], &curr_poly.tvlist[0]);
        }

    }
}

int btnCnt = 6;
float btnSize = 60.0;

void drawDebugBtn()
{

    float tmpBtnS = btnSize*2/sSize;
    for (int i=0; i<btnCnt; i++)
    {
        glBegin (GL_POLYGON);
        glColor3f (arc4random()%255/255.0, arc4random()%255/255.0, arc4random()%255/255.0);
        float orign = -1;
        float x = i*tmpBtnS;
        glVertex2f( orign+x, orign );//
        glVertex2f( orign+x, orign+tmpBtnS );
        glVertex2f( orign+x+tmpBtnS, orign+tmpBtnS );
        glVertex2f( orign+x+tmpBtnS, orign );
        glEnd ();
    }

}


void myDisplay ()
{

    Light_RENDERLIST4DV1_World16(&gRend_list, &gCam, GetLightList(), 1);
    World_To_Camera_RENDERLIST4DV1(&gRend_list, &gCam);
    Sort_RENDERLIST4DV1(&gRend_list, SORT_POLYLIST_AVGZ);
    Camera_To_Perspective_RENDERLIST4DV1(&gRend_list, &gCam);
    Perspective_To_Screen_RENDERLIST4DV1(&gRend_list, &gCam);

    glClear (GL_COLOR_BUFFER_BIT);
    drawPoly(&gRend_list);
    drawDebugBtn();
    glFlush();
    
    for (int i=0; i<OBJECT4DV1_MAX_POLYS; i++)
    {
        POLYF4DV1_PTR p = gRend_list.poly_ptrs[i];
        if (!p)
        {
            break;
        }
        float z = p->tvlist[0].z+p->tvlist[1].z+p->tvlist[2].z;
        printf("idx : %d  z:%.2f \n", p->idx, z);
    }
}


void myMouse(int button,int state,int x,int y)
{
    if(state==GLUT_DOWN)
    {
        
        if (sSize - y <= btnSize)
        {
            int idx = x/btnSize;
            if (idx < btnCnt)
            {
                printf("click: %d", idx);
            }
        }
    }
}

void onTimer(int value)
{
    glutPostRedisplay();
    glutTimerFunc(200, onTimer, 1);
}

int main(int argc, char *argv[])
{

    RESET_RENDERLIST4DV1(&gRend_list);
    
    POINT4D cam_pos = {0,30,0,1};
    VECTOR4D cam_dir = {0,0,0,1};
    
    Init_CAM4DV1(&gCam, &cam_pos, &cam_dir, 50,sSize,90, sSize,sSize);
    Build_CAM4DV1_Matrix_Euler(&gCam, CAM_ROT_SEQ_ZYX);

    for ( int tower=0; tower<10; tower++)
    {
        OBJECT4DV1 obj;
        float scale = (50 + arc4random()%50)*0.01;
        scale = 0.5;
        int xt = 300;
        float x = xt*0.5 - arc4random()%xt;
        float z = 70 + arc4random()%130;
        VECTOR4D vscale = {scale,scale,scale,scale}, vpos = {x,0,z,1}, vrot = {0,0,0,1};
#ifdef __APPLE__
        Load_OBJECT4DV1_PLG(&obj,"/MyFiles/Work/GitProject/Render/Render/tower1.plg", &vscale, &vpos, &vrot);
#else
        Load_OBJECT4DV1_PLG(&obj,"C:\\Users\\Administrator\\Desktop\\git\\Render\\Render\\tower1.plg", &vscale, &vpos, &vrot);
#endif
        Model_To_World_OBJECT4DV1(&obj);
        Insert_OBJECT4DV1_RENDERLIST4DV1(&gRend_list, &obj, 0, 0);
        
    }
    
    for (int cube=0; cube < 30; cube++)
    {
        OBJECT4DV1 obj;
        float scale = (50 + arc4random()%50)*0.01;
        float r = 0.1*(arc4random()%100);
        int xt = 400;
        float x = xt*0.5 - arc4random()%xt;
        float z = 100 + arc4random()%300;
        VECTOR4D vscale = {scale,scale,scale,scale}, vpos = {x,0,z,1}, vrot = {r,r,r,1};
#ifdef __APPLE__
        Load_OBJECT4DV1_PLG(&obj,"/MyFiles/Work/GitProject/Render/Render/cube1.plg", &vscale, &vpos, &vrot);
#else
        Load_OBJECT4DV1_PLG(&obj,"C:\\Users\\Administrator\\Desktop\\git\\Render\\Render\\cube1.plg", &vscale, &vpos, &vrot);
#endif
        
        Model_To_World_OBJECT4DV1(&obj);
        Insert_OBJECT4DV1_RENDERLIST4DV1(&gRend_list, &obj, 0, 0);
    }
    
    Remove_Backfaces_RENDERLIST4DV1(&gRend_list, &gCam);
    
    Reset_Lights_LIGHTV1();
    VECTOR4D sun_pos = {0, 1000, 0, 0};
    RGBAV1 c0 = {0};
    RGBAV1 c1;
    c1.r = c1.g = c1.b = 255;
    RGBAV1 c2 = {0};
    Init_Light_LIGHTV1(0, LIGHTV1_STATE_ON, LIGHTV1_ATTR_AMBIENT, c1, c0, c2, NULL, NULL,
                       0, 0, 0, 0, 0, 0);
    
    for (int i=0; i<OBJECT4DV1_MAX_POLYS; i++)
    {
        POLYF4DV1_PTR p = gRend_list.poly_ptrs[i];
        if (!p)
        {
            break;
        }
        p->idx = i;
    }
    
    glutInit(&argc, argv);//≥ı ºªØ,±ÿ–Î‘⁄µ˜”√∆‰À˚GLUT∫Ø ˝«∞µ˜”√“ªœ¬
    glutInitDisplayMode (GLUT_RGBA | GLUT_SINGLE);//…Ë∂®ƒ£ Ω,RGBA…´≤ ,∫Õµ•ª∫≥Â«¯
    glutInitWindowPosition (100, 100);//…Ë÷√¥∞ø⁄Œª÷√,»Áπ˚…Ë-1,-1æÕ «ƒ¨»œŒª÷√
    glutInitWindowSize (sSize, sSize);//…Ë÷√¥∞ø⁄¥Û–°
    glutCreateWindow ("hello word!");//¥¥Ω®√˚≥∆Œ™"hello word!"µƒ¥∞ø⁄,¥∞ø⁄¥¥Ω®∫Û≤ªª·¡¢º¥œ‘ æµΩ∆¡ƒª…œ,“™µ˜”√∫Û√ÊµƒglutMainLoop()≤≈ª·œ‘ æ
    glutDisplayFunc(myDisplay);//µ˜”√ªÊ÷∆∫Ø ˝ πÀ¸œ‘ æ‘⁄∏’¥¥Ω®µƒ¥∞ø⁄…œ
//   glutTimerFunc(200, onTimer, 1);
    
    glutMouseFunc(myMouse);
    glutMainLoop();//œ˚œ¢—≠ª∑,¥∞ø⁄πÿ±’≤≈ª·∑µªÿ
    
    
    
    return 0;
}


