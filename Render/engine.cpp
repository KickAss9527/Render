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

int Load_OBJECT4DV1_PLG(OBJECT4DV1_PTR obj, char *filename, VECTOR4D_PTR scale, VECTOR4D_PTR pos, VECTOR4D_PTR rot)
{
    FILE *fp;
    char buffer[256];
    char *token_string;

    memset(obj, 0, sizeof(OBJECT4DV1));
    obj->state = OBJECT4DV1_STATE_ACTIVE | OBJECT4DV1_STATE_VISIBLE;
    obj->world_pos.x = pos->x;
    obj->world_pos.y = pos->y;
    obj->world_pos.z = pos->z;
    obj->world_pos.w = pos->w;

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

void Transform_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, MATRIX4X4_PTR mt, int coord_select)
{
    switch(coord_select)
    {
        case TRANSFORM_LOCAL_ONLY:
        {
            for(int poly=0; poly<rend_list->num_polys; poly++)
            {
                POLY4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
                if((curr_poly==NULL) ||
                   !(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
                   (curr_poly->state & POLY4DV1_STATE_CLIPPED) ||
                   (curr_poly->state & POLY4DV1_STATE_BACKFACE))
                    continue;

                for(int vertex=0; vertex<3; vertex++)
                {
                    POINT4D presult;
                    Mat_Mul_VECTOR4D_4X4(&curr_poly->vlist[vertex], mt, &presult);
                    VECTOR4D_COPY(&curr_poly->vlist[vertex], &presult);
                }
            }
        } break;
        case TRANSFORM_TRANS_ONLY:
        {
            for(int poly=0; poly<rend_list->num_polys; poly++)
            {
                POLY4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
                if((curr_poly==NULL) ||
                   !(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
                   (curr_poly->state & POLY4DV1_STATE_CLIPPED) ||
                   (curr_poly->state & POLY4DV1_STATE_BACKFACE))
                    continue;

                for(int vertex=0; vertex<3; vertex++)
                {
                    POINT4D presult;
                    Mat_Mul_VECTOR4D_4X4(&curr_poly->vlist[vertex], mt, &presult);
                    VECTOR4D_COPY(&curr_poly->tvlist[vertex], &presult);
                }
            }
        } break;
        case TRANSFORM_LOCAL_TO_TRANS:
        {
            for(int poly=0; poly<rend_list->num_polys; poly++)
            {
                POLY4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
                if((curr_poly==NULL) ||
                   !(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
                   (curr_poly->state & POLY4DV1_STATE_CLIPPED) ||
                   (curr_poly->state & POLY4DV1_STATE_BACKFACE))
                    continue;

                for(int vertex=0; vertex<3; vertex++)
                {
                    Mat_Mul_VECTOR4D_4X4(&curr_poly->vlist[vertex], mt, &curr_poly->tvlist[vertex]);
                }
            }
        } break;
        default:break;
    }
}

void Transform_OBJECT4DV1(OBJECT4DV1_PTR obj, MATRIX4X4_PTR mt, int coord_select, int transform_basis)
{
    switch(coord_select)
    {
        case TRANSFORM_LOCAL_ONLY:
        {
            for(int vertex=0; vertex<obj->num_vertices; vertex++)
            {
                POINT4D presult;
                Mat_Mul_VECTOR4D_4X4(&obj->vlist_local[vertex], mt, &presult);
                VECTOR4D_COPY(&obj->vlist_local[vertex], &presult);

            }
        }break;
        case TRANSFORM_TRANS_ONLY:
        {
            for(int vertex=0; vertex<obj->num_vertices; vertex++)
            {
                POINT4D presult;
                Mat_Mul_VECTOR4D_4X4(&obj->vlist_trans[vertex], mt, &presult);
                VECTOR4D_COPY(&obj->vlist_trans[vertex], &presult);
            }
        }break;
        case TRANSFORM_LOCAL_TO_TRANS:
        {
            for(int vertex=0; vertex<obj->num_vertices; vertex++)
            {
                Mat_Mul_VECTOR4D_4X4(&obj->vlist_trans[vertex], mt, &obj->vlist_trans[vertex]);
            }
        }break;
        default: break;
    }

    if (transform_basis)
    {
        VECTOR4D vresult;
        Mat_Mul_VECTOR4D_4X4(&obj->ux, mt, &vresult);
        VECTOR4D_COPY(&obj->ux, &vresult);
        Mat_Mul_VECTOR4D_4X4(&obj->uy, mt, &vresult);
        VECTOR4D_COPY(&obj->uy, &vresult);
        Mat_Mul_VECTOR4D_4X4(&obj->uz, mt, &vresult);
        VECTOR4D_COPY(&obj->uz, &vresult);
    }
}

void Model_To_World_OBJECT4DV1(OBJECT4DV1_PTR obj, int coord_select = TRANSFORM_LOCAL_TO_TRANS)
{
    if (coord_select == TRANSFORM_LOCAL_TO_TRANS)
    {
        for(int vertex=0; vertex<obj->num_vertices; vertex++)
        {
            VECTOR4D_Add(&obj->vlist_local[vertex], &obj->world_pos, &obj->vlist_trans[vertex]);
        }
    }
    else //transform_trans_only
    {
        for(int vertex=0; vertex<obj->num_vertices; vertex++)
        {
            VECTOR4D_Add(&obj->vlist_trans[vertex], &obj->world_pos, &obj->vlist_trans[vertex]);
        }
    }
}

void Model_To_World_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, POINT4D_PTR world_pos, int coord_select = TRANSFORM_LOCAL_TO_TRANS)
{
    if(coord_select == TRANSFORM_LOCAL_TO_TRANS)
    {
        for (int poly=0; poly<rend_list->num_polys; poly++)
        {
            POLY4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
            if ((curr_poly==NULL) ||
                !(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
                (curr_poly->state & POLY4DV1_STATE_BACKFACE) ||
                (curr_poly->state & POLY4DV1_STATE_CLIPPED))
                continue;
            for (int vertex=0; vertex<3; vertex++)
            {
                VECTOR4D_Add(&curr_poly->vlist[vertex], world_pos, &curr_poly->tvlist[vertex]);
            }
        }
    }
    else
    {
        for (int poly=0; poly<rend_list->num_polys; poly++)
        {
            POLY4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
            if ((curr_poly==NULL) ||
                !(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
                (curr_poly->state & POLY4DV1_STATE_BACKFACE) ||
                (curr_poly->state & POLY4DV1_STATE_CLIPPED))
                continue;
            for (int vertex=0; vertex<3; vertex++)
            {
                VECTOR4D_Add(&curr_poly->tvlist[vertex], world_pos, &curr_poly->tvlist[vertex]);
            }
        }
    }
}

void Init_CAM4DV1(CAM4DV1_PTR cam,
                  int cam_attr,
                  POINT4D_PTR cam_pos,
                  VECTOR4D_PTR cam_dir,
                  POINT4D_PTR cam_target,
                  float near_clip_z,
                  float far_clip_z,
                  float fov,
                  float viewport_width,
                  float viewport_height)
{
    cam->attr = cam_attr;
    VECTOR4D_COPY(&cam->pos, cam_pos);
    VECTOR4D_COPY(&cam->dir, cam_dir);

    VECTOR4D_INITXYZ(&cam->u, 1,0,0);
    VECTOR4D_INITXYZ(&cam->u, 0,1,0);
    VECTOR4D_INITXYZ(&cam->u, 0,0,1);

    if(cam_target!=NULL)
        VECTOR4D_COPY(&cam->target, cam_target);
    else
        VECTOR4D_ZERO(&cam->target);

    cam->near_clip_z = near_clip_z;
    cam->far_clip_z = far_clip_z;
    cam->viewport_width = viewport_width;
    cam->viewport_height = viewport_height;
    cam->viewport_center_x = (viewport_width-1)*0.5;
    cam->viewport_center_y = (viewport_height-1)*0.5;
    cam->aspect_ratio = viewport_width/viewport_width;
    cam->fov = fov;

    if(fov == 90.0)
    {
        POINT3D pt_origin;
        VECTOR3D_INITXYZ(&pt_origin, 0,0,0);
        VECTOR3D vn;

        VECTOR3D_INIT(&vn, 1,0,-1);
        PLANE3D_Init(&cam->rt_clip_plane, &pt_origin, &vn, 1);

        VECTOR3D_INIT(&vn, -1,0,-1);
        PLANE3D_Init(&cam->lt_clip_plane, &pt_origin, &vn, 1);

        VECTOR3D_INIT(&vn, 0,1,-1);
        PLANE3D_Init(&cam->tp_clip_plane, &pt_origin, &vn, 1);

        VECTOR3D_INIT(&vn, 0,-1,-1);
        PLANE3D_Init(&cam->bt_clip_plane, &pt_origin, &vn, 1);
    }
    else
    {
        POINT3D pt_origin;
        VECTOR3D_INITXYZ(&pt_origin, 0, 0, 0);
        VECTOR3D vn;

        VECTOR3D_INIT(&vn, cam->view_dist,0,-cam->viewplane_width*0.5);
        PLANE3D_Init(&cam->rt_clip_plane, &pt_origin, &vn, 1);

        VECTOR3D_INIT(&vn, -cam->view_dist,0,-cam->viewplane_width*0.5);
        PLANE3D_Init(&cam->lt_clip_plane, &pt_origin, &vn, 1);

        VECTOR3D_INIT(&vn, 0,cam->view_dist,-cam->viewplane_width*0.5);
        PLANE3D_Init(&cam->tp_clip_plane, &pt_origin, &vn, 1);

        VECTOR3D_INIT(&vn, 0,-cam->view_dist,-cam->viewplane_width*0.5);
        PLANE3D_Init(&cam->bt_clip_plane, &pt_origin, &vn, 1);
    }

    MAT_IDENTITY_4X4(&cam->mcam);
    MAT_IDENTITY_4X4(&cam->mper);
    MAT_IDENTITY_4X4(&cam->mscr);
    cam->viewplane_width = 2/cam->aspect_ratio;
    float tan_fov_div2 = tan(DEG_TO_RAD(fov/2));
    cam->view_dist = 0.5*cam->viewplane_width*tan_fov_div2;

}
