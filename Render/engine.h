#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED
#include "lxMath.h"
typedef struct POLY4DV1_TYP{
    int state;
    int attr;
    int color;
    POINT4D_PTR vlist;
    POINT4D_PTR tvlist;
    int vert[3];
} POLY4DV1, *POLY4DV1_PTR;

typedef struct POLYF4DV1_TYP{
    int state;
    int attr;
    int color;
    POINT4D vlist[3];
    POINT4D tvlist[3];
    POLY4DV1_TYP *next;
    POLY4DV1_TYP *prev;
} POLYF4DV1, *POLYF4DV1_PTR;

#define OBJECT4DV1_STATE_ACTIVE      0x0001
#define OBJECT4DV1_STATE_VISIBLE     0x0002
#define OBJECT4DV1_STATE_CULLED      0x0004

#define POLY4DV1_ATTR_2SIDED    0x0001
#define POLY4DV1_ATTR_TRANSPARENT    0x0002
#define POLY4DV1_ATTR_8BITCOLOR    0x0004
#define POLY4DV1_ATTR_RGB16    0x0008
#define POLY4DV1_ATTR_RGB24    0x0010

#define POLY4DV1_ATTR_SHADE_MODE_PURE   0x0020
#define POLY4DV1_ATTR_SHADE_MODE_FLAT   0x0040
#define POLY4DV1_ATTR_SHADE_MODE_GOURAUD    0x0080
#define POLY4DV1_ATTR_SHADE_MODE_PHONG  0x0100

#define POLY4DV1_STATE_ACTIVE   0x001
#define POLY4DV1_STATE_CLIPPED  0x002
#define POLY4DV1_STATE_BACKFACE 0x004
#define OBJECT4DV1_MAX_VERTICES      64
#define OBJECT4DV1_MAX_POLYS       128
#define RENDERLIST4DV1_MAX_POLYS   OBJECT4DV1_MAX_POLYS

typedef struct OBJECT4DV1_TYP{
    int id;
    char name[64];
    int state;
    int attr;
    float avg_radius;
    float max_radius;
    POINT4D world_pos;
    VECTOR4D dir;
    VECTOR4D ux, uy, uz;
    int num_vertices;
    POINT4D vlist_local[OBJECT4DV1_MAX_VERTICES];
    POINT4D vlist_trans[OBJECT4DV1_MAX_VERTICES];
    int num_polys;
    POLY4DV1 plist[OBJECT4DV1_MAX_POLYS];
} OBJECT4DV1, *OBJECT4DV1_PTR;

typedef struct RENDERLIST4DV1_TYP{
    int state;
    int attr;
    POLY4DV1_PTR poly_ptrs[RENDERLIST4DV1_MAX_POLYS];
    POLY4DV1 poly_data[RENDERLIST4DV1_MAX_POLYS];
    int num_polys;
} RENDERLIST4DV1, *RENDERLIST4DV1_PTR;

typedef struct CAM4DV1_TYP{
    int state;
    int attr;
    POINT4D pos;
    VECTOR4D dir;
    VECTOR4D u;
    VECTOR4D v;
    VECTOR4D n;
    POINT4D target;
    float view_dist;
    float view_dist_h;
    float view_dist_v;
    float fov;
    float near_clip_z;
    float far_clip_z;
    PLANE3D rt_clip_plane;
    PLANE3D lt_clip_plane;
    PLANE3D tp_clip_plane;
    PLANE3D bt_clip_plane;
    float viewplane_width;
    float viewplane_height;
    float viewport_width;
    float viewport_height;
    float viewport_center_x;
    float viewport_center_y;
    float aspect_ratio;
    MATRIX4X4 mcam;
    MATRIX4X4 mper;
    MATRIX4X4 mscr;
} CAM4DV1, *CAM4DV1_PTR;

#define PLX_RGB_MASK    0x8000
#define PLX_SHADE_MODE_MASK     0x6000
#define PLX_2SIDED_MASK 0x1000
#define PLX_COLOR_MASK  0x0fff
#define PLX_COLOR_MODE_RGB_FLAG 0x8000
#define PLX_COLOR_MODE_INDEXED_FLAG 0x0000
#define PLX_2SIDED_FLAG     0x1000
#define PLX_1SIDE_FLAG      0x0000
#define PLX_SHADE_MODE_PURE_FLAG    0x0000
#define PLX_SHADE_MODE_FLAT_FLAG    0x2000
#define PLX_SHADE_MODE_GOURAUD_FLAG    0x4000
#define PLX_SHADE_MODE_PHONG_FLAG    0x6000
#define RGB16BIT555(r,g,b) ((b & 31) + ((g & 31) << 5) + ((r & 31) << 10))
// this builds a 16 bit color value in 5.6.5 format (green dominate mode)
#define RGB16BIT565(r,g,b) ((b & 31) + ((g & 63) << 5) + ((r & 31) << 11))// this builds a 24 bit color value in 8.8.8 format
#define RGB16BIT(r,g,b) RGB16BIT565(r,g,b)
#define RGB24BIT(a,r,g,b) ((b) + ((g) << 8) + ((r) << 16) )
// this builds a 32 bit color value in A.8.8.8 format (8-bit alpha mode)
#define RGB32BIT(a,r,g,b) ((b) + ((g) << 8) + ((r) << 16) + ((a) << 24))
// bit manipulation macros
#define SET_BIT(word,bit_flag)  ((word)=((word) | (bit_flag)))
#define RESET_BIT(word,bit_flag) ((word)=((word) & (~bit_flag)))
// initializes a direct draw struct,
// basically zeros it and sets the dwSize field
#define DDRAW_INIT_STRUCT(ddstruct) {memset(&ddstruct,0,sizeof(ddstruct));ddstruct.dwSize=sizeof(ddstruct); }

