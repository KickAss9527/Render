#include <iostream>
#include <string.h>
#include <stdio.h>
#include "engine.h"
#include <stdlib.h>
#include <math.h>

LIGHTV1 gLights[MAX_LIGHTS];
int gNum_lights;
int dd_pixel_format;

LIGHTV1_PTR GetLightList(void)
{
    return gLights;
}
int Reset_Lights_LIGHTV1(void)
{
    dd_pixel_format = DD_PIXEL_FORMAT888;
    static int first_time = 1;
    memset(gLights, 0, MAX_LIGHTS*sizeof(LIGHTV1));
    gNum_lights = 0;
    first_time = 0;
    return 1;
}

int Init_Light_LIGHTV1(int index,
                       int _state,
                       int _attr,
                       RGBAV1 _c_ambient,
                       RGBAV1 _c_diffuse,
                       RGBAV1 _c_specular,
                       POINT4D_PTR _pos,
                       VECTOR4D_PTR _dir,
                       float _kc,
                       float _kl,
                       float _kq,
                       float _spot_inner,
                       float _spot_outer,
                       float _pf)
{
    if (index<0 || index>=MAX_LIGHTS) return 0;

    gLights[index].state = _state;
    gLights[index].id = index;
    gLights[index].attr = _attr;
    gLights[index].c_ambient = _c_ambient;
    gLights[index].c_diffuse = _c_diffuse;
    gLights[index].c_specular = _c_specular;
    gLights[index].kc = _kc;
    gLights[index].kl = _kl;
    gLights[index].kq = _kq;

    if(_pos)
    {
        VECTOR4D_COPY(&gLights[index].pos, _pos);
    }
    if(_dir)
    {
        VECTOR4D_COPY(&gLights[index].dir, _dir);
        VECTOR4D_Normalize(&gLights[index].dir);
    }
    gLights[index].spot_inner = _spot_inner;
    gLights[index].spot_outer = _spot_outer;
    gLights[index].pf = _pf;
    return index;
}

int Insert_POLY4DV1_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, POLY4DV1_PTR poly)
{
    if (rend_list->num_polys >= RENDERLIST4DV1_MAX_POLYS) return 0;

    rend_list->poly_ptrs[rend_list->num_polys] = &rend_list->poly_data[rend_list->num_polys];
    rend_list->poly_data[rend_list->num_polys].state = poly->state;
    rend_list->poly_data[rend_list->num_polys].attr = poly->attr;
    rend_list->poly_data[rend_list->num_polys].color = poly->color;

    VECTOR4D_COPY(&rend_list->poly_data[rend_list->num_polys].tvlist[0],
                  &poly->vlist[poly->vert[0]]);
    VECTOR4D_COPY(&rend_list->poly_data[rend_list->num_polys].tvlist[1],
                  &poly->vlist[poly->vert[1]]);
    VECTOR4D_COPY(&rend_list->poly_data[rend_list->num_polys].tvlist[2],
                  &poly->vlist[poly->vert[2]]);
    VECTOR4D_COPY(&rend_list->poly_data[rend_list->num_polys].vlist[0],
                  &poly->vlist[poly->vert[0]]);
    VECTOR4D_COPY(&rend_list->poly_data[rend_list->num_polys].vlist[1],
                  &poly->vlist[poly->vert[1]]);
    VECTOR4D_COPY(&rend_list->poly_data[rend_list->num_polys].vlist[2],
                  &poly->vlist[poly->vert[2]]);

    if(rend_list->num_polys == 0)
    {
        rend_list->poly_data[0].next = NULL;
        rend_list->poly_data[0].prev = NULL;
    }
    else
    {
        rend_list->poly_data[0].next = NULL;
        rend_list->poly_data[rend_list->num_polys].prev = &rend_list->poly_data[rend_list->num_polys-1];
        rend_list->poly_data[rend_list->num_polys-1].next = &rend_list->poly_data[rend_list->num_polys];
    }
    rend_list->num_polys++;
    return 1;
}

int Insert_OBJECT4DV1_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list,
                                     OBJECT4DV1_PTR obj,
                                     int insert_local,
                                     int lighting_on)
{
    if(!(obj->state & OBJECT4DV1_STATE_ACTIVE) ||
       obj->state & OBJECT4DV1_STATE_CULLED ||
       !(obj->state & OBJECT4DV1_STATE_VISIBLE))
    {
        return 0;
    }

    for (int poly=0; poly < obj->num_polys; poly++)
    {
        POLY4DV1_PTR curr_poly = &obj->plist[poly];
        if(!(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
           curr_poly->state & POLY4DV1_STATE_CLIPPED ||
           curr_poly->state & POLY4DV1_STATE_BACKFACE)
        {
            continue;
        }
        POINT4D_PTR vlist_old = curr_poly->vlist;
        if(insert_local)
            curr_poly->vlist = obj->vlist_local;
        else
            curr_poly->vlist = obj->vlist_trans;

        unsigned int base_color;
        if(lighting_on)
        {
            base_color = (unsigned int)curr_poly->color;
            curr_poly->color = (int)(base_color>>16);
        }

        if(!Insert_POLY4DV1_RENDERLIST4DV1(rend_list, curr_poly))
        {
            curr_poly->vlist = vlist_old;
            return 0;
        }

        if (lighting_on)
        {
            curr_poly->color = (int)base_color;
        }
        curr_poly->vlist = vlist_old;

    }
    return 1;
}

void RESET_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list)
{
    rend_list->num_polys = 0;
}

