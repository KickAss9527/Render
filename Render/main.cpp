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
#include <GL/gl.h>
#include <GL/glext.h>
#endif

#include <stdlib.h>
#include "lxMath.h"
#include "engine.h"
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
static int slices = 16;
static int stacks = 16;
int sSize = 800;
CAM4DV1 gCam;
RENDERLIST4DV2 gRend_list;
OBJECT4DV2 gAllObjects[100];
bool isDrawWireframe = 10;
int refreshFrequency = 30;
BITMAP_IMAGE myTex;
#define AMBIENT_LIGHT_INDEX   0 // ambient light index
#define INFINITE_LIGHT_INDEX  1 // infinite light index
#define POINT_LIGHT_INDEX     2 // point light index
#define SPOT_LIGHT_INDEX      3 // spot light index

void drawPoint(POINT4D_PTR p, RGBAV1_PTR color)
{
    float w = sSize*0.5;
    POINT2D np = {(p->x-w)/w, (w-p->y)/w};
    glBegin (GL_POINTS);

    glColor3f (color->r/255.0, color->g/255.0, color->b/255.0);

    glVertex2f(np.x, np.y);
    glEnd ();
}

RGBAV1 getTexture(BITMAP_IMAGE_PTR tex, POINT2D_PTR pos)
{
    int delta = 3*((tex->height - pos->y)*tex->width + pos->x);
    RGBAV1 c;
    c.b = tex->buffer[delta];
    c.g = tex->buffer[delta+1];
    c.r = tex->buffer[delta+2];
    return c;
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
}/*
double x, y, xleft, xright; // 插值x和y，左右线段x
double oneoverz_left, oneoverz_right; // 左右线段1/z
double oneoverz_top, oneoverz_bottom; // 上下顶点1/z
double oneoverz, oneoverz_step;   // 插值1/z以及扫描线步长
double soverz_top, soverz_bottom; // 上下顶点s/z

double soverz_left, soverz_right; // 左右线段s/z

double soverz, soverz_step; // 插值s/z以及扫描线步长

double s, t; // 要求的原始s和t
for(y = y0; y < y1; ++y)
{
    xleft = 用y和左边的直线方程来求出左边的x
    xright = 用y和右边的直线方程来求出右边的x

    oneoverz_top = 1.0 / z0;
    oneoverz_bottom = 1.0 / z1;
    oneoverz_left = (y – y0) * (oneoverz_bottom – oneoverz_top) / (y1 – y0) + oneoverz_top;
    oneoverz_bottom = 1.0 / z2;
    oneoverz_right = (y – y0) * (oneoverz_bottom – oneoverz_top) / (y2 – y0) + oneoverz_top;
    oneoverz_step = (oneoverz_right – oneoverz_left) / (xright – xleft);

    soverz_top = s0 / z0;
    soverz_bottom = s1 / z1;
    soverz_left = (y – y0) * (soverz_bottom – soverz_top) / (y1 – y0) + soverz_top;
    soverz_bottom = s2 / z2;
    soverz_right = (y – y0) * (soverz_bottom – soverz_top) / (y2 – y0) + soverz_top;
    soverz_step = (soverz_right – soverz_left) / (xright – xleft);
        for(x = xleft, oneoverz = oneoverz_left, soverz = soverz_left,
            x < xright; ++x, oneoverz += oneoverz_step,
            soverz += soverz_step)
    {
        s = soverz / oneoverz;

        帧缓冲像素[x, y] = 纹理[s, t];
    }
}*/
void drawTranglePlaneTexture(VERTEX4DTV1_PTR pt, VERTEX4DTV1_PTR pm, VERTEX4DTV1_PTR pb)
{
    float ymt = (pm->y - pt->y);
    float ybt = (pb->y - pt->y);
    float dmx = (pm->x - pt->x)/ymt;
    float dbx = (pb->x - pt->x)/ybt;
    float dmz = (1/pm->z - 1/pt->z)/ymt;
    float dbz = (1/pb->z - 1/pt->z)/ybt;
    float xb, xm, zb, zm;
    xb = xm = pt->x;
    zb = zm = 1/pt->z;

    float um,vm,ub,vb;
    float dm_u,dm_v,db_u,db_v;

    dm_u = (pm->u0/pm->z - pt->u0/pt->z)/ymt;
    dm_v = (pm->v0/pm->z - pt->v0/pt->z)/ymt;

    db_u = (pb->u0/pb->z - pt->u0/pt->z)/ybt;
    db_v = (pb->v0/pb->z - pt->v0/pt->z)/ybt;

    um = ub = pt->u0/pt->z;
    vm = vb = pt->v0/pt->z;

    for(int y=pt->y; y<=pb->y; y++)
    {
        bool seqMB = xm < xb;
        float u,v,z;

        if(seqMB)
        {
            z = zm;
            u = um;
            v = vm;
        }
        else
        {
            z = zb;
            u = ub;
            v = vb;
        }
        float startX = MIN(xb,xm);
        float endX = MAX(xb,xm);

        for(int x=startX; x<=endX; x++)
        {
            POINT4D p0 = {static_cast<float>(x), static_cast<float>(y)};
            float delX;
            if(fabs(endX-startX)<0.00001)
            {
                delX=1;
            }
            else
            {
                delX = (x-(int)startX)/(endX-startX);
            }

            POINT2D uv;
            delX *= seqMB?1:-1;
            float tmpZ = z + delX*(zb - zm);
//            tmpZ = 1;
//            zb = zm =1;
            float tmpU = u + delX*(ub - um);
            float tmpV = v + delX*(vb - vm);
            tmpU /= tmpZ;
            tmpV /= tmpZ;

            uv.x = MIN(MAX(0, tmpU), 1);
            uv.y = MIN(MAX(0, tmpV), 1);

            uv.x *= myTex.width;
            uv.y *= myTex.height;

            uv.x = floor(uv.x);
            uv.y = floor(uv.y);
            RGBAV1 color = getTexture(&myTex, &uv);
            drawPoint(&p0, &color);
        }

        xb += dbx;
        xm += dmx;
        zb += dbz;
        zm += dmz;

        um += dm_u;
        vm += dm_v;

        ub += db_u;
        vb += db_v;

        if(y == (int)pm->y)
        {
            xm = pm->x;
            zm = 1/pm->z;
            um = pm->u0/pm->z;
            vm = pm->v0/pm->z;
            float ybm = pb->y - pm->y;
            dmx = (pb->x - pm->x)/ybm;
            dmz = (1/pb->z - 1/pm->z)/ybm;
            dm_u = (pb->u0/pb->z - pm->u0/pm->z)/ybm;
            dm_v = (pb->v0/pb->z - pm->v0/pm->z)/ybm;
        }
    }
}