#define POLY4DV2_ATTR_2SIDED        0x0001
#define POLY4DV2_ATTR_TRANSPARENT      0x0002
#define POLY4DV2_ATTR_8BITCOLOR       0x0004
#define POLY4DV2_ATTR_RGB16         0x0008
#define POLY4DV2_ATTR_RGB24         0x0010
#define POLY4DV2_ATTR_SHADE_MODE_PURE    0x0020
#define POLY4DV2_ATTR_SHADE_MODE_CONSTANT  0x0020 // (alias)
#define POLY4DV2_ATTR_SHADE_MODE_EMISSIVE  0x0020 // (alias)#define POLY4DV2_ATTR_SHADE_MODE_FLAT    0x0040
#define POLY4DV2_ATTR_SHADE_MODE_GOURAUD  0x0080
#define POLY4DV2_ATTR_SHADE_MODE_PHONG   0x0100
#define POLY4DV2_ATTR_SHADE_MODE_FASTPHONG 0x0100 // (alias)
#define POLY4DV2_ATTR_SHADE_MODE_TEXTURE  0x0200
// new
#define POLY4DV2_ATTR_ENABLE_MATERIAL 0x0800 // use material for lighting
#define POLY4DV2_ATTR_DISABLE_MATERIAL 0x1000 // use basic color only
// for lighting (emulate ver 1.0)
// states of polygons and faces
#define POLY4DV2_STATE_NULL        0x0000
#define POLY4DV2_STATE_ACTIVE       0x0001
#define POLY4DV2_STATE_CLIPPED      0x0002
#define POLY4DV2_STATE_BACKFACE      0x0004
#define POLY4DV2_STATE_LIT        0x0008

#define TRANSFORM_LOCAL_ONLY    0
#define TRANSFORM_TRANS_ONLY    1
#define TRANSFORM_LOCAL_TO_TRANS    2

#define CAM_ROT_SEQ_XYZ 0
#define CAM_ROT_SEQ_YXZ 1
#define CAM_ROT_SEQ_XZY 2
#define CAM_ROT_SEQ_YZX 3
#define CAM_ROT_SEQ_ZYX 4
#define CAM_ROT_SEQ_ZXY 5

#define UVN_MODE_SIMPLE            0
#define UVN_MODE_SPHERICAL         1
#define CULL_OBJECT_X_PLANE 0x0001
#define CULL_OBJECT_Y_PLANE 0x0002
#define CULL_OBJECT_Z_PLANE 0x0004
#define CULL_OBJECT_XYZ_PLANES (CULL_OBJECT_X_PLANE | CULL_OBJECT_Y_PLANE | CULL_OBJECT_Z_PLANE)
int Load_OBJECT4DV1_PLG(OBJECT4DV1_PTR obj, char *filename, VECTOR4D_PTR scale, VECTOR4D_PTR pos, VECTOR4D_PTR rot);
void RESET_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list);
void Init_CAM4DV1(CAM4DV1_PTR cam,
                  //int cam_attr,
                  POINT4D_PTR cam_pos,
                  VECTOR4D_PTR cam_dir,
                 // POINT4D_PTR cam_target,
                  float near_clip_z,
                  float far_clip_z,
                  float fov,
                  float viewport_width,
                  float viewport_height);
void Remove_Backfaces_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam);
void Build_CAM4DV1_Matrix_Euler(CAM4DV1_PTR cam, int cam_rot_seq);
void Model_To_World_OBJECT4DV1(OBJECT4DV1_PTR obj, int coord_select = TRANSFORM_LOCAL_TO_TRANS);
void World_To_Camera_OBJECT4DV1(CAM4DV1_PTR cam, OBJECT4DV1_PTR obj);
void Camera_To_Perspective_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam);
void Perspective_To_Screen_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam);
#endif // ENGINE_H_INCLUDED