void Reset_OBJECT4DV1(OBJECT4DV1_PTR obj)
{
    RESET_BIT(obj->state, OBJECT4DV1_STATE_CULLED);
    for (int poly=0; poly<obj->num_polys; poly++)
    {
        POLY4DV1_PTR curr_poly = &obj->plist[poly];
        if (!(curr_poly->state & POLY4DV2_STATE_ACTIVE))
        {
            continue;
        }
        RESET_BIT(curr_poly->state, POLY4DV1_STATE_CLIPPED);
        RESET_BIT(curr_poly->state, POLY4DV1_STATE_BACKFACE);
    }
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
        //1000,0000,0000,0000    PLX_COLOR_MODE_RGB_FLAG
        //1010,1111,1111,1111    poly_surface_desc
        //1111,0000,0000,0000    f00
        //0000,1111,0000,0000    f0
        //0000,0000,1111,0000    f  
        if(poly_surface_desc & PLX_COLOR_MODE_RGB_FLAG)
        {
            SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_RGB24);
            int red = ((poly_surface_desc & 0xff0000) >> 16);
            int green = ((poly_surface_desc & 0x00ff00) >> 8);
            int blue = (poly_surface_desc & 0x000ff);
            obj->plist[poly].color = RGB24BIT(0,red, green, blue);
        }
        else
        {
            SET_BIT(obj->plist[poly].attr, POLY4DV1_ATTR_8BITCOLOR);
            obj->plist[poly].color = (poly_surface_desc & 0x00ff);
        }
        //110,0000,0000,0000   PLX_SHADE_MODE_MASK
        //010,0000,0000,0000   PLX_SHADE_MODE_FLAT_FLAG
        //100,0000,0000,0000   PLX_SHADE_MODE_GOURAUD_FLAG
        //110,0000,0000,0000   PLX_SHADE_MODE_PHONG_FLAG
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
                POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
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
                POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
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
                POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
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

void Model_To_World_OBJECT4DV1(OBJECT4DV1_PTR obj, int coord_select)
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

