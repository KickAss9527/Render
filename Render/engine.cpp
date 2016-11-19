#include <iostream>
#include <string.h>
#include <stdio.h>
#include "engine.h"
#include <stdlib.h>
#include <math.h>

void RESET_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list)
{
    rend_list->num_polys = 0;
}

float Compute_OBJECT4DV1_Radius(OBJECT4DV1_PTR obj)
{
    obj->avg_radius = obj->max_radius = 0;
    for(int vertex=0; vertex<obj->num_vertices; vertex++)
    {
        float dist_to_vertex = sqrtf(obj->vlist_local[vertex].x*obj->vlist_local[vertex].x+
                                    obj->vlist_local[vertex].y*obj->vlist_local[vertex].y+
                                    obj->vlist_local[vertex].z*obj->vlist_local[vertex].z);
        obj->avg_radius += dist_to_vertex;
        if(dist_to_vertex > obj->max_radius)
            obj->max_radius = dist_to_vertex;
    }
    obj->avg_radius/=obj->num_vertices;
    return obj->max_radius;
}

char *Get_Line_PLG(char *buffer, int maxlength, FILE *fp)
{
    int index = 0;
    int length = 0;

    while(1)
    {
        if(!fgets(buffer, maxlength, fp)) return(NULL);
        for(length = strlen(buffer), index=0; isspace(buffer[index]); index++);
        if(index >= length || buffer[index]=='#') continue;
        return(&buffer[index]);
    };
}

int Load_OBJECT4DV1_PLG(OBJECT4DV1_PTR obj, char *filename, VECTOR4D_PTR scale, VECTOR4D pos, VECTOR4D rot)
{
    FILE *fp;
    char buffer[256];
    char *token_string;

    memset(obj, 0, sizeof(OBJECT4DV1));
    obj->state = OBJECT4DV1_STATE_ACTIVE | OBJECT4DV1_STATE_VISIBLE;
    obj->world_pos.x = pos.x;
    obj->world_pos.y = pos.y;
    obj->world_pos.z = pos.z;
    obj->world_pos.w = pos.w;

    if(!(fp=fopen(filename, "r")))
    {
        printf("couldn't open PLG file %s", filename);
        return (0);
    }

    if (!(token_string = Get_Line_PLG(buffer, 255, fp)))
    {
        printf("PLG file error with file %s (obj descriptor invalid)",filename);
        return 0;
    }

    sscanf(token_string, "%s %d %d", obj->name, &obj->num_vertices, &obj->num_polys);
    for (int vertex=0; vertex < obj->num_vertices; vertex++)
    {
        if(!(token_string = Get_Line_PLG(buffer, 255, fp)))
        {
            printf("PLG file error with file %s (vertex list invalid)", filename);
            return 0;
        }
        sscanf(token_string, "%f %f %f", &obj->vlist_local[vertex].x,
                                         &obj->vlist_local[vertex].y,
                                         &obj->vlist_local[vertex].z);
        obj->vlist_local[vertex].w = 1;
        obj->vlist_local[vertex].x *= scale->x;
        obj->vlist_local[vertex].y *= scale->y;
        obj->vlist_local[vertex].z *= scale->z;
    }

    Compute_OBJECT4DV1_Radius(obj);
    int poly_surface_desc = 0;
    int poly_num_verts = 0;
    char tmp_string[8];

    for(int poly=0; poly<obj->num_polys; poly++)
    {
        if(!(token_string = Get_Line_PLG(buffer, 255, fp)))
        {
            printf("PLG file error with file %s(polygon descriptor invalid)", filename);
            return 0;
        }
        sscanf(token_string, "%s %d %d %d %d", tmp_string, &poly_num_verts,
               &obj->plist[poly].vert[0], &obj->plist[poly].vert[1], &obj->plist[poly].vert[2]);
        if(tmp_string[0] == '0' && toupper(tmp_string[1]) == 'X')
            sscanf(tmp_string, "%x", &poly_surface_desc);
        else
            poly_surface_desc = atoi(tmp_string);
        obj->plist[poly].vlist = obj->vlist_local;
        if(poly_surface_desc & PLX_2SIDED_FLAG)
        {
            SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_2SIDED);
        }
        else
        {
            //one side
        }
        if(poly_surface_desc & PLX_COLOR_MODE_RGB_FLAG)
        {
            SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_RGB16);
            int red = ((poly_surface_desc & 0x0f00) >> 8);
            int green = ((poly_surface_desc & 0x00f0) >> 4);
            int blue = (poly_surface_desc & 0x000f);
            obj->plist[poly].color = RGB16BIT(red*16, green*16, blue*16);
        }
        else
        {
            SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_8BITCOLOR);
            obj->plist[poly].color = (poly_surface_desc & 0x00ff);
        }

        int shade_mode = (poly_surface_desc & PLX_SHADE_MODE_MASK);
        switch(shade_mode)
        {
            case PLX_SHADE_MODE_PURE_FLAG:{
                SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_SHADE_MODE_PURE);
            }break;
            case PLX_SHADE_MODE_FLAT_FLAG:{
                SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_SHADE_MODE_FLAT);
            }break;
            case PLX_SHADE_MODE_GOURAUD_FLAG:{
                SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_SHADE_MODE_GOURAUD);
            }break;
            case PLX_SHADE_MODE_PHONG_FLAG:{
                SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_SHADE_MODE_PHONG);
            }break;
            default:break;
        }
        obj->plist[poly].state = POLY4DV1_STATE_ACTIVE;
    }

    fclose(fp);
    return 1;
}