void drawTranglePlaneGOURAUD(POINT4D_PTR pt, POINT4D_PTR pm, POINT4D_PTR pb, RGBAV1_PTR ct, RGBAV1_PTR cm, RGBAV1_PTR cb)
{
    float ymt = (pm->y - pt->y);
    float ybt = (pb->y - pt->y);
    float dm = (pm->x - pt->x)/ymt;
    float db = (pb->x - pt->x)/ybt;
    float xb, xm;
    xb = xm = pt->x;

    float cm_r,cm_g,cm_b,cb_r,cb_g,cb_b;
    float dm_r,dm_g,dm_b,db_r,db_g,db_b;

    dm_r = (cm->r - ct->r)/ymt;
    dm_g = (cm->g - ct->g)/ymt;
    dm_b = (cm->b - ct->b)/ymt;

    db_r = (cb->r - ct->r)/ybt;
    db_g = (cb->g - ct->g)/ybt;
    db_b = (cb->b - ct->b)/ybt;

    cm_r = cb_r = ct->r;
    cm_g = cb_g = ct->g;
    cm_b = cb_b = ct->b;

    for(int y=pt->y; y<=pb->y; y++)
    {
        bool seqMB = xm < xb;
        float r,g,b;

        if(seqMB)
        {
            r = cm_r;
            g = cm_g;
            b = cm_b;
        }
        else
        {
            r = cb_r;
            g = cb_g;
            b = cb_b;
        }
        float startX = MIN(xb,xm);
        float endX = MAX(xb,xm);
        for(int x=startX; x<=endX; x++)
        {
            POINT4D p0 = {static_cast<float>(x), static_cast<float>(y)};
            float delX;
            if(fabs(endX-startX)<0.00001)
            {
                delX=1;
            }
            else
            {
                delX = (x-(int)startX)/(endX-startX);
            }
            RGBAV1 co;
            delX *= seqMB?1:-1;
            float tmpR = r + delX*(cb_r - cm_r);
            float tmpG = g + delX*(cb_g - cm_g);
            float tmpB = b + delX*(cb_b - cm_b);

            co.r = MIN(MAX(0, tmpR), 255);
            co.g = MIN(MAX(0, tmpG), 255);
            co.b = MIN(MAX(0, tmpB), 255);
            drawPoint(&p0, &co);
        }

        xb += db;
        xm += dm;

        cm_r += dm_r;
        cm_g += dm_g;
        cm_b += dm_b;

        cb_r += db_r;
        cb_g += db_g;
        cb_b += db_b;

        if(y == (int)pm->y)
        {
            xm = pm->x;
            cm_r = cm->r;
            cm_g = cm->g;
            cm_b = cm->b;
            float ybm = pb->y - pm->y;
            dm = (pb->x - pm->x)/ybm;
            dm_r = (cb->r - cm->r)/ybm;
            dm_g = (cb->g - cm->g)/ybm;
            dm_b = (cb->b - cm->b)/ybm;
        }
    }
}