void Model_To_World_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, POINT4D_PTR world_pos,
                                   int coord_select)
{
    if(coord_select == TRANSFORM_LOCAL_TO_TRANS)
    {
        for (int poly=0; poly<rend_list->num_polys; poly++)
        {
            POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
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
            POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
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


void Build_CAM4DV1_Matrix_Euler(CAM4DV1_PTR cam, int cam_rot_seq)
{
    MATRIX4X4 mt_inv, mx_inv, my_inv, mz_inv, mrot, mtmp;
    Mat_Init_4X4(&mt_inv, 1, 0, 0, 0,
                          0, 1, 0, 0,
                          0, 0, 1, 0,
                          -cam->pos.x, -cam->pos.y, -cam->pos.z, 1);
    float theta_x = cam->dir.x;
    float theta_y = cam->dir.y;
    float theta_z = cam->dir.z;

    float cos_theta = Fast_Cos(theta_x);
    float sin_theta = -Fast_Sin(theta_x);
    Mat_Init_4X4(&mx_inv, 1, 0, 0, 0,
                          0, cos_theta, sin_theta, 0,
                          0, -sin_theta, cos_theta, 0,
                          0, 0, 0, 1);

    cos_theta = Fast_Cos(theta_y);
    sin_theta = -Fast_Sin(theta_y);
    Mat_Init_4X4(&my_inv, cos_theta, 0, -sin_theta, 0,
                          0, 1, 0, 0,
                          sin_theta, 0, cos_theta, 0,
                          0, 0, 0, 1);

    cos_theta = Fast_Cos(theta_z);
    sin_theta = -Fast_Sin(theta_z);
    Mat_Init_4X4(&mz_inv, cos_theta, sin_theta, 0, 0,
                          -sin_theta, cos_theta, 0, 0,
                          0, 0, 1, 0,
                          0, 0, 0, 1);

    switch(cam_rot_seq)
    {
        case CAM_ROT_SEQ_XYZ:
        {
            Mat_Mul_4X4(&mx_inv, &my_inv, &mtmp);
            Mat_Mul_4X4(&mtmp, &mz_inv, &mrot);
        }break;
        case CAM_ROT_SEQ_YXZ:
        {
            Mat_Mul_4X4(&my_inv, &mx_inv, &mtmp);
            Mat_Mul_4X4(&mtmp, &mz_inv, &mrot);
        }break;
        case CAM_ROT_SEQ_XZY:
        {
            Mat_Mul_4X4(&mx_inv, &mz_inv, &mtmp);
            Mat_Mul_4X4(&mtmp, &my_inv, &mrot);
        }break;
        case CAM_ROT_SEQ_YZX:
        {
            Mat_Mul_4X4(&my_inv, &mz_inv, &mtmp);
            Mat_Mul_4X4(&mtmp, &mx_inv, &mrot);
        }break;
        case CAM_ROT_SEQ_ZYX:
        {
            Mat_Mul_4X4(&mz_inv, &my_inv, &mtmp);
            Mat_Mul_4X4(&mtmp, &mx_inv, &mrot);
        }break;
        case CAM_ROT_SEQ_ZXY:
        {
            Mat_Mul_4X4(&mz_inv, &mx_inv, &mtmp);
            Mat_Mul_4X4(&mtmp, &my_inv, &mrot);
        }break;
        default: break;
    }
    Mat_Mul_4X4(&mt_inv, &mrot, &cam->mcam);
}

void Build_CAM4DV1_Matrix_UVN(CAM4DV1_PTR cam, int mode)
{
    MATRIX4X4 mt_inv, mt_uvn, mtmp;
    Mat_Init_4X4(&mt_inv, 1, 0, 0, 0,
                          0, 1, 0, 0,
                          0, 0, 1, 0,
                          -cam->pos.x, -cam->pos.y, -cam->pos.z, 1);
    if (mode == UVN_MODE_SPHERICAL)
    {
        float phi = cam->dir.x;
        float theta = cam->dir.y;

        float sin_phi = Fast_Sin(phi);
        float cos_phi = Fast_Cos(phi);

        float sin_theta = Fast_Sin(theta);
        float cos_theta = Fast_Cos(theta);

        cam->target.x = -sin_phi*sin_theta;
        cam->target.y = cos_phi;
        cam->target.z = sin_phi*cos_theta;
    }
    VECTOR4D_Build(&cam->pos, &cam->target, &cam->n);
    VECTOR4D_INITXYZ(&cam->v, 0, 1, 0);
    VECTOR4D_Cross(&cam->v, &cam->n, &cam->u);
    VECTOR4D_Cross(&cam->n, &cam->u, &cam->v);

    VECTOR4D_Normalize(&cam->u);
    VECTOR4D_Normalize(&cam->n);
    VECTOR4D_Normalize(&cam->v);

    Mat_Mul_4X4(&mt_inv, &mt_uvn, &cam->mcam);

}

void World_To_Camera_OBJECT4DV1(CAM4DV1_PTR cam, OBJECT4DV1_PTR obj)
{
    for (int vertex=0; vertex<obj->num_vertices; vertex++)
    {
        POINT4D presult;
        Mat_Mul_VECTOR4D_4X4(&obj->vlist_trans[vertex], &cam->mcam, &presult);
        VECTOR4D_COPY(&obj->vlist_trans[vertex], &presult);
    }
}

void World_To_Camera_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam)
{
    for (int poly=0; poly<rend_list->num_polys; poly++)
    {
        POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
        if (curr_poly==NULL ||
            !(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
            curr_poly->state & POLY4DV1_STATE_CLIPPED ||
            curr_poly->state & POLY4DV1_STATE_BACKFACE)
        {
            continue;
        }

        for (int vertex=0; vertex<3; vertex++)
        {
            POINT4D presult;
            Mat_Mul_VECTOR4D_4X4(&curr_poly->tvlist[vertex], &cam->mcam, &presult);
            VECTOR4D_COPY(&curr_poly->tvlist[vertex], &presult);
        }
    }
}

int Cull_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam, int cull_flags)
{
    POINT4D sphere_pos;
    Mat_Mul_VECTOR4D_4X4(&obj->world_pos, &cam->mcam, &sphere_pos);

    if (cull_flags & CULL_OBJECT_Z_PLANE)
    {
        if((sphere_pos.z - obj->max_radius > cam->far_clip_z) ||
           (sphere_pos.z + obj->max_radius < cam->near_clip_z))
        {
            SET_BIT(obj->state, OBJECT4DV1_STATE_CULLED);
            return 1;
        }
    }
    if (cull_flags & CULL_OBJECT_X_PLANE)
    {
        float z_test = 0.5*cam->viewplane_width*sphere_pos.z/cam->view_dist;
        if((sphere_pos.x - obj->max_radius > z_test) ||
           (sphere_pos.x + obj->max_radius < -z_test))
        {
            SET_BIT(obj->state, OBJECT4DV1_STATE_CULLED);
            return 1;
        }
    }
    if (cull_flags & CULL_OBJECT_Y_PLANE)
    {
        float z_test = 0.5*cam->viewplane_height*sphere_pos.z/cam->view_dist;
        if((sphere_pos.y - obj->max_radius > z_test) ||
           (sphere_pos.y + obj->max_radius < -z_test))
        {
            SET_BIT(obj->state, OBJECT4DV1_STATE_CULLED);
            return 1;
        }
    }
    return 0;
}

void Remove_Backfaces_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam)
{
    if(obj->state & OBJECT4DV1_STATE_CULLED)
        return;
    for (int poly=0; poly<obj->num_polys; poly++)
    {
        POLY4DV1_PTR curr_poly = &obj->plist[poly];

        if(!(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
           (curr_poly->state & POLY4DV1_STATE_CLIPPED) ||
           (curr_poly->attr & POLY4DV1_ATTR_2SIDED) ||
           (curr_poly->state & POLY4DV1_STATE_BACKFACE))
        {
            continue;
        }

        int vindex_0 = curr_poly->vert[0];
        int vindex_1 = curr_poly->vert[1];
        int vindex_2 = curr_poly->vert[2];

        VECTOR4D u,v,n;
        VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_1], &u);
        VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_2], &v);

        VECTOR4D_Cross(&u, &v, &n);
        VECTOR4D view;
        VECTOR4D_Build(&obj->vlist_trans[vindex_0], &cam->pos, &view);

        float dp = VECTOR4D_Dot(&n, &view);
        printf("%f, ", dp);
        if(dp <= 0)
        {
            SET_BIT(curr_poly->state, POLY4DV1_STATE_BACKFACE);
        }
    }
}

