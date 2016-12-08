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
RENDERLIST4DV2 gRend_list;
OBJECT4DV2 gAllObjects[100];
bool isDrawWireframe = 10;
int refreshFrequency = 30;

#define AMBIENT_LIGHT_INDEX   0 // ambient light index
#define INFINITE_LIGHT_INDEX  1 // infinite light index
#define POINT_LIGHT_INDEX     2 // point light index
#define SPOT_LIGHT_INDEX      3 // spot light index

void drawPoint(POINT4D_PTR p, RGBAV1_PTR color)
{
    int w = sSize*0.5;
    POINT2D np = {(p->x-w)/w, (w-p->y)/w};
    glBegin (GL_POINTS);
    glColor3f (color->r/255.0, color->g/255.0, color->b/255.0);
    glVertex2f(np.x, np.y);
    glEnd ();
}

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
void drawTranglePlane(POINT4D_PTR pt, POINT4D_PTR pm, POINT4D_PTR pb)
{
    float dl = (pm->x - pt->x)/(pm->y - pt->y);
    float dr = (pb->x - pt->x)/(pb->y - pt->y);
    float xl = pt->x;
    float xr = xl;
    
    for (int y=pt->y; y<=pb->y; y++)
    {
        POINT4D p0 = {static_cast<float>(xl), static_cast<float>(y)};
        POINT4D p1 = {static_cast<float>(xr), static_cast<float>(y)};
        drawLine(&p0, &p1);
        xl += dl;
        xr += dr;
        if (y+1 >= pm->y)
        {
            break;
        }
    }
    
    dl = (pb->x - pm->x)/(pb->y - pm->y);
    for (int y=pm->y; y<=pb->y; y++)
    {
        POINT4D p0 = {static_cast<float>(xl), static_cast<float>(y)};
        POINT4D p1 = {static_cast<float>(xr), static_cast<float>(y)};
        drawLine(&p0, &p1);
        xl += dl;
        xr += dr;
    }
}