void drawTrangleBottomPlane(POINT4D_PTR pt, POINT4D_PTR pl, POINT4D_PTR pr)
{
    float dl = (pl->x - pt->x)/(pl->y - pt->y);
    float dr = (pr->x - pt->x)/(pr->y - pt->y);
    float xl=pt->x, xr=pt->x;
    for (int y=pt->y; y<=(int)pl->y; y++)
    {
        if ((xl < pl->x && xl < pr->x && xl < pt->x) ||
            (xr < pl->x && xr < pr->x && xr < pt->x) ||
            (xl > pl->x && xl > pr->x && xl > pt->x) ||
            (xr > pl->x && xr > pr->x && xr > pt->x)) {
            continue;
        }
        POINT4D p0 = {static_cast<float>(xl), static_cast<float>(y)};
        POINT4D p1 = {static_cast<float>(xr), static_cast<float>(y)};
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
        if ((xl < pl->x && xl < pr->x && xl < pb->x) ||
            (xr < pl->x && xr < pr->x && xr < pb->x) ||
            (xl > pl->x && xl > pr->x && xl > pb->x) ||
            (xr > pl->x && xr > pr->x && xr > pb->x)) {
            continue;
        }
        POINT4D p0 = {static_cast<float>(xl), static_cast<float>(y)};
        POINT4D p1 = {static_cast<float>(xr), static_cast<float>(y)};
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
void drawTrangleTexture(VERTEX4DTV1_PTR p0, VERTEX4DTV1_PTR p1, VERTEX4DTV1_PTR p2)
{

    VERTEX4DTV1_PTR pt, pm, pb;

    if(p0->y <= p1->y && p1->y <= p2->y)
    {
        pt = p0;
        pm = p1;
        pb = p2;
    }
    else if(p0->y <= p2->y && p2->y <= p1->y)
    {
        pt = p0;
        pm = p2;
        pb = p1;
    }
    else if(p1->y <= p0->y && p0->y <= p2->y)
    {
        pt = p1;
        pm = p0;
        pb = p2;
    }
    else if(p1->y <= p2->y && p2->y <= p0->y)
    {
        pt = p1;
        pm = p2;
        pb = p0;
    }
    else if(p2->y <= p0->y && p0->y <= p1->y)
    {
        pt = p2;
        pm = p0;
        pb = p1;
    }
    else if(p2->y <= p1->y && p1->y <= p0->y)
    {
        pt = p2;
        pm = p1;
        pb = p0;
    }
//    pt->u0 = 0;
//    pt->v0 = 0;
//    pm->u0 = 0;
//    pm->v0 = 0;
//    pt->u0 = 0;
//    pt->v0 = 0;
    drawTranglePlaneTexture(pt, pm, pb);
}

void drawTrangleGOURAUD(POINT4D_PTR p0, POINT4D_PTR p1, POINT4D_PTR p2,RGBAV1_PTR c0, RGBAV1_PTR c1, RGBAV1_PTR c2)
{
//    if (p0->y == p1->y)
//    {
//        if (p2->y >= p0->y)//top plane
//        {
//            if (p2->x > p0->x)
//            {
//                drawTrangleGOURAUDTopPlane(NULL, p2, p0, p1, c2, c0, c1);
//            }
//            else
//            {
//                drawTrangleGOURAUDTopPlane(NULL, p0, p2, p1, c0, c2, c1);
//            }
//        }
//        else//
//        {
//            drawTrangleGOURAUDBottomPlane(p2, p0, p1, NULL, c2, c0, c1);
//        }
//    }
//    else if (p0->y == p2->y)
//    {
//        if (p1->y >= p0->y)//
//        {
//            drawTrangleGOURAUDTopPlane(NULL, p1, p0, p2, c1, c0, c2);
//        }
//        else//
//        {
//            drawTrangleGOURAUDBottomPlane(p1, p0, p2,NULL,  c1, c0, c2);
//        }
//    }
//    else if (p2->y == p1->y)
//    {
//        if (p0->y >= p2->y)//
//        {
//            drawTrangleGOURAUDTopPlane(NULL, p0, p2, p1, c0, c2, c1);
//        }
//        else//
//        {
//            drawTrangleGOURAUDBottomPlane(p0, p2, p1, NULL, c0, c2, c1);
//        }
//    }
    if (0)
    {

    }
    else
    {
        POINT4D_PTR pt, pm, pb;
        RGBAV1_PTR ct, cm, cb;
        if(p0->y <= p1->y && p1->y <= p2->y)
        {
            pt = p0;
            pm = p1;
            pb = p2;
            ct = c0;
            cm = c1;
            cb = c2;
        }
        else if(p0->y <= p2->y && p2->y <= p1->y)
        {
            pt = p0;
            pm = p2;
            pb = p1;
            ct = c0;
            cm = c2;
            cb = c1;
        }
        else if(p1->y <= p0->y && p0->y <= p2->y)
        {
            pt = p1;
            pm = p0;
            pb = p2;
            ct = c1;
            cm = c0;
            cb = c2;
        }
        else if(p1->y <= p2->y && p2->y <= p0->y)
        {
            pt = p1;
            pm = p2;
            pb = p0;
            ct = c1;
            cm = c2;
            cb = c0;
        }
        else if(p2->y <= p0->y && p0->y <= p1->y)
        {
            pt = p2;
            pm = p0;
            pb = p1;
            ct = c2;
            cm = c0;
            cb = c1;
        }
        else if(p2->y <= p1->y && p1->y <= p0->y)
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
    RGBAV1 white, gray, black, red, green, blue;

    white.rgba = RGB32BIT(0,255,255,255);
    gray.rgba  = RGB32BIT(0,200,200,200);
    black.rgba = RGB32BIT(0,0,0,0);
    red.rgba   = RGB32BIT(0,255,0,0);
    green.rgba = RGB32BIT(0,0,255,0);
    blue.rgba  = RGB32BIT(0,0,0,255);

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
            //c0 = blue;
            //c1 = red;
            //c2 = green;

//            printf("\n %.1f, %.1f; %.1f, %.1f; %.1f, %.1f",
//                   curr_poly->tvlist[0].t.x,curr_poly->tvlist[0].t.y,
//                   curr_poly->tvlist[1].t.x,curr_poly->tvlist[1].t.y,
//                   curr_poly->tvlist[2].t.x,curr_poly->tvlist[2].t.y);
            drawTrangleTexture(&curr_poly->tvlist[0], &curr_poly->tvlist[1], &curr_poly->tvlist[2]);
//            drawTrangleGOURAUD(&curr_poly->tvlist[0].v,
//                               &curr_poly->tvlist[1].v,
//                               &curr_poly->tvlist[2].v, &c0, &c1, &c2);

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

        static float ro = 1;
        ro += 0.000001;
        Rotate_XYZ_OBJECT4DV2(obj, 0, ro, 0);
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



void display(void)
{
 //glClear(GL_COLOR_BUFFER_BIT);
 //绘制像素

//glDrawPixels(myTex.width,myTex.height,GL_BGR_EXT,GL_UNSIGNED_BYTE,myTex.buffer);

 //---------------------------------

 for(int y=0; y<myTex.height; y++)
 {
     for(int x=0; x<myTex.width; x++)
     {
         POINT4D np = {static_cast<float>(x),static_cast<float>(y), 1, 1};
         POINT2D t = {static_cast<float>(x),static_cast<float>(y)};
         RGBAV1 c = getTexture(&myTex, &t);
         drawPoint(&np, &c);
     }
 }
glFlush();
}

void loadTexture()
{
#ifdef __APPLE__

    FILE* pfile=fopen("/MyFiles/Work/GitProject/Render/Render/metal04.bmp","rb");
#else
    FILE* pfile=fopen("C:\\Users\\Administrator\\Documents\\GitHub\\Render\\Render\\metal04.bmp","rb");
#endif

     if(pfile == 0) exit(0);
     //读取图像大小


     fseek(pfile,0x0012,SEEK_SET);
     fread(&myTex.width,sizeof(myTex.width),1,pfile);
     fread(&myTex.height,sizeof(myTex.height),1,pfile);
     //计算像素数据长度
     int pixellength=myTex.width*3;
     while(pixellength%4 != 0)pixellength++;
     pixellength *= myTex.height;
     //读取像素数据
     myTex.buffer = (GLubyte*)malloc(pixellength);
     if(myTex.buffer == 0) exit(0);
     fseek(pfile,54,SEEK_SET);
     fread(myTex.buffer,pixellength,1,pfile);

     //关闭文件
     fclose(pfile);

}

int main(int argc, char *argv[])
{
    loadTexture();
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
        Load_OBJECT4DV2_PLG(&obj,"C:\\Users\\Administrator\\Documents\\GitHub\\Render\\Render\\tower1.plg", &vscale, &vpos, &vrot);
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
        float y = 24;
        x = 0;
        z = 15;
        VECTOR4D vscale = {scale,scale,scale,scale}, vpos = {x,y,z,1}, vrot = {r,r,r,1};
#ifdef __APPLE__
        Load_OBJECT4DV2_PLG(&obj,"/MyFiles/Work/GitProject/Render/Render/cube1.plg", &vscale, &vpos, &vrot);
#else
        Load_OBJECT4DV2_PLG(&obj,"C:\\Users\\Administrator\\Documents\\GitHub\\Render\\Render\\cube1.plg", &vscale, &vpos, &vrot);
#endif
        gAllObjects[cube+towerCnt] = obj;
        obj.texture = &myTex;

        obj.tlist[0].x = 0; obj.tlist[0].y = 1;
        obj.tlist[1].x = 0; obj.tlist[1].y = 0;
        obj.tlist[2].x = 1; obj.tlist[2].y = 0;
        obj.tlist[3].x = 1; obj.tlist[3].y = 1;
        obj.tlist[4].x = 0; obj.tlist[4].y = 1;
        obj.tlist[5].x = 1; obj.tlist[5].y = 0;

        obj.tlist[6].x = 1; obj.tlist[6].y = 1;
        obj.tlist[7].x = 0; obj.tlist[7].y = 1;
        obj.tlist[8].x = 1; obj.tlist[8].y = 0;
        obj.tlist[9].x = 0; obj.tlist[9].y = 1;
        obj.tlist[10].x = 0; obj.tlist[10].y = 0;
        obj.tlist[11].x = 1; obj.tlist[11].y = 0;

        obj.tlist[12].x = 0; obj.tlist[12].y = 1;
        obj.tlist[13].x = 1; obj.tlist[13].y = 1;
        obj.tlist[14].x = 1; obj.tlist[14].y = 0;
        obj.tlist[15].x = 0; obj.tlist[15].y = 0;
        obj.tlist[16].x = 0; obj.tlist[16].y = 1;
        obj.tlist[17].x = 1; obj.tlist[17].y = 0;

        obj.tlist[18].x = 1; obj.tlist[18].y = 0;
        obj.tlist[19].x = 1; obj.tlist[19].y = 1;
        obj.tlist[20].x = 0; obj.tlist[20].y = 0;
        obj.tlist[21].x = 1; obj.tlist[21].y = 1;
        obj.tlist[22].x = 0; obj.tlist[22].y = 1;
        obj.tlist[23].x = 0; obj.tlist[23].y = 0;

        obj.tlist[24].x = 1; obj.tlist[24].y = 1;
        obj.tlist[25].x = 0; obj.tlist[25].y = 1;
        obj.tlist[26].x = 1; obj.tlist[26].y = 0;
        obj.tlist[27].x = 0; obj.tlist[27].y = 1;
        obj.tlist[28].x = 0; obj.tlist[28].y = 0;
        obj.tlist[29].x = 1; obj.tlist[29].y = 0;

        obj.tlist[30].x = 1; obj.tlist[30].y = 1;
        obj.tlist[31].x = 0; obj.tlist[31].y = 1;
        obj.tlist[32].x = 0; obj.tlist[32].y = 0;
        obj.tlist[33].x = 1; obj.tlist[33].y = 0;
        obj.tlist[34].x = 1; obj.tlist[34].y = 1;
        obj.tlist[35].x = 0; obj.tlist[35].y = 0;

        //

        for(int i=0; i<12; i++)
        {
            obj.plist[i].text[0] = i*3+0;
            obj.plist[i].text[1] = i*3+1;
            obj.plist[i].text[2] = i*3+2;

        }
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


