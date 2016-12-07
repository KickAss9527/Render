#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED
#include "lxMath.h"

typedef unsigned short USHORT;
typedef unsigned short WORD;
typedef unsigned char UCHAR;
typedef unsigned char BYTE;
typedef unsigned int  QUAD;
typedef unsigned int  UINT;

typedef struct RGBAV1_TYP{
    union{
        int rgba;
        UCHAR rgba_M[4];
        struct{UCHAR a, b, g, r;};
    };
} RGBAV1, *RGBAV1_PTR;

typedef struct BITMAP_IMAGE_TYP{
    int state;
    int attr;
    int x,y;
    int width,height;
    int num_bytes;
    int bpp;
    UCHAR *buffer;
}BITMAP_IMAGE, *BITMAP_IMAGE_PTR;

typedef struct MATV1_TYP{
    int state;
    int id;
    char name[64];
    int attr;
    RGBAV1 color;
    float ka, kd, ks, power;
    RGBAV1 ra, rd, rs;
    char texture_file[80];
    BITMAP_IMAGE texture;
}MATV1, *MATV1_PTR;

typedef struct POLY4DV1_TYP{
    int state;
    int attr;
    int color;
    int lcolor;
    POINT4D_PTR vlist;
    POINT4D_PTR tvlist;
    int vert[3];
} POLY4DV1, *POLY4DV1_PTR;

typedef struct POLYF4DV1_TYP{
    int state;
    int attr;
    int color;
    int lcolor;
    POINT4D vlist[3];
    POINT4D tvlist[3];
    POLYF4DV1_TYP *next;
    POLYF4DV1_TYP *prev;
} POLYF4DV1, *POLYF4DV1_PTR;

typedef struct POLY4DV2_TYP{
    int state;
    int attr;
    int color;
    int lit_color[3];
    BITMAP_IMAGE_PTR texture;
    int mati;
    VERTEX4DTV1_PTR vlist;
    POINT2D_PTR tvlist;
    
    int vert[3];
    int text[3];
    float nlength;
}POLY4DV2, *POLY4DV2_PTR;

typedef struct POLYF4DV2_TYP{
    int state;
    int attr;
    int color;
    int lit_color[3];
    BITMAP_IMAGE_PTR texture;
    int mati;
    VERTEX4DTV1 vlist[3];
    VERTEX4DTV1 tvlist[3];
    
    float nlength;
    float avg_z;
    VECTOR4D normal;

    POLYF4DV2_TYP *next;
    POLYF4DV2_TYP *prev;
}POLYF4DV2, *POLYF4DV2_PTR;

#define OBJECT4DV1_STATE_ACTIVE      0x0001
#define OBJECT4DV1_STATE_VISIBLE     0x0002
#define OBJECT4DV1_STATE_CULLED      0x0004

#define POLY4DV1_ATTR_2SIDED    0x0001
#define POLY4DV1_ATTR_TRANSPARENT    0x0002
#define POLY4DV1_ATTR_8BITCOLOR    0x0004
#define POLY4DV1_ATTR_RGB16    0x0008
#define POLY4DV1_ATTR_RGB24    0x0010

#define POLY4DV1_ATTR_SHADE_MODE_PURE   0x0020
#define POLY4DV1_ATTR_SHADE_MODE_CONSTANT   0x0020
#define POLY4DV1_ATTR_SHADE_MODE_FLAT   0x0040
#define POLY4DV1_ATTR_SHADE_MODE_GOURAUD    0x0080
#define POLY4DV1_ATTR_SHADE_MODE_PHONG  0x0100

#define POLY4DV1_STATE_ACTIVE   0x001
#define POLY4DV1_STATE_CLIPPED  0x002
#define POLY4DV1_STATE_BACKFACE 0x004
#define OBJECT4DV1_MAX_VERTICES      640
#define OBJECT4DV1_MAX_POLYS       1280
#define RENDERLIST4DV1_MAX_POLYS   OBJECT4DV1_MAX_POLYS

#define DD_PIXEL_FORMAT8 8
#define DD_PIXEL_FORMAT555 15
#define DD_PIXEL_FORMAT565 16 
#define DD_PIXEL_FORMAT888 24 
#define DD_PIXEL_FORMATALPHA888 32
#define MATV1_ATTR_2SIDE    0x0001
#define MATV1_ATTR_TRANSPARENT    0x0002
#define MATV1_ATTR_8BITCOLOR    0x0004
#define MATV1_ATTR_RGB16    0x0008
#define MATV1_ATTR_RGB24    0x0010
#define MATV1_ATTR_SHADE_MODE_CONSTANT    0x020
#define MATV1_ATTR_SHADE_MODE_EMMISIVE    0x020
#define MATV1_ATTR_SHADE_MODE_FLAT    0x0040
#define MATV1_ATTR_SHADE_MODE_GOURAUD    0x0080
#define MATV1_ATTR_SHADE_MODE_FASTPHONG    0x0100
#define MATV1_ATTR_SHADE_MODE_TEXTURE    0x0200