void Remove_Backfaces_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam)
{
    for (int poly=0; poly<rend_list->num_polys; poly++)
    {
        POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
        if((curr_poly == NULL) ||
           !(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
           (curr_poly->state & POLY4DV1_STATE_CLIPPED) ||
           (curr_poly->state & POLY4DV1_ATTR_2SIDED) ||
           (curr_poly->state & POLY4DV1_STATE_BACKFACE))
        {
            continue;
        }

        VECTOR4D u,v,n;
        VECTOR4D_Build(&curr_poly->tvlist[0], &curr_poly->tvlist[1], &u);
        VECTOR4D_Build(&curr_poly->tvlist[0], &curr_poly->tvlist[2], &v);

        VECTOR4D_Cross(&u, &v, &n);
        VECTOR4D view;
        VECTOR4D_Build(&curr_poly->tvlist[0], &cam->pos, &view);

        float dp = VECTOR4D_Dot(&n, &view);
        if(dp <= 0)
            SET_BIT(curr_poly->state, POLY4DV1_STATE_BACKFACE);
    }
}

void Camera_To_Perspective_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam)
{
    for (int vertex=0; vertex<obj->num_vertices; vertex++)
    {
        float z = obj->vlist_trans[vertex].z;
        obj->vlist_trans[vertex].x = cam->view_dist*obj->vlist_trans[vertex].x/z;
        obj->vlist_trans[vertex].y = cam->view_dist*obj->vlist_trans[vertex].y*cam->aspect_ratio/z;
    }
}

void Build_Camera_To_Perspective_MATRIX4X4(CAM4DV1_PTR cam, MATRIX4X4_PTR m)
{
    Mat_Init_4X4(m, cam->view_dist_h, 0, 0, 0,
                    0, cam->view_dist_v*cam->aspect_ratio, 0, 0,
                    0, 0, 1, 1,
                 0, 0, 0, 0);
}

void Convert_From_Homogeneous4D_OBJECT4DV1(OBJECT4DV1_PTR obj)
{
    for (int vertex=0; vertex<obj->num_vertices; vertex++)
    {
        VECTOR4D_DIV_BY_W(&obj->vlist_trans[vertex]);
    }
}

void Camera_To_Perspective_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam)
{
    for (int poly=0; poly<rend_list->num_polys; poly++)
    {
        POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
        if ((curr_poly==NULL) ||
            !(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
            (curr_poly->state & POLY4DV1_STATE_BACKFACE) ||
            (curr_poly->state & POLY4DV1_STATE_CLIPPED))
        {
            continue;
        }
        for (int vertex=0; vertex<3; vertex++)
        {
            float z = curr_poly->tvlist[vertex].z;
            curr_poly->tvlist[vertex].x = cam->view_dist*curr_poly->tvlist[vertex].x/z;
            curr_poly->tvlist[vertex].y = cam->view_dist*curr_poly->tvlist[vertex].y*cam->aspect_ratio/z;
        }
    }
}

void Convert_From_Homogeneous4D_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list)
{
    for (int poly=0; poly<rend_list->num_polys; poly++)
    {
        POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
        if ((curr_poly==NULL) ||
            !(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
            (curr_poly->state & POLY4DV1_STATE_BACKFACE) ||
            (curr_poly->state & POLY4DV1_STATE_CLIPPED))
        {
            continue;
        }
        for (int vertex=0; vertex<3; vertex++)
        {
            VECTOR4D_DIV_BY_W(&curr_poly->tvlist[vertex]);
        }
    }
}

void Perspective_To_Screen_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam)
{
    float alpha = 0.5*cam->viewport_width-0.5;
    float beta = 0.5*cam->viewport_height-0.5;

    for (int vertex=0; vertex<obj->num_vertices; vertex++)
    {
        obj->vlist_trans[vertex].x = alpha + alpha*obj->vlist_trans[vertex].x;
        obj->vlist_trans[vertex].y = beta - beta*obj->vlist_trans[vertex].y;
    }
}

void Perspective_To_Screen_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam)
{
    for (int poly=0; poly<rend_list->num_polys; poly++)
    {
        POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
        if ((curr_poly==NULL) ||
            !(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
            (curr_poly->state & POLY4DV1_STATE_BACKFACE) ||
            (curr_poly->state & POLY4DV1_STATE_CLIPPED))
        {
            continue;
        }
        float alpha = 0.5*cam->viewport_width-0.5;
        float beta = 0.5*cam->viewport_height-0.5;
        for (int vertex=0; vertex<3; vertex++)
        {
            curr_poly->tvlist[vertex].x = alpha + alpha*curr_poly->tvlist[vertex].x;
            curr_poly->tvlist[vertex].y = beta - beta*curr_poly->tvlist[vertex].y;
        }
    }
}

void Camera_To_Perspective_Screen_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam)
{
    float alpha = 0.5*cam->viewport_width-0.5;
    float beta = 0.5*cam->viewport_height-0.5;
    for (int vertex=0; vertex<obj->num_vertices; vertex++)
    {
        float z = obj->vlist_trans[vertex].z;
        obj->vlist_trans[vertex].x = cam->view_dist_h*obj->vlist_trans[vertex].x/z;
        obj->vlist_trans[vertex].y = cam->view_dist_v*obj->vlist_trans[vertex].y*cam->aspect_ratio/z;
        obj->vlist_trans[vertex].x = alpha + obj->vlist_trans[vertex].x;
        obj->vlist_trans[vertex].y = beta - obj->vlist_trans[vertex].y;//juri
    }
}

