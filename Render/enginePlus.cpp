//
//  enginePlus.c
//  Render
//
//  Created by XuMingQi on 2016/12/19.
//  Copyright © 2016年 XuMingQi. All rights reserved.
//

#include "enginePlus.h"
#include <iostream>
#include <string.h>
#include <stdio.h>

#include <stdlib.h>

using namespace std;
static int TerrainHeightMax = 30;
static float TerrainSizeScale = 4;
void GenerateTerrain(OBJECT4DV2_PTR obj)
{
    memset(obj, 0, sizeof(OBJECT4DV2));
    obj->state = OBJECT4DV2_STATE_ACTIVE | OBJECT4DV2_STATE_VISIBLE;
    obj->world_pos.x = 0;
    obj->world_pos.y = 0;
    obj->world_pos.z = 200;
    obj->world_pos.w = 1;
    obj->num_frames = 1;
    obj->curr_frame = 0;
    obj->attr = OBJECT4DV2_ATTR_SINGLE_FRAME;
    
    BITMAP_IMAGE texture;
    loadTexture("earthheightmap03.bmp", &texture);
    
    int deltaX = texture.width*0.5*TerrainSizeScale;
    int deltaZ = texture.height*0.5*TerrainSizeScale;
    
    float tmpX, tmpZ;
    tmpX = MIN(texture.width, 100);
    tmpZ = MIN(texture.height, 100);
//    tmpX = tmpZ = 2;
    
    
    obj->num_polys = tmpX*tmpZ*2;
    obj->num_vertices = (tmpX+1)*(tmpZ+1);
    Init_OBJECT4DV2(obj, obj->num_vertices, obj->num_polys, obj->num_frames);
    POINT3D *coordArr = new POINT3D[obj->num_vertices];
    for (int z=0; z<=tmpZ; z++)
    {
        for (int x=0; x<=tmpX; x++)
        {
            POINT2D p = {x*texture.width/tmpX, z*texture.height/tmpZ};
            RGBAV1 c = getTextureColor(&texture, &p);
            POINT3D coord;
            coord.x = p.x*TerrainSizeScale - deltaX;
            coord.z = p.y*TerrainSizeScale - deltaZ;
            coord.y = TerrainHeightMax*(c.r + c.g + c.b)/(255*3.0);
//            printf("\n (%.1f, %.1f) : %d, %d, %d", p.x, p.y, c.r,c.g,c.b);
            int coordIdx = x+z*((int)tmpX+1);
            coordArr[coordIdx] = coord;
            
        }
    }
    
    for (int vertex=0; vertex<obj->num_vertices; vertex++)
    {
        POINT3D p = coordArr[vertex];
        obj->vlist_local[vertex].x = p.x;
        obj->vlist_local[vertex].y = p.y;
        obj->vlist_local[vertex].z = p.z;
        obj->vlist_local[vertex].w = 1;
//        printf("\n %.2f, %.2f, %.2f", obj->vlist_local[vertex].x, obj->vlist_local[vertex].y, obj->vlist_local[vertex].z);
        SET_BIT(obj->vlist_local[vertex].attr, VERTEX4DTV1_ATTR_POINT);
    }
    
    Compute_OBJECT4DV2_Radius(obj);
    
    // v0----v1
    // | \    |
    // |   \  |
    // v3----v2
    for (int poly=0; poly<obj->num_polys*0.5; poly++)
    {
        int v0, v1, v2, v3;
        int y = poly/tmpX;
        v0 = poly + y;
        v1 = v0 + 1;
        v2 = v1 + tmpX + 1;
        v3 = v2 - 1;
        
        int polyIdx = poly*2;
        obj->plist[polyIdx].vert[0] = v0;
        obj->plist[polyIdx].vert[1] = v2;
        obj->plist[polyIdx].vert[2] = v1;
        
        obj->plist[polyIdx+1].vert[0] = v0;
        obj->plist[polyIdx+1].vert[1] = v3;
        obj->plist[polyIdx+1].vert[2] = v2;
        
        for (int i=polyIdx; i<=polyIdx+1; i++)
        {
            obj->plist[i].vlist = obj->vlist_local;
            SET_BIT(obj->plist[i].attr, POLY4DV2_ATTR_DISABLE_MATERIAL);
            obj->plist[i].state = POLY4DV2_STATE_ACTIVE;
            obj->plist[i].vlist = obj->vlist_local;
            obj->plist[i].tlist = obj->tlist;
        }
    }
    
    Compute_OBJECT4DV2_Poly_Normals(obj);
    Compute_OBJECT4DV2_Vertex_Normals(obj);
    
}