void drawTranglePlaneGOURAUD(POINT4D_PTR pt, POINT4D_PTR pm, POINT4D_PTR pb, RGBAV1_PTR ct, RGBAV1_PTR cm, RGBAV1_PTR cb)
{
    float dl = (pm->x - pt->x)/(pm->y - pt->y);
    float dr = (pb->x - pt->x)/(pb->y - pt->y);
    float dcl_r = (cm->r - ct->r)/(pm->y - pt->y);
    float dcr_r = (cb->r - ct->r)/(pb->y - pt->y);
    float dcl_g = (cm->g - ct->g)/(pm->y - pt->y);
    float dcr_g = (cb->g - ct->g)/(pb->y - pt->y);
    float dcl_b = (cm->b - ct->b)/(pm->y - pt->y);
    float dcr_b = (cb->b - ct->b)/(pb->y - pt->y);
    
    float xl = pt->x;
    float xr = xl;
    float rl = ct->r;
    float rr = rl;
    float gl = ct->g;
    float gr = gl;
    float bl = ct->b;
    float br = bl;

    
    for (int y=pt->y; y<=pb->y; y++)
    {
        float tmpxl = xl;
        float tmpxr = xr;
        float tmpcrl = rl;
        float tmpcrr = rr;
        float tmpcgl = gl;
        float tmpcgr = gr;
        float tmpcbl = bl;
        float tmpcbr = br;
        if (xl > xr)
        {
            tmpxl = xr;
            tmpxr = xl;
            tmpcrl = rr;
            tmpcrr = rl;
            tmpcgl = gr;
            tmpcgr = gl;
            tmpcbl = br;
            tmpcbr = bl;
        }
        for (int x=tmpxl; x<=tmpxr; x++)
        {
            POINT4D p = {static_cast<float>(x), static_cast<float>(y)};
            float dx = (x-tmpxl)/(tmpxr-tmpxl);
            RGBAV1 c;
            int r = tmpcrl + (tmpcrr - tmpcrl)*dx;
            int g = tmpcgl + (tmpcgr - tmpcgl)*dx;
            int b = tmpcbl + (tmpcbr - tmpcbl)*dx;
            c.r = MIN(MAX(0, r), 255);
            c.g = MIN(MAX(0, g), 255);
            c.b = MIN(MAX(0, b), 255);
            drawPoint(&p, &c);
        }

        rl += dcl_r;
        rr += dcr_r;
        gl += dcl_g;
        gr += dcr_g;
        bl += dcl_b;
        br += dcr_b;
        xl += dl;
        xr += dr;
        if (y+1 >= pm->y)
        {
            break;
        }
    }
    
    dl = (pb->x - pm->x)/(pb->y - pm->y);

        dcl_r = (cb->r - cm->r)/(pm->y - pt->y);
        dcl_g = (cb->g - cm->g)/(pm->y - pt->y);
        dcl_b = (cb->b - cm->b)/(pm->y - pt->y);
    for (int y=pm->y; y<=pb->y; y++)
    {
        float tmpxl = xl;
        float tmpxr = xr;
        float tmpcrl = rl;
        float tmpcrr = rr;
        float tmpcgl = gl;
        float tmpcgr = gr;
        float tmpcbl = bl;
        float tmpcbr = br;
        
        if (xl > xr)
        {
            tmpxl = xr;
            tmpxr = xl;
            tmpcrl = rr;
            tmpcrr = rl;
            tmpcgl = gr;
            tmpcgr = gl;
            tmpcbl = br;
            tmpcbr = bl;
        }
        for (int x=tmpxl; x<=tmpxr; x++)
        {
            POINT4D p = {static_cast<float>(x), static_cast<float>(y)};
            float dx = (x-tmpxl)/(tmpxr-tmpxl);
            RGBAV1 c;
            int r = tmpcrl + (tmpcrr - tmpcrl)*dx;
            int g = tmpcgl + (tmpcgr - tmpcgl)*dx;
            int b = tmpcbl + (tmpcbr - tmpcbl)*dx;
            c.r = MIN(MAX(0, r), 255);
            c.g = MIN(MAX(0, g), 255);
            c.b = MIN(MAX(0, b), 255);
            drawPoint(&p, &c);
        }
        
        rl += dcl_r;
        rr += dcr_r;
        gl += dcl_g;
        gr += dcr_g;
        bl += dcl_b;
        br += dcr_b;
        xl += dl;
        xr += dr;
    }
}

void drawTrangleBottomPlane(POINT4D_PTR pt, POINT4D_PTR pl, POINT4D_PTR pr)
{
    float dl = (pl->x - pt->x)/(pl->y - pt->y);
    float dr = (pr->x - pt->x)/(pr->y - pt->y);
    float xl=pt->x, xr=pt->x;
    for (int y=pt->y; y<=(int)pl->y; y++)
    {
        POINT4D p0 = {static_cast<float>(xl), static_cast<float>(y)};
        POINT4D p1 = {static_cast<float>(xr), static_cast<float>(y)};
        drawLine(&p0, &p1);
        xl += dl;
        xr += dr;
    }
}

void drawTrangleGOURAUDBottomPlane(POINT4D_PTR pt, POINT4D_PTR pl, POINT4D_PTR pr,RGBAV1_PTR c0, RGBAV1_PTR c1, RGBAV1_PTR c2)
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
    for (int y=pl->y; y<(int)pb->y; y++)
    {
        POINT4D p0 = {static_cast<float>(xl), static_cast<float>(y)};
        POINT4D p1 = {static_cast<float>(xr), static_cast<float>(y)};
        drawLine(&p0, &p1);
        xl += dl;
        xr += dr;
    }
}
void drawTrangleGOURAUDTopPlane(POINT4D_PTR pb, POINT4D_PTR pl, POINT4D_PTR pr,RGBAV1_PTR c0, RGBAV1_PTR c1, RGBAV1_PTR c2)
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
//        drawTranglePlane(pt, pm, pb);
        int xline = (pm->y - pt->y)*(pb->x - pt->x)/(pb->y - pt->y);
        xline += pt->x;//get x of mid point

        POINT4D pTmp = {static_cast<float>(xline), pm->y,1,1};
        drawTrangleBottomPlane(pt, pm, &pTmp);
        drawTrangleTopPlane(pb, pm, &pTmp);
    }
}

