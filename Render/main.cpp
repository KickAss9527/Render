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

#include "lxMath.h"
#include "engine.h"
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "enginePlus.h"
static int slices = 16;
static int stacks = 16;

VECTOR2D sizeScreen = {500, 500};
CAM4DV1 gCam;
RENDERLIST4DV2 gRend_list;
OBJECT4DV2 gAllObjects[100];
float *ZBuffer;
bool isDrawWireframe = 10;
int refreshFrequency = 30;
bool isRotate = 110;
float keyboardMovingOffset = 2;
int towerCnt = 0;
int cubeCnt = 0;
BITMAP_IMAGE myTex;

#define AMBIENT_LIGHT_INDEX   0 // ambient light index
#define INFINITE_LIGHT_INDEX  1 // infinite light index
#define POINT_LIGHT_INDEX     2 // point light index
#define SPOT_LIGHT_INDEX      3 // spot light index

const char *getFilePath(const char* fileName)
{
    string path;
#ifdef __APPLE__
    path = "/MyFiles/Work/GitProject/Render/Render/";
#else
    path = "C:\\Users\\Administrator\\Documents\\GitHub\\Render\\Render\\";
#endif
    
    path += fileName;
    return path.c_str();
}

const char *getModelPath_Cube(){return getFilePath("cube1.plg");}
const char *getModelPath_CubeTexture(){return getFilePath("cubeTex.plg");}
const char *getModelPath_Tower(){return getFilePath("tower1.plg");}

void resetZBuffer()
{
    memset((void*)ZBuffer, 0, sizeof(float)*sizeScreen.x*sizeScreen.y);
}

bool testZBuffer(float z, POINT4D_PTR scrPos)
{
    int idx = scrPos->x + scrPos->y*sizeScreen.x;
    if (z > ZBuffer[idx])
    {
        ZBuffer[idx] = z;
        return true;
    }
    
    return false;
}

void drawPoint(POINT4D_PTR p, RGBAV1_PTR color)
{
    float w = sizeScreen.x*0.5;
    POINT2D np = {(p->x-w)/w, (w-p->y)/w};
    glBegin (GL_POINTS);
    //if(color->r + color->g  + color->b < 1){printf("\n, p->x:%.2f", p->x);}
    glColor3f (color->r/255.0, color->g/255.0, color->b/255.0);

    glVertex2f(np.x, np.y);
    glEnd ();
}



void drawLine(POINT4D_PTR p0, POINT4D_PTR p1)
{
    int w = sizeScreen.x*0.5;
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

void drawTranglePlaneTexture(VERTEX4DTV1_PTR pt, VERTEX4DTV1_PTR pm, VERTEX4DTV1_PTR pb, BITMAP_IMAGE_PTR tex)
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
            if (!testZBuffer(tmpZ, &p0))
            {
                continue;
            }
//            tmpZ = 1;
//            zb = zm =1;
            float tmpU = u + delX*(ub - um);
            float tmpV = v + delX*(vb - vm);
            tmpU /= tmpZ;
            tmpV /= tmpZ;

            uv.x = MIN(MAX(0, tmpU), 1);
            uv.y = MIN(MAX(0, tmpV), 1);

            uv.x *= tex->width;
            uv.y *= tex->height;

            uv.x = floor(uv.x);
            uv.y = floor(uv.y);
            RGBAV1 color = getTextureColor(tex, &uv);
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
void drawTrangleTexture(VERTEX4DTV1_PTR p0, VERTEX4DTV1_PTR p1, VERTEX4DTV1_PTR p2, BITMAP_IMAGE_PTR tex)
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
    drawTranglePlaneTexture(pt, pm, pb, tex);
}

void drawTrangleGOURAUD(POINT4D_PTR p0, POINT4D_PTR p1, POINT4D_PTR p2,RGBAV1_PTR c0, RGBAV1_PTR c1, RGBAV1_PTR c2)
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

        if (curr_poly->attr & POLY4DV2_ATTR_SHADE_MODE_TEXTURE)
        {
            drawTrangleTexture(&curr_poly->tvlist[0], &curr_poly->tvlist[1], &curr_poly->tvlist[2], &myTex);
        }
        else if (curr_poly->attr & POLY4DV2_ATTR_SHADE_MODE_GOURAUD)
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

            glColor3f (0.8, 0.8, 0);//…Ë÷√µ±«∞ª≠± —’…´
            drawLine(&curr_poly->tvlist[0].v, &curr_poly->tvlist[1].v);
            drawLine(&curr_poly->tvlist[2].v, &curr_poly->tvlist[1].v);
            drawLine(&curr_poly->tvlist[2].v, &curr_poly->tvlist[0].v);
        }
    }
}

int btnCnt = 6;
float btnSize = 60.0;