#define MATV1_STATE_ACTIVE    0x0001
#define MATV1_MATERIALS    256



#define LIGHTV1_ATTR_AMBIENT    0x001
#define LIGHTV1_ATTR_INFINITE    0x002
#define LIGHTV1_ATTR_POINT    0x004
#define LIGHTV1_ATTR_SPOTLIGHT1    0x008
#define LIGHTV1_ATTR_SPOTLIGHT2    0x010
#define LIGHTV1_STATE_ON    1
#define LIGHTV1_STATE_OFF    0
#define MAX_LIGHTS    8
#define RENDERLIST4DV2_MAX_POLYS    32768


typedef struct LIGHTV1_TYP
{
    int state;
    int id;
    int attr;
    RGBAV1 c_ambient;
    RGBAV1 c_diffuse;
    RGBAV1 c_specular;
    POINT4D pos;
    VECTOR4D dir;
    float kc, kl, kq;
    float spot_inner;
    float spot_outer;
    float pf;
}LIGHTV1, *LIGHTV1_PTR;

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
    POLYF4DV1_PTR poly_ptrs[RENDERLIST4DV1_MAX_POLYS];
    POLYF4DV1 poly_data[RENDERLIST4DV1_MAX_POLYS];
    int num_polys;
} RENDERLIST4DV1, *RENDERLIST4DV1_PTR;

typedef struct RENDERLIST4DV2_TYP{
    int state;
    int attr;
    POLYF4DV2_PTR poly_ptrs[RENDERLIST4DV2_MAX_POLYS];
    POLYF4DV2 poly_data[RENDERLIST4DV2_MAX_POLYS];
    int num_polys;
} RENDERLIST4DV2, *RENDERLIST4DV2_PTR;

typedef struct OBJECT4DV2_TYP{
    int id;
    char name[64];
    int state;
    int attr;
    int mati;
    float *avg_radius;
    float *max_radius;
    POINT4D world_pos;
    VECTOR4D dir;
    VECTOR4D ux, uy, uz;
    int num_vertices;
    int num_frames;
    int total_vertices;
    int curr_frame;
    VERTEX4DTV1_PTR vlist_local;
    VERTEX4DTV1_PTR vlist_trans;
    VERTEX4DTV1_PTR head_vlist_local;
    VERTEX4DTV1_PTR head_vlist_trans;
    POINT2D_PTR tlist;
    BITMAP_IMAGE_PTR texture;
    POLY4DV2_PTR plist;
    int num_polys;
    int Set_Frame(int frame);
}OBJECT4DV2, *OBJECT4DV2_PTR;

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
#define PLX_SHADE_MODE_MASK     0x6000000
#define PLX_2SIDED_MASK 0x1000
#define PLX_COLOR_MASK  0x0fff
#define PLX_COLOR_MODE_RGB_FLAG 0x8000000
#define PLX_COLOR_MODE_INDEXED_FLAG 0x0000
#define PLX_2SIDED_FLAG     0x1000
#define PLX_1SIDE_FLAG      0x0000
#define PLX_SHADE_MODE_PURE_FLAG    0x0000000
#define PLX_SHADE_MODE_FLAT_FLAG    0x2000000
#define PLX_SHADE_MODE_GOURAUD_FLAG    0x4000000
#define PLX_SHADE_MODE_PHONG_FLAG    0x6000000
#define RGB888FROM24BIT(RGB,r,g,b){ *r = ( ((RGB) >> 16) & 0xff); *g = (((RGB) >> 8) & 0xff); *b = ( (RGB) & 0xff); }
#define _RGB565FROM16BIT(RGB, r,g,b) { *r = ( ((RGB) >> 11) & 0x1f); *g = (((RGB) >> 5) & 0x3f); *b = ((RGB) & 0x1f); }
#define _RGB555FROM16BIT(RGB, r,g,b) { *r = ( ((RGB) >> 10) & 0x1f); *g = (((RGB) >> 5) & 0x1f); *b = ( (RGB) & 0x1f); }

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
#define POLY4DV2_ATTR_SHADE_MODE_EMISSIVE  0x0020 // (alias)
#define POLY4DV2_ATTR_SHADE_MODE_FLAT    0x0040
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

#define SORT_POLYLIST_AVGZ  0
#define SORT_POLYLIST_NEARZ  1
#define SORT_POLYLIST_FARZ  2

#define POLY4DV2_ATTR_ENABLE_MATERIAL 0x0800
#define POLY4DV2_ATTR_DISABLE_MATERIAL 0x1000