void drawTrangleGOURAUD(POINT4D_PTR p0, POINT4D_PTR p1, POINT4D_PTR p2,RGBAV1_PTR c0, RGBAV1_PTR c1, RGBAV1_PTR c2)
{
    if (p0->y == p1->y)
    {
        if (p2->y >= p0->y)//top plane
        {
            drawTrangleGOURAUDTopPlane(p2, p0, p1, c2, c0, c1);
        }
        else//
        {
            drawTrangleGOURAUDBottomPlane(p2, p0, p1, c2, c0, c1);
        }
    }
    else if (p0->y == p2->y)
    {
        if (p1->y >= p0->y)//
        {
            drawTrangleGOURAUDTopPlane(p1, p0, p2, c1, c0, c2);
        }
        else//
        {
            drawTrangleGOURAUDBottomPlane(p1, p0, p2, c1, c0, c2);
        }
    }
    else if (p2->y == p1->y)
    {
        if (p0->y >= p2->y)//
        {
            drawTrangleGOURAUDTopPlane(p0, p2, p1, c0, c2, c1);
        }
        else//
        {
            drawTrangleGOURAUDBottomPlane(p0, p2, p1, c0, c2, c1);
        }
    }
    else
    {
        POINT4D_PTR pt, pm, pb;
        RGBAV1_PTR ct, cm, cb;
        if(p0->y < p1->y && p1->y < p2->y)
        {
            pt = p0;
            pm = p1;
            pb = p2;
            ct = c0;
            cm = c1;
            cb = c2;
        }
        else if(p0->y < p2->y && p2->y < p1->y)
        {
            pt = p0;
            pm = p2;
            pb = p1;
            ct = c0;
            cm = c2;
            cb = c1;
        }
        else if(p1->y < p0->y && p0->y < p2->y)
        {
            pt = p1;
            pm = p0;
            pb = p2;
            ct = c1;
            cm = c0;
            cb = c2;
        }
        else if(p1->y < p2->y && p2->y < p0->y)
        {
            pt = p1;
            pm = p2;
            pb = p0;
            ct = c1;
            cm = c2;
            cb = c0;
        }
        else if(p2->y < p0->y && p0->y < p1->y)
        {
            pt = p2;
            pm = p0;
            pb = p1;
            ct = c2;
            cm = c0;
            cb = c1;
        }
        else if(p2->y < p1->y && p1->y < p0->y)
        {
            pt = p2;
            pm = p1;
            pb = p0;
            ct = c2;
            cm = c1;
            cb = c0;
        }
        
        drawTranglePlaneGOURAUD(pt, pm, pb, ct, cm, cb);
    }
}