void Camera_To_Perspective_Screen_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam)
{
    for (int poly=0; poly<rend_list->num_polys; poly++)
    {
        POLYF4DV1_PTR curr_poly = rend_list->poly_ptrs[poly];
        if ((curr_poly==NULL) ||
            !(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
            (curr_poly->state & POLY4DV1_STATE_BACKFACE) ||
            (curr_poly->state & POLY4DV1_STATE_CLIPPED))
        {
            continue;
        }
        float alpha = 0.5*cam->viewport_width-0.5;
        float beta = 0.5*cam->viewport_height-0.5;
        for (int vertex=0; vertex<3; vertex++)
        {
            float z = curr_poly->tvlist[vertex].z;
            curr_poly->tvlist[vertex].x = cam->view_dist_h*curr_poly->tvlist[vertex].x/z;
            curr_poly->tvlist[vertex].y = cam->view_dist_v*curr_poly->tvlist[vertex].y*cam->aspect_ratio/z;
            curr_poly->tvlist[vertex].x = alpha + curr_poly->tvlist[vertex].x;
            curr_poly->tvlist[vertex].y = beta - curr_poly->tvlist[vertex].y;
        }
    }
}

void Build_Perspective_To_Screen_4D_MATRIX4X4(CAM4DV1_PTR cam, MATRIX4X4_PTR m)
{
    float alpha = 0.5*cam->viewport_width-0.5;
    float beta = 0.5*cam->viewport_height-0.5;
    Mat_Init_4X4(m, alpha, 0, 0, 0,
                    0, -beta, 0, 0,
                    alpha, beta, 1, 0,
                    0, 0, 0, 1);
}

void Build_Perspective_To_Screen_MATRIX4X4(CAM4DV1_PTR cam, MATRIX4X4_PTR m)
{
    float alpha = 0.5*cam->viewport_width-0.5;
    float beta = 0.5*cam->viewport_height-0.5;
    Mat_Init_4X4(m, alpha, 0, 0, 0,
                 0, -beta, 0, 0,
                 alpha, beta, 1, 0,
                 0, 0, 0, 1);
}

void Init_CAM4DV1(CAM4DV1_PTR cam,
                 // int cam_attr,
                  POINT4D_PTR cam_pos,
                  VECTOR4D_PTR cam_dir,
                //  POINT4D_PTR cam_target,
                  float near_clip_z,
                  float far_clip_z,
                  float fov,
                  float viewport_width,
                  float viewport_height)
{
    //cam->attr = cam_attr;
    VECTOR4D_COPY(&cam->pos, cam_pos);
    VECTOR4D_COPY(&cam->dir, cam_dir);

    VECTOR4D_INITXYZ(&cam->u, 1,0,0);
    VECTOR4D_INITXYZ(&cam->u, 0,1,0);
    VECTOR4D_INITXYZ(&cam->u, 0,0,1);

//    if(cam_target!=NULL)
//        VECTOR4D_COPY(&cam->target, cam_target);
//    else
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

        VECTOR3D_INITXYZ(&vn, 1,0,-1);
        PLANE3D_Init(&cam->rt_clip_plane, &pt_origin, &vn, 1);

        VECTOR3D_INITXYZ(&vn, -1,0,-1);
        PLANE3D_Init(&cam->lt_clip_plane, &pt_origin, &vn, 1);

        VECTOR3D_INITXYZ(&vn, 0,1,-1);
        PLANE3D_Init(&cam->tp_clip_plane, &pt_origin, &vn, 1);

        VECTOR3D_INITXYZ(&vn, 0,-1,-1);
        PLANE3D_Init(&cam->bt_clip_plane, &pt_origin, &vn, 1);
    }
    else
    {
        POINT3D pt_origin;
        VECTOR3D_INITXYZ(&pt_origin, 0, 0, 0);
        VECTOR3D vn;

        VECTOR3D_INITXYZ(&vn, cam->view_dist,0,-cam->viewplane_width*0.5);
        PLANE3D_Init(&cam->rt_clip_plane, &pt_origin, &vn, 1);

        VECTOR3D_INITXYZ(&vn, -cam->view_dist,0,-cam->viewplane_width*0.5);
        PLANE3D_Init(&cam->lt_clip_plane, &pt_origin, &vn, 1);

        VECTOR3D_INITXYZ(&vn, 0,cam->view_dist,-cam->viewplane_width*0.5);
        PLANE3D_Init(&cam->tp_clip_plane, &pt_origin, &vn, 1);

        VECTOR3D_INITXYZ(&vn, 0,-cam->view_dist,-cam->viewplane_width*0.5);
        PLANE3D_Init(&cam->bt_clip_plane, &pt_origin, &vn, 1);
    }

    MAT_IDENTITY_4X4(&cam->mcam);
    MAT_IDENTITY_4X4(&cam->mper);
    MAT_IDENTITY_4X4(&cam->mscr);
    cam->viewplane_width = 2/cam->aspect_ratio;
    float tan_fov_div2 = tan(DEG_TO_RAD(fov/2));
    cam->view_dist = 0.5*cam->viewplane_width*tan_fov_div2;
}