#define VERTEXT_FLAGS_OVERRIDE_CONSTANT 0x1000
#define VERTEXT_FLAGS_OVERRIDE_EMISSIVE 0x1000
#define VERTEXT_FLAGS_OVERRIDE_PURE 0x1000
#define VERTEXT_FLAGS_OVERRIDE_FLAT 0x2000
#define VERTEXT_FLAGS_OVERRIDE_GOURAUD 0x4000
#define VERTEXT_FLAGS_OVERRIDE_TEXTURE 0x8000

#define VERTEXT_FLAGS_OVERRIDE_INVERT_TEXTURE_U 0x0080
#define VERTEXT_FLAGS_OVERRIDE_INVERT_TEXTURE_V 0x0100
#define VERTEXT_FLAGS_OVERRIDE_INVERT_SWAP_UV 0x0800

#define OBJECT4DV2_MAX_VERTICES 1024
#define OBJECT4DV2_MAX_POLYS    2048
#define OBJECT4DV2_STATE_NULL 0x0000
#define OBJECT4DV2_STATE_ACTIVE 0x0001
#define OBJECT4DV2_STATE_VISIBLE 0x0002
#define OBJECT4DV2_STATE_CULLED 0x0004

#define OBJECT4DV2_ATTR_SINGLE_FRAME 0x0001
#define OBJECT4DV2_ATTR_MULTI_FRAME 0x0002
#define OBJECT4DV2_ATTR_TEXTURES 0x0004


#define VERTEX4DTV1_ATTR_NULL 0x0000
#define VERTEX4DTV1_ATTR_POINT 0x0001
#define VERTEX4DTV1_ATTR_NORMAL 0x0002
#define VERTEX4DTV1_ATTR_TEXTURE 0x0004


int Load_OBJECT4DV1_PLG(OBJECT4DV1_PTR obj, char *filename, VECTOR4D_PTR scale, VECTOR4D_PTR pos, VECTOR4D_PTR rot);
int Load_OBEJCT4DV2_PLG(OBJECT4DV2_PTR obj,
                        char *filename,
                        VECTOR4D_PTR scale,
                        VECTOR4D_PTR pos,
                        VECTOR4D_PTR rot,
                        int vertex_flags);

void RESET_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list);
void Reset_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list);

int Insert_OBJECT4DV1_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list,
                                     OBJECT4DV1_PTR obj,
                                     int insert_local,
                                     int lighting_on);
int Insert_OBJECT4DV2_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list,
                                     OBJECT4DV2_PTR obj,
                                     int insert_local);

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
void Remove_Backfaces_OBJECT4DV2(OBJECT4DV2_PTR obj, CAM4DV1_PTR cam);
void Remove_Backfaces_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam);
void Remove_Backfaces_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, CAM4DV1_PTR cam);

void Build_CAM4DV1_Matrix_Euler(CAM4DV1_PTR cam, int cam_rot_seq);

void Model_To_World_OBJECT4DV1(OBJECT4DV1_PTR obj, int coord_select = TRANSFORM_LOCAL_TO_TRANS);
void Model_To_World_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, POINT4D_PTR world_pos, int coord_select = TRANSFORM_LOCAL_TO_TRANS);
void Model_To_World_OBJECT4DV2(OBJECT4DV2_PTR obj, int coord_select, int all_frames);

void World_To_Camera_OBJECT4DV1(CAM4DV1_PTR cam, OBJECT4DV1_PTR obj);
void World_To_Camera_OBJECT4DV2(OBJECT4DV2_PTR obj, CAM4DV1_PTR cam);
void World_To_Camera_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam);
void World_To_Camera_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, CAM4DV1_PTR cam);

void Camera_To_Perspective_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam);
void Camera_To_Perspective_OBJECT4DV2(OBJECT4DV2_PTR obj, CAM4DV1_PTR cam);
void Camera_To_Perspective_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam);
void Camera_To_Perspective_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, CAM4DV1_PTR cam);

void Perspective_To_Screen_OBJECT4DV1(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam);
void Perspective_To_Screen_OBJECT4DV2(OBJECT4DV2_PTR obj, CAM4DV1_PTR cam);
void Perspective_To_Screen_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, CAM4DV1_PTR cam);
void Perspective_To_Screen_RENDERLIST4DV2(RENDERLIST4DV2_PTR rend_list, CAM4DV1_PTR cam);

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
                       float _pf);
int Light_OBJECT4DV1_World16(OBJECT4DV1_PTR obj,
                             CAM4DV1_PTR cam,
                             LIGHTV1_PTR lights,
                             int max_lights);
int Light_RENDERLIST4DV1_World16(RENDERLIST4DV1_PTR rend_list,
                                 CAM4DV1_PTR cam,
                                 LIGHTV1_PTR lights,
                                 int max_lights);
LIGHTV1_PTR GetLightList(void);
int Reset_Lights_LIGHTV1(void);
void Sort_RENDERLIST4DV1(RENDERLIST4DV1_PTR rend_list, int sort_method);
#endif // ENGINE_H_INCLUDED