void drawPoly2(RENDERLIST4DV2_PTR rend_list)
{
    for(int poly=0; poly<rend_list->num_polys; poly++)
    {
        POLYF4DV2_PTR curr_poly = rend_list->poly_ptrs[poly];
        if(!(curr_poly->state & POLY4DV2_STATE_ACTIVE) ||
           (curr_poly->state & POLY4DV2_STATE_CLIPPED) ||
           (curr_poly->state & POLY4DV2_STATE_BACKFACE))
        {
            continue;
        }
        if (curr_poly->attr & POLY4DV2_ATTR_SHADE_MODE_GOURAUD)
        {
            RGBAV1 c0, c1, c2;
            c0.rgba = curr_poly->lit_color[0];
            c1.rgba = curr_poly->lit_color[1];
            c2.rgba = curr_poly->lit_color[2];

            drawTrangleGOURAUD(&curr_poly->tvlist[0].v,
                               &curr_poly->tvlist[1].v,
                               &curr_poly->tvlist[2].v, &c0, &c1, &c2);
        }
        else if(curr_poly->attr & POLY4DV2_ATTR_SHADE_MODE_FLAT)
        {
            unsigned int r, g, b;
            int color = curr_poly->lit_color[0];
            RGB888FROM24BIT(color, &r, &g, &b);
            glColor3f (r/255.0, g/255.0, b/255.0);//…Ë÷√µ±«∞ª≠± —’…´
            drawTrangle(&curr_poly->tvlist[0].v,
                        &curr_poly->tvlist[1].v,
                        &curr_poly->tvlist[2].v);

        }
        RGBAV1 white, gray, black, red, green, blue;
        
        white.rgba = RGB32BIT(0,255,255,255);
        gray.rgba  = RGB32BIT(0,200,200,200);
        black.rgba = RGB32BIT(0,0,0,0);
        red.rgba   = RGB32BIT(0,255,0,0);
        green.rgba = RGB32BIT(0,0,255,0);
        blue.rgba  = RGB32BIT(0,0,0,255);


        
        //        printf("\n(v1 : %.1f, %.1f;  v2 : %.1f, %.1f;  v3 : %.1f, %.1f)",
        //               curr_poly->tvlist[0].v.x, curr_poly->tvlist[0].v.y,
        //               curr_poly->tvlist[1].v.x, curr_poly->tvlist[1].v.y,
        //               curr_poly->tvlist[2].v.x, curr_poly->tvlist[2].v.y);

    }
    
    if (isDrawWireframe)
    {
        for(int poly=0; poly<rend_list->num_polys; poly++)
        {
            POLYF4DV2_PTR curr_poly = rend_list->poly_ptrs[poly];
            if(!(curr_poly->state & POLY4DV2_STATE_ACTIVE) ||
               (curr_poly->state & POLY4DV2_STATE_CLIPPED) ||
               (curr_poly->state & POLY4DV2_STATE_BACKFACE))
            {
                continue;
            }
            
            glColor3f (1.0, 1.0, 0.0);//…Ë÷√µ±«∞ª≠± —’…´
            drawLine(&curr_poly->tvlist[0].v, &curr_poly->tvlist[1].v);
            drawLine(&curr_poly->tvlist[2].v, &curr_poly->tvlist[1].v);
            drawLine(&curr_poly->tvlist[2].v, &curr_poly->tvlist[0].v);
            
        }
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

//        printf("(%d, %d, %d)\n", r, g, b);
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
        float cc = (i+1.0)/btnCnt;
        glColor3f (cc, cc, cc);
        float orign = -1;
        float x = i*tmpBtnS;
        glVertex2f( orign+x, orign );//
        glVertex2f( orign+x, orign+tmpBtnS );
        glVertex2f( orign+x+tmpBtnS, orign+tmpBtnS );
        glVertex2f( orign+x+tmpBtnS, orign );
        glEnd ();
    }

}