int Light_RENDERLIST4DV1_World16(RENDERLIST4DV1_PTR rend_list,
                                 CAM4DV1_PTR cam,
                                 LIGHTV1_PTR lights,
                                 int max_lights)
{
    unsigned int r_base, g_base, b_base, r_sum, g_sum, b_sum, shared_color;
    float dp, dist, i, nl, atten;
    
    for (int poly=0; poly<rend_list->num_polys; poly++)
    {
        POLYF4DV1_PTR curr_poly = &rend_list->poly_data[poly];
        if(!(curr_poly->state & POLY4DV2_STATE_ACTIVE) ||
           curr_poly->state & POLY4DV2_STATE_CLIPPED ||
           curr_poly->state & POLY4DV2_STATE_BACKFACE)
        {
            continue;
        }
                
        if (curr_poly->attr & POLY4DV1_ATTR_SHADE_MODE_FLAT ||
            curr_poly->attr & POLY4DV1_ATTR_SHADE_MODE_GOURAUD)
        {
            if (dd_pixel_format == DD_PIXEL_FORMAT888)
            {
                RGB888FROM24BIT(curr_poly->color, &r_base, &g_base, &b_base);
            }
            else if(dd_pixel_format == DD_PIXEL_FORMAT565)
            {
                _RGB565FROM16BIT(curr_poly->color, &r_base, &g_base, &b_base);
                r_base <<= 3;
                g_base <<= 2;
                b_base <<= 3;
            }
            else
            {
                _RGB565FROM16BIT(curr_poly->color, &r_base, &g_base, &b_base);
                r_base <<= 3;
                g_base <<= 3;
                b_base <<= 3;
            }
            r_sum = g_sum = b_sum = 0;
            printf("(%d, %d, %d)\n", r_base, g_base, b_base);
            for (int curr_light=0; curr_light<max_lights; curr_light++)
            {
                if (!lights[curr_light].state)
                {
                    continue;
                }
                
                if (lights[curr_light].attr & LIGHTV1_ATTR_POINT)
                {
                    VECTOR4D u, v, n, l;
                    VECTOR4D_Build(&curr_poly->tvlist[0],
                                   &curr_poly->tvlist[1], &u);
                    VECTOR4D_Build(&curr_poly->tvlist[0],
                                   &curr_poly->tvlist[2], &v);
                    VECTOR4D_Cross(&u, &v, &n);
                    nl = VECTOR4D_Length(&n);
                    VECTOR4D_Build(&curr_poly->tvlist[0],
                                   &lights[curr_light].pos, &l);
                    dist = VECTOR4D_Length(&l);
                    dp = VECTOR4D_Dot(&n, &l);
                    if (dp > 0)
                    {
                        atten = (lights[curr_light].kc + lights[curr_light].kl*dist + lights[curr_light].kq*dist*dist);
                        i = 128*(nl*dist*atten);
                        r_sum += (lights[curr_light].c_diffuse.r * r_base * i)/(256*128);
                        g_sum += (lights[curr_light].c_diffuse.g * r_base * i)/(256*128);
                        b_sum += (lights[curr_light].c_diffuse.b * r_base * i)/(256*128);
                    }
                    
                }
                else if (lights[curr_light].attr & LIGHTV1_ATTR_AMBIENT)
                {
                    r_sum += ((lights[curr_light].c_ambient.r * r_base)/256);
                    g_sum += ((lights[curr_light].c_ambient.g * g_base)/256);
                    b_sum += ((lights[curr_light].c_ambient.b * b_base)/256);
                }
                else if (lights[curr_light].attr & LIGHTV1_ATTR_INFINITE) {
                    VECTOR4D u, v, n;
                    VECTOR4D_Build(&curr_poly->tvlist[0], &curr_poly->tvlist[1], &u);
                    VECTOR4D_Build(&curr_poly->tvlist[0], &curr_poly->tvlist[2], &v);
                    VECTOR4D_Cross(&u, &v, &n);
                    nl = VECTOR4D_Length(&n);
                    dp = VECTOR4D_Dot(&n, &lights[curr_light].dir);
                    if (dp > 0)
                    {
                        i = 128*dp/nl;
                        r_sum += (lights[curr_light].c_diffuse.r * r_base * i)/(256*128);
                        g_sum += (lights[curr_light].c_diffuse.g * g_base * i)/(256*128);
                        b_sum += (lights[curr_light].c_diffuse.b * b_base * i)/(256*128);
                    }
                }
                else if (lights[curr_light].attr & LIGHTV1_ATTR_SPOTLIGHT1)
                {
                    VECTOR4D u, v, n, l;
                    VECTOR4D_Build(&curr_poly->tvlist[0], &curr_poly->tvlist[1], &u);
                    VECTOR4D_Build(&curr_poly->tvlist[0], &curr_poly->tvlist[2], &v);
                    VECTOR4D_Cross(&u, &v, &n);
                    nl = VECTOR4D_Length(&n);
                    VECTOR4D_Build(&curr_poly->tvlist[0],
                                   &lights[curr_light].pos, &l);
                    dist = VECTOR4D_Length(&l);
                    dp = VECTOR4D_Dot(&n, &lights[curr_light].dir);
                    if (dp > 0)
                    {
                        atten = (lights[curr_light].kc + lights[curr_light].kl*dist + lights[curr_light].kq*dist*dist);
                        i = 128*(nl*dist*atten);
                        r_sum += (lights[curr_light].c_diffuse.r * r_base * i)/(256*128);
                        g_sum += (lights[curr_light].c_diffuse.g * r_base * i)/(256*128);
                        b_sum += (lights[curr_light].c_diffuse.b * r_base * i)/(256*128);
                    }
                }
                else if (lights[curr_light].attr & LIGHTV1_ATTR_SPOTLIGHT2)
                {
                    VECTOR4D u, v, n, d, s;
                    VECTOR4D_Build(&curr_poly->tvlist[0], &curr_poly->tvlist[1], &u);
                    VECTOR4D_Build(&curr_poly->tvlist[0], &curr_poly->tvlist[2], &v);
                    VECTOR4D_Cross(&u, &v, &n);
                    nl = VECTOR4D_Length(&n);
                    dp = VECTOR4D_Dot(&n, &lights[curr_light].dir);
                    if (dp > 0)
                    {
                        VECTOR4D_Build(&lights[curr_light].pos, &curr_poly->tvlist[0], &s);
                        dist = VECTOR4D_Length(&s);
                        float dpsl = VECTOR4D_Dot(&s, &lights[curr_light].dir)/dist;
                        if (dist > 0)
                        {
                            atten = (lights[curr_light].kc + lights[curr_light].kl*dist + lights[curr_light].kq*dist*dist);
                            float dpsl_exp = dpsl;
                            for (int e_index=1; e_index < (int)lights[curr_light].pf; e_index++)
                            {
                                dpsl_exp*=dpsl;
                            }
                            i = 128*dpsl_exp/(nl*atten);
                            r_sum += (lights[curr_light].c_diffuse.r * r_base * i)/(256*128);
                            g_sum += (lights[curr_light].c_diffuse.g * g_base * i)/(256*128);
                            b_sum += (lights[curr_light].c_diffuse.b * b_base * i)/(256*128);
                        }
                    }
                }
            }
            r_sum = MIN(255, r_sum);
            g_sum = MIN(255, g_sum);
            b_sum = MIN(255, b_sum);
            shared_color = RGB24BIT(0, r_sum, g_sum, b_sum);
            curr_poly->lcolor = shared_color;
            
        }
        else//POLY4DV1_ATTR_SHADE_MODE_CONSTANT
        {
            curr_poly->lcolor = curr_poly->color;
        }
    }
    
    return 1;
}