void drawDebugBtn()
{

    float tmpBtnS = btnSize*2/sizeScreen.x;
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
    resetZBuffer();
    for (int i=0; i<1000; i++)
    {
        OBJECT4DV2_PTR obj = &gAllObjects[i];
        static float ro = 1;
        ro += 0.000001;
        if(isRotate)
        {
            Rotate_XYZ_OBJECT4DV2(obj, -30, 0, 0);
            isRotate = false;
        }
        if (!(obj->state & OBJECT4DV2_STATE_ACTIVE))
        {
            break;
        }
        UpdatePool(clock()*0.00001, obj);
        
        Model_To_World_OBJECT4DV2(obj);
        Insert_OBJECT4DV2_RENDERLIST4DV2(&gRend_list, obj, 0);
    }
    
    Remove_Backfaces_RENDERLIST4DV2(&gRend_list, &gCam);
    Light_RENDERLIST4DV2_World16(&gRend_list, &gCam, GetLightList(), 4);
    World_To_Camera_RENDERLIST4DV2(&gRend_list, &gCam);
//    Clip_Polys_RENDERLIST4DV2(&gRend_list, &gCam, CLIP_POLY_Z_PLANE);
    Sort_RENDERLIST4DV2(&gRend_list, SORT_POLYLIST_AVGZ);
    Camera_To_Perspective_RENDERLIST4DV2(&gRend_list, &gCam);
    Perspective_To_Screen_RENDERLIST4DV2(&gRend_list, &gCam);


    glClear (GL_COLOR_BUFFER_BIT);
    drawPoly2(&gRend_list);
    drawDebugBtn();
    glFlush();

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

        if (sizeScreen.x - y <= btnSize)
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
             RGBAV1 c = getTextureColor(&myTex, &t);
             drawPoint(&np, &c);
         }
     }
    glFlush();
}

void keyboardEvt(int key, int x, int y)
{

    float offset = keyboardMovingOffset;

    switch(key)
    {
        case GLUT_KEY_DOWN:
        {
            gCam.pos.y -= offset;
        }break;
        case GLUT_KEY_UP:
        {
            gCam.pos.y += offset;
        }break;
        case GLUT_KEY_LEFT:
        {
            gCam.pos.x -= offset;
        }break;
        case GLUT_KEY_RIGHT:
        {
            gCam.pos.x += offset;
        }break;
        case GLUT_KEY_HOME:
        {
            gCam.pos.z -= offset;
        }break;
        case GLUT_KEY_END:
        {
            gCam.pos.z += offset;
        }break;
        default:break;
    }

    Build_CAM4DV1_Matrix_Euler(&gCam, CAM_ROT_SEQ_ZYX);
}

int main(int argc, char *argv[])
{
    LoadMyBitmap(getFilePath("Wood.bmp"), &myTex);
    ZBuffer = (float*)malloc(sizeof(float)*sizeScreen.x*sizeScreen.y);
    
    POINT4D cam_pos = {0,30,0,1};
    VECTOR4D cam_dir = {0,0,0,1};
    Init_CAM4DV1(&gCam, &cam_pos, &cam_dir, 10,sizeScreen.x,90, sizeScreen.x,sizeScreen.y);
    Build_CAM4DV1_Matrix_Euler(&gCam, CAM_ROT_SEQ_ZYX);

    for (int tower=0; tower<towerCnt; tower++)
    {
        OBJECT4DV2 obj;
        float scale = (50 + rand()%50)*0.01;
        scale = 0.5;
        int xt = 300;
        float x = xt*0.5 - rand()%xt;
        float z = 50 + rand()%100;
        VECTOR4D vscale = {scale,scale,scale,scale}, vpos = {x,0,z,1}, vrot = {0,0,0,1};
        Load_OBJECT4DV2_PLG(&obj, getModelPath_Tower(), &vscale, &vpos, &vrot);
        gAllObjects[tower] = obj;
    }

    for (int cube=0; cube < cubeCnt; cube++)
    {
        OBJECT4DV2 obj;
        float scale = (50 + rand()%50)*0.01;
        float r = rand()%100;
        int xt = 400;
        float x = xt*0.5 - rand()%xt;
        float z = 20 + rand()%100;
        float y = 24;
        x = 0;
        z = 45;
        scale = 1;
        VECTOR4D vscale = {scale,scale,scale,scale}, vpos = {x,y,z,1}, vrot = {r,r,r,1};
        Load_OBJECT4DV2_PLG(&obj, getModelPath_CubeTexture(), &vscale, &vpos, &vrot);
        gAllObjects[cube+towerCnt] = obj;

    }
    
//  terrain
    OBJECT4DV2 terrain;
    GeneratePool(&terrain);
    gAllObjects[0] = terrain;

    loadLights();

    glutInit(&argc, argv);//≥ı ºªØ,±ÿ–Î‘⁄µ˜”√∆‰À˚GLUT∫Ø ˝«∞µ˜”√“ªœ¬
    glutInitDisplayMode (GLUT_RGBA | GLUT_SINGLE);//…Ë∂®ƒ£ Ω,RGBA…´≤ ,∫Õµ•ª∫≥Â«¯
    glutInitWindowPosition (100, 100);//…Ë÷√¥∞ø⁄Œª÷√,»Áπ˚…Ë-1,-1æÕ «ƒ¨»œŒª÷√
    glutInitWindowSize (sizeScreen.x, sizeScreen.y);//…Ë÷√¥∞ø⁄¥Û–°
    glutCreateWindow ("hello word!");//¥¥Ω®√˚≥∆Œ™"hello word!"µƒ¥∞ø⁄,¥∞ø⁄¥¥Ω®∫Û≤ªª·¡¢º¥œ‘ æµΩ∆¡ƒª…œ,“™µ˜”√∫Û√ÊµƒglutMainLoop()≤≈ª·œ‘ æ
    glutDisplayFunc(myDisplay);//µ˜”√ªÊ÷∆∫Ø ˝ πÀ¸œ‘ æ‘⁄∏’¥¥Ω®µƒ¥∞ø⁄…œ
   glutTimerFunc(refreshFrequency, onTimer, 1);
    glutSpecialFunc(keyboardEvt);
    glutMouseFunc(myMouse);
    glutMainLoop();//œ˚œ¢—≠ª∑,¥∞ø⁄πÿ±’≤≈ª·∑µªÿ
    return 0;
}