void loadLights()
{
    Reset_Lights_LIGHTV1();

    RGBAV1 white, gray, black, red, green, blue;

    white.rgba = RGB32BIT(0,255,255,255);
    gray.rgba  = RGB32BIT(0,200,200,200);
    black.rgba = RGB32BIT(0,0,0,0);
    red.rgba   = RGB32BIT(0,255,0,0);
    green.rgba = RGB32BIT(0,0,255,0);
    blue.rgba  = RGB32BIT(0,0,0,255);


    // ambient light
    Init_Light_LIGHTV1(AMBIENT_LIGHT_INDEX,
                       LIGHTV1_STATE_ON,      // turn the light on
                       LIGHTV1_ATTR_AMBIENT,  // ambient light type
                       white, black, black,    // color for ambient term only
                       NULL, NULL,            // no need for pos or dir
                       0,0,0,                 // no need for attenuation
                       0,0,0);                // spotlight info NA


    VECTOR4D dlight_dir = {-1,0,-1,0};

    // directional light
    Init_Light_LIGHTV1(INFINITE_LIGHT_INDEX,
                       LIGHTV1_STATE_OFF,      // turn the light on
                       LIGHTV1_ATTR_INFINITE, // infinite light type
                       black, white, black,    // color for diffuse term only
                       NULL, &dlight_dir,     // need direction only
                       0,0,0,                 // no need for attenuation
                       0,0,0);                // spotlight info NA


    VECTOR4D plight_pos = {0,200,0,0};

    // point light
    Init_Light_LIGHTV1(POINT_LIGHT_INDEX,
                       LIGHTV1_STATE_OFF,      // turn the light on
                       LIGHTV1_ATTR_POINT,    // pointlight type
                       black, green, black,   // color for diffuse term only
                       &plight_pos, NULL,     // need pos only
                       0,.001,0,              // linear attenuation only
                       0,0,1);                // spotlight info NA

    VECTOR4D slight_pos = {0,200,0,0};
    VECTOR4D slight_dir = {-1,0,-1,0};

    // spot light
    Init_Light_LIGHTV1(SPOT_LIGHT_INDEX,
                       LIGHTV1_STATE_OFF,         // turn the light on
                       LIGHTV1_ATTR_SPOTLIGHT2,  // spot light type 2
                       black, red, black,      // color for diffuse term only
                       &slight_pos, &slight_dir, // need pos only
                       0,.001,0,                 // linear attenuation only
                       0,0,1);
}


void myDisplay ()
{
    Reset_RENDERLIST4DV2(&gRend_list);
    for (int i=0; i<100; i++)
    {
        OBJECT4DV2_PTR obj = &gAllObjects[i];
        
        float ro = 0.000001*clock();
        Rotate_XYZ_OBJECT4DV2(obj, 0, ro, ro);
        if (!(obj->state & OBJECT4DV2_STATE_ACTIVE))
        {
            break;
        }
        Model_To_World_OBJECT4DV2(obj);
        Insert_OBJECT4DV2_RENDERLIST4DV2(&gRend_list, obj, 0);
    }
    Remove_Backfaces_RENDERLIST4DV2(&gRend_list, &gCam);
    Light_RENDERLIST4DV2_World16(&gRend_list, &gCam, GetLightList(), 4);
    World_To_Camera_RENDERLIST4DV2(&gRend_list, &gCam);
    Sort_RENDERLIST4DV2(&gRend_list, SORT_POLYLIST_AVGZ);
    Camera_To_Perspective_RENDERLIST4DV2(&gRend_list, &gCam);
    Perspective_To_Screen_RENDERLIST4DV2(&gRend_list, &gCam);


    glClear (GL_COLOR_BUFFER_BIT);
    drawPoly2(&gRend_list);
    drawDebugBtn();
    glFlush();

//    print zOrder
//    for (int i=0; i<OBJECT4DV1_MAX_POLYS; i++)
//    {
//        POLYF4DV1_PTR p = gRend_list.poly_ptrs[i];
//        if (!p)
//        {
//            break;
//        }
//        float z = p->tvlist[0].z+p->tvlist[1].z+p->tvlist[2].z;
//        printf("idx : %d  z:%.2f \n", p->idx, z);
//    }
}

void btnClick(int idx, LIGHTV1_PTR lights)
{
    LIGHTV1_PTR li = &lights[idx];
    li->state = li->state == LIGHTV1_STATE_ON ? LIGHTV1_STATE_OFF : LIGHTV1_STATE_ON;
//    if (idx==0)
//    {
//        LIGHTV1_PTR li = &lights[AMBIENT_LIGHT_INDEX];
//        int c = li->c_ambient.g;
//        RGBAV1 rgb;
//        c+=10;
//        rgb.rgba = RGB32BIT(0, c, c, c);
//        li->c_ambient = rgb;
//    }
    
    myDisplay();
}

void myMouse(int button,int state,int x,int y)
{
    if(state==GLUT_DOWN)
    {

        if (sSize - y <= btnSize)
        {
            int idx = x/btnSize;
            btnClick(idx, GetLightList());
        }
    }
}