int Light_OBJECT4DV1_World16(OBJECT4DV1_PTR obj,
                             CAM4DV1_PTR cam,
                             LIGHTV1_PTR lights,
                             int max_lights)
{
    unsigned int r_base, g_base, b_base, r_sum, g_sum, b_sum, shared_color;
    float dp, dist, i, nl, atten;
    
    if (!(obj->state & OBJECT4DV1_STATE_ACTIVE) ||
        obj->state & OBJECT4DV1_STATE_CULLED ||
        !(obj->state & OBJECT4DV1_STATE_VISIBLE))
    {
        return 0;
    }
    
    for (int poly=0; poly<obj->num_polys; poly++)
    {
        POLY4DV1_PTR curr_poly = &obj->plist[poly];
        if(!(curr_poly->state & POLY4DV2_STATE_ACTIVE) ||
           curr_poly->state & POLY4DV2_STATE_CLIPPED ||
           curr_poly->state & POLY4DV2_STATE_BACKFACE)
        {
            continue;
        }
        
        int vindex_0 = curr_poly->vert[0];
        int vindex_1 = curr_poly->vert[1];
        int vindex_2 = curr_poly->vert[2];
        
        if (curr_poly->attr & POLY4DV1_ATTR_SHADE_MODE_FLAT ||
            curr_poly->attr & POLY4DV1_ATTR_SHADE_MODE_GOURAUD)
        {
            if (dd_pixel_format == DD_PIXEL_FORMAT888)
            {
                RGB888FROM24BIT(curr_poly->color, &r_base, &g_base, &b_base);
            }
            else if(dd_pixel_format == DD_PIXEL_FORMAT565)
            {
                _RGB565FROM16BIT(curr_poly->color, &r_base, &g_base, &b_base);
                r_base <<= 3;
                g_base <<= 2;
                b_base <<= 3;
            }
            else
            {
                _RGB565FROM16BIT(curr_poly->color, &r_base, &g_base, &b_base);
                r_base <<= 3;
                g_base <<= 3;
                b_base <<= 3;
            }
            r_sum = g_sum = b_sum = 0;
            printf("(%d, %d, %d)\n", r_base, g_base, b_base);
            for (int curr_light=0; curr_light<max_lights; curr_light++)
            {
                if (!lights[curr_light].state)
                {
                    continue;
                }
                
                if (lights[curr_light].attr & LIGHTV1_ATTR_POINT)
                {
                    VECTOR4D u, v, n, l;
                    VECTOR4D_Build(&obj->vlist_trans[vindex_0],
                                   &obj->vlist_trans[vindex_1], &u);
                    VECTOR4D_Build(&obj->vlist_trans[vindex_0],
                                   &obj->vlist_trans[vindex_2], &v);
                    VECTOR4D_Cross(&u, &v, &n);
                    nl = VECTOR4D_Length(&n);
                    VECTOR4D_Build(&obj->vlist_trans[vindex_0],
                                   &lights[curr_light].pos, &l);
                    dist = VECTOR4D_Length(&l);
                    dp = VECTOR4D_Dot(&n, &l);
                    if (dp > 0)
                    {
                        atten = (lights[curr_light].kc + lights[curr_light].kl*dist + lights[curr_light].kq*dist*dist);
                        i = 128*(nl*dist*atten);
                        r_sum += (lights[curr_light].c_diffuse.r * r_base * i)/(256*128);
                        g_sum += (lights[curr_light].c_diffuse.g * r_base * i)/(256*128);
                        b_sum += (lights[curr_light].c_diffuse.b * r_base * i)/(256*128);
                    }
                    
                }
                else if (lights[curr_light].attr & LIGHTV1_ATTR_AMBIENT)
                {
                    r_sum += ((lights[curr_light].c_ambient.r * r_base)/256);
                    g_sum += ((lights[curr_light].c_ambient.g * g_base)/256);
                    b_sum += ((lights[curr_light].c_ambient.b * b_base)/256);
                }
                else if (lights[curr_light].attr & LIGHTV1_ATTR_INFINITE) {
                    VECTOR4D u, v, n;
                    VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_1], &u);
                    VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_2], &v);
                    VECTOR4D_Cross(&u, &v, &n);
                    nl = VECTOR4D_Length(&n);
                    dp = VECTOR4D_Dot(&n, &lights[curr_light].dir);
                    if (dp > 0)
                    {
                        i = 128*dp/nl;
                        r_sum += (lights[curr_light].c_diffuse.r * r_base * i)/(256*128);
                        g_sum += (lights[curr_light].c_diffuse.g * g_base * i)/(256*128);
                        b_sum += (lights[curr_light].c_diffuse.b * b_base * i)/(256*128);
                    }
                }
                else if (lights[curr_light].attr & LIGHTV1_ATTR_SPOTLIGHT1)
                {
                    VECTOR4D u, v, n, l;
                    VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_1], &u);
                    VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_2], &v);
                    VECTOR4D_Cross(&u, &v, &n);
                    nl = VECTOR4D_Length(&n);
                    VECTOR4D_Build(&obj->vlist_trans[vindex_0],
                                   &lights[curr_light].pos, &l);
                    dist = VECTOR4D_Length(&l);
                    dp = VECTOR4D_Dot(&n, &lights[curr_light].dir);
                    if (dp > 0)
                    {
                        atten = (lights[curr_light].kc + lights[curr_light].kl*dist + lights[curr_light].kq*dist*dist);
                        i = 128*(nl*dist*atten);
                        r_sum += (lights[curr_light].c_diffuse.r * r_base * i)/(256*128);
                        g_sum += (lights[curr_light].c_diffuse.g * r_base * i)/(256*128);
                        b_sum += (lights[curr_light].c_diffuse.b * r_base * i)/(256*128);
                    }
                }
                else if (lights[curr_light].attr & LIGHTV1_ATTR_SPOTLIGHT2)
                {
                    VECTOR4D u, v, n, d, s;
                    VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_1], &u);
                    VECTOR4D_Build(&obj->vlist_trans[vindex_0], &obj->vlist_trans[vindex_2], &v);
                    VECTOR4D_Cross(&u, &v, &n);
                    nl = VECTOR4D_Length(&n);
                    dp = VECTOR4D_Dot(&n, &lights[curr_light].dir);
                    if (dp > 0)
                    {
                        VECTOR4D_Build(&lights[curr_light].pos, &obj->vlist_trans[vindex_0], &s);
                        dist = VECTOR4D_Length(&s);
                        float dpsl = VECTOR4D_Dot(&s, &lights[curr_light].dir)/dist;
                        if (dist > 0)
                        {
                            atten = (lights[curr_light].kc + lights[curr_light].kl*dist + lights[curr_light].kq*dist*dist);
                            float dpsl_exp = dpsl;
                            for (int e_index=1; e_index < (int)lights[curr_light].pf; e_index++)
                            {
                                dpsl_exp*=dpsl;
                            }
                            i = 128*dpsl_exp/(nl*atten);
                            r_sum += (lights[curr_light].c_diffuse.r * r_base * i)/(256*128);
                            g_sum += (lights[curr_light].c_diffuse.g * g_base * i)/(256*128);
                            b_sum += (lights[curr_light].c_diffuse.b * b_base * i)/(256*128);
                        }
                    }
                }
            }
            r_sum = MIN(255, r_sum);
            g_sum = MIN(255, g_sum);
            b_sum = MIN(255, b_sum);
            shared_color = RGB24BIT(0, r_sum, g_sum, b_sum);
            curr_poly->lcolor = shared_color;
            
        }
        else//POLY4DV1_ATTR_SHADE_MODE_CONSTANT
        {
            curr_poly->lcolor = curr_poly->color;
        }
    }

    return 1;
}