void onTimer(int value)
{
    glutPostRedisplay();
    glutTimerFunc(refreshFrequency, onTimer, 1);
}

int main(int argc, char *argv[])
{
    POINT4D cam_pos = {0,30,0,1};
    VECTOR4D cam_dir = {0,0,0,1};

    Init_CAM4DV1(&gCam, &cam_pos, &cam_dir, 50,sSize,90, sSize,sSize);
    Build_CAM4DV1_Matrix_Euler(&gCam, CAM_ROT_SEQ_ZYX);

    int towerCnt = 0;
    for (int tower=0; tower<towerCnt; tower++)
    {
        OBJECT4DV2 obj;
        float scale = (50 + rand()%50)*0.01;
        scale = 0.5;
        int xt = 300;
        float x = xt*0.5 - rand()%xt;
        float z = 50 + rand()%100;
        VECTOR4D vscale = {scale,scale,scale,scale}, vpos = {x,0,z,1}, vrot = {0,0,0,1};
#ifdef __APPLE__
        Load_OBJECT4DV2_PLG(&obj, "/MyFiles/Work/GitProject/Render/Render/tower1.plg", &vscale, &vpos, &vrot);
#else
        Load_OBJECT4DV2_PLG(&obj,"C:\\Users\\Administrator\\Desktop\\git\\Render\\Render\\tower1.plg", &vscale, &vpos, &vrot);
#endif

        gAllObjects[tower] = obj;
    }

    for (int cube=0; cube < 1; cube++)
    {
        OBJECT4DV2 obj;
        float scale = (50 + rand()%50)*0.01;
        float r = rand()%100;
        int xt = 400;
        float x = xt*0.5 - rand()%xt;
        float z = 30 + rand()%100;
        float y = 30;
        x = 0;
        z = 10;
        VECTOR4D vscale = {scale,scale,scale,scale}, vpos = {x,y,z,1}, vrot = {r,r,r,1};
#ifdef __APPLE__
        Load_OBJECT4DV2_PLG(&obj,"/MyFiles/Work/GitProject/Render/Render/cube1.plg", &vscale, &vpos, &vrot);
#else
        Load_OBJECT4DV2_PLG(&obj,"C:\\Users\\Administrator\\Desktop\\git\\Render\\Render\\cube1.plg", &vscale, &vpos, &vrot);
#endif
        gAllObjects[cube+towerCnt] = obj;
        

    }

    loadLights();

    glutInit(&argc, argv);//≥ı ºªØ,±ÿ–Î‘⁄µ˜”√∆‰À˚GLUT∫Ø ˝«∞µ˜”√“ªœ¬
    glutInitDisplayMode (GLUT_RGBA | GLUT_SINGLE);//…Ë∂®ƒ£ Ω,RGBA…´≤ ,∫Õµ•ª∫≥Â«¯
    glutInitWindowPosition (100, 100);//…Ë÷√¥∞ø⁄Œª÷√,»Áπ˚…Ë-1,-1æÕ «ƒ¨»œŒª÷√
    glutInitWindowSize (sSize, sSize);//…Ë÷√¥∞ø⁄¥Û–°
    glutCreateWindow ("hello word!");//¥¥Ω®√˚≥∆Œ™"hello word!"µƒ¥∞ø⁄,¥∞ø⁄¥¥Ω®∫Û≤ªª·¡¢º¥œ‘ æµΩ∆¡ƒª…œ,“™µ˜”√∫Û√ÊµƒglutMainLoop()≤≈ª·œ‘ æ
    glutDisplayFunc(myDisplay);//µ˜”√ªÊ÷∆∫Ø ˝ πÀ¸œ‘ æ‘⁄∏’¥¥Ω®µƒ¥∞ø⁄…œ
   glutTimerFunc(refreshFrequency, onTimer, 1);

    glutMouseFunc(myMouse);
    glutMainLoop();//œ˚œ¢—≠ª∑,¥∞ø⁄πÿ±’≤≈ª·∑µªÿ



    return 0;
}


