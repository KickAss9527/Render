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
#include "engine.h"
#include <stdlib.h>

using namespace std;

typedef short BITE2;
typedef int BITE4;

typedef struct  tagBITMAPFILEHEADER{
    //WORD bfType;//文件类型，必须是0x424D，即字符“BM”
    BITE4 bfSize;//文件大小
    BITE2 bfReserved1;//保留字
    BITE2 bfReserved2;//保留字
    BITE4 bfOffBits;//从文件头到实际位图数据的偏移字节数
}BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{
    BITE4 biSize;//信息头大小
    BITE4 biWidth;//图像宽度
    BITE4 biHeight;//图像高度
    BITE2 biPlanes;//位平面数，必须为1
    BITE2 biBitCount;//每像素位数
    BITE4  biCompression; //压缩类型
    BITE4  biSizeImage; //压缩图像大小字节数
    BITE4  biXPelsPerMeter; //水平分辨率
    BITE4  biYPelsPerMeter; //垂直分辨率
    BITE4  biClrUsed; //位图实际用到的色彩数
    BITE4  biClrImportant; //本位图中重要的色彩数
}BITMAPINFOHEADER; //位图信息头定义

typedef struct tagRGBQUAD{
    BYTE rgbBlue; //该颜色的蓝色分量
    BYTE rgbGreen; //该颜色的绿色分量
    BYTE rgbRed; //该颜色的红色分量
    BYTE rgbReserved; //保留值
}RGBQUAD;//调色板定义

//像素信息
typedef struct tagIMAGEDATA
{
    BYTE red;
    BYTE green;
    BYTE blue;
}IMAGEDATA;

BITMAPFILEHEADER strHead;
RGBQUAD strPla[256];//256色调色板
BITMAPINFOHEADER strInfo;
IMAGEDATA imagedata[256][256];//存储像素信息

//显示位图文件头信息
void showBmpHead(BITMAPFILEHEADER pBmpHead){
    cout<<"位图文件头:"<<endl;
    //cout<<"bfType value is "<<hex<<pBmpHead.bfType<<endl;
    cout<<"文件大小:"<<pBmpHead.bfSize<<endl;
    //printf("文件大小:%d\n",pBmpHead.bfSize);
    cout<<"保留字_1:"<<pBmpHead.bfReserved1<<endl;
    cout<<"保留字_2:"<<pBmpHead.bfReserved2<<endl;
    cout<<"实际位图数据的偏移字节数:"<<pBmpHead.bfOffBits<<endl<<endl;
}

void showBmpInforHead(tagBITMAPINFOHEADER pBmpInforHead){
    cout<<"位图信息头:"<<endl;
    cout<<"结构体的长度:"<<pBmpInforHead.biSize<<endl;
    cout<<"位图宽:"<<pBmpInforHead.biWidth<<endl;
    cout<<"位图高:"<<pBmpInforHead.biHeight<<endl;
    cout<<"biPlanes平面数:"<<pBmpInforHead.biPlanes<<endl;
    cout<<"biBitCount采用颜色位数:"<<pBmpInforHead.biBitCount<<endl;
    cout<<"压缩方式:"<<pBmpInforHead.biCompression<<endl;
    cout<<"biSizeImage实际位图数据占用的字节数:"<<pBmpInforHead.biSizeImage<<endl;
    cout<<"X方向分辨率:"<<pBmpInforHead.biXPelsPerMeter<<endl;
    cout<<"Y方向分辨率:"<<pBmpInforHead.biYPelsPerMeter<<endl;
    cout<<"使用的颜色数:"<<pBmpInforHead.biClrUsed<<endl;
    cout<<"重要颜色数:"<<pBmpInforHead.biClrImportant<<endl;
}

BITMAP_IMAGE_PTR LoadMyBitmap(const char *strFile)
{
    if(isBitmapLoaded(strFile))
        return getBitmap(strFile);
    FILE *fpi,*fpw;
    fpi=fopen(getFilePath(strFile).c_str(),"rb");
    if(fpi!=NULL){
        //先读取文件类型
        WORD bfType;
        fread(&bfType,1,sizeof(WORD),fpi);
        if(0x4d42!=bfType){
            cout<<"the file is not a bmp file!"<<endl;
            return nullptr;
        }
        //读取bmp文件的文件头和信息头
        fread(&strHead,1,sizeof(tagBITMAPFILEHEADER),fpi);
        showBmpHead(strHead);//显示文件头
        fread(&strInfo,1,sizeof(tagBITMAPINFOHEADER),fpi);
        showBmpInforHead(strInfo);//显示文件信息头

        BITMAP_IMAGE_PTR tex = getNewBitmap();
        tex->height = strInfo.biHeight;
        tex->width = strInfo.biWidth;
        tex->bitCnt = strInfo.biBitCount;
        tex->name = strFile;
        if (strInfo.biBitCount == 8)
        {
            //读取调色板
            for(int nCounti=0;nCounti<strInfo.biClrUsed;nCounti++){
                //存储的时候，一般去掉保留字rgbReserved
                fread((char *)&strPla[nCounti].rgbBlue,1,sizeof(BYTE),fpi);
                fread((char *)&strPla[nCounti].rgbGreen,1,sizeof(BYTE),fpi);
                fread((char *)&strPla[nCounti].rgbRed,1,sizeof(BYTE),fpi);
                fread((char *)&strPla[nCounti].rgbReserved,1,sizeof(BYTE),fpi);
            }
            int length = tex->width*tex->height;
            tex->buffer = (UCHAR*)malloc(length*3);
            fread(tex->buffer,length, 1,fpi);
            for (int i=length-1; i>=0; i--)
            {
                int idx = tex->buffer[i];
                //printf("\ni : %d", idx);
                tagRGBQUAD rgba = strPla[idx];
                tex->buffer[i*3] = rgba.rgbBlue;
                tex->buffer[i*3 + 1] = rgba.rgbGreen;
                tex->buffer[i*3 + 2] = rgba.rgbRed;
            }
            tex->bitCnt = 8*3;
        }
        else
        {
            int length = tex->width*tex->height*strInfo.biBitCount/8;
            tex->buffer = (UCHAR*)malloc(length);
            fread(tex->buffer,length, 1,fpi);

        }

        //读出图片的像素数据
//        memset(imagedata,0,sizeof(IMAGEDATA) * 256 * 256);
//        //fseek(fpi,54,SEEK_SET);
//        fread(imagedata,sizeof(struct tagIMAGEDATA) * strInfo.biWidth,strInfo.biHeight,fpi);
        //for(int i = 0;i < strInfo.biWidth;++i)
//        for(int i = 0;i < 1;++i)//只输出第一行数据
//        {
//            for(int j = 0;j < strInfo.biHeight; ++j){
//                printf("%d  ", imagedata[i][j].green);
//                // cout<<imagedata[0][j].green+ " ";
//                if((i * strInfo.biHeight+j+1) % 5 == 0)
//                    cout<<endl;
//            }
//        }

        fclose(fpi);
        return tex;
    }
    else{
        cout<<"file open error!"<<endl;
        return nullptr;
    }


}

static int TerrainHeightMax = 60;
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

    BITMAP_IMAGE texture = *getNewBitmap();
#ifdef __APPLE__
        LoadMyBitmap("/MyFiles/Work/GitProject/Render/Render/earthheightmap03.bmp");
#else
        LoadMyBitmap("C:\\Users\\Administrator\\Documents\\GitHub\\Render\\Render\\earthheightmap03.bmp");
#endif

    int deltaX = texture.width*0.5*TerrainSizeScale;
    int deltaZ = texture.height*0.5*TerrainSizeScale;

    float tmpX, tmpZ;
    tmpX = MIN(texture.width, 100);
    tmpZ = MIN(texture.height, 100);

    obj->num_polys = tmpX*tmpZ*2;
    obj->num_vertices = (tmpX+1)*(tmpZ+1);
    Init_OBJECT4DV2(obj, obj->num_vertices, obj->num_polys, obj->num_frames);
    POINT3D *coordArr = new POINT3D[obj->num_vertices];
    for (int z=0; z<=tmpZ; z++)
    {
        for (int x=0; x<=tmpX; x++)
        {
            POINT2D p = {x*texture.width/tmpX, z*texture.height/tmpZ};
            POINT2D texP = {p.x, texture.height - p.y};
            RGBAV1 c = getTextureColor(&texture, &texP);
            POINT3D coord;
            coord.x = p.x*TerrainSizeScale - deltaX;
            coord.z = p.y*TerrainSizeScale - deltaZ;
            int Gray = (c.r*30 + c.g*59 + c.r*11 + 50) / 100;
            coord.y = TerrainHeightMax*Gray/(255*1.0);
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
        obj->plist[polyIdx].vert[1] = v1;
        obj->plist[polyIdx].vert[2] = v2;

        obj->plist[polyIdx+1].vert[0] = v0;
        obj->plist[polyIdx+1].vert[1] = v2;
        obj->plist[polyIdx+1].vert[2] = v3;

        for (int i=polyIdx; i<=polyIdx+1; i++)
        {
            obj->plist[i].vlist = obj->vlist_local;
            SET_BIT(obj->plist[i].attr, POLY4DV2_ATTR_DISABLE_MATERIAL);
            obj->plist[i].state = POLY4DV2_STATE_ACTIVE;
            obj->plist[i].vlist = obj->vlist_local;
            obj->plist[i].tlist = obj->tlist;
            SET_BIT(obj->plist[i].attr, POLY4DV2_ATTR_RGB24);
            int red = 200;
            int green = 200;
            int blue = 200;
            obj->plist[i].color = RGB24BIT(0,red, green, blue);
            SET_BIT(obj->plist[i].attr, POLY4DV2_ATTR_SHADE_MODE_FLAT);
        }
    }

    Compute_OBJECT4DV2_Poly_Normals(obj);
    Compute_OBJECT4DV2_Vertex_Normals(obj);

}

int gridCnt = 50;
int poolMaxHeight = 10;
void GeneratePool(OBJECT4DV2_PTR obj)
{
    int gridLength = 2;
    memset(obj, 0, sizeof(OBJECT4DV2));
    obj->state = OBJECT4DV2_STATE_ACTIVE | OBJECT4DV2_STATE_VISIBLE;
    obj->world_pos.x = 0;
    obj->world_pos.y = 0;
    obj->world_pos.z = gridCnt*gridLength*0.5;
    obj->world_pos.w = 1;
    obj->num_frames = 1;
    obj->curr_frame = 0;
    obj->attr = OBJECT4DV2_ATTR_SINGLE_FRAME;

    obj->num_polys = gridCnt*gridCnt*2;
    obj->num_vertices = (gridCnt+1)*(gridCnt+1);
    Init_OBJECT4DV2(obj, obj->num_vertices, obj->num_polys, obj->num_frames);
    int halfLength = gridLength*gridCnt*.5;

    for (int vertex=0; vertex<obj->num_vertices; vertex++)
    {
        int xC = vertex%(gridCnt+1);
        int zC = vertex/(gridCnt+1);
        int x = gridLength*xC - halfLength;
        int z = halfLength - (gridLength*zC);
        obj->vlist_local[vertex].x = x;
        obj->vlist_local[vertex].z = z;
        float distance = pow(xC - gridCnt*0.5, 2)+pow(zC - gridCnt*0.5, 2);
        distance = sqrtf(distance);
//        distance/=3;
        obj->vlist_local[vertex].y = 0.3*poolMaxHeight*cos(distance);
        obj->vlist_local[vertex].w = 1;

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
        int y = poly/gridCnt;
        v0 = poly + y;
        v1 = v0 + 1;
        v2 = v1 + gridCnt + 1;
        v3 = v2 - 1;

        int polyIdx = poly*2;
        obj->plist[polyIdx].vert[0] = v0;
        obj->plist[polyIdx].vert[1] = v1;
        obj->plist[polyIdx].vert[2] = v2;

        obj->plist[polyIdx+1].vert[0] = v0;
        obj->plist[polyIdx+1].vert[1] = v2;
        obj->plist[polyIdx+1].vert[2] = v3;

        for (int i=polyIdx; i<=polyIdx+1; i++)
        {
            obj->plist[i].vlist = obj->vlist_local;
            SET_BIT(obj->plist[i].attr, POLY4DV2_ATTR_DISABLE_MATERIAL);
            obj->plist[i].state = POLY4DV2_STATE_ACTIVE;
            obj->plist[i].vlist = obj->vlist_local;
            obj->plist[i].tlist = obj->tlist;
            SET_BIT(obj->plist[i].attr, POLY4DV2_ATTR_RGB24);
            int red = 200;
            int green = 200;
            int blue = 200;
            obj->plist[i].color = RGB24BIT(0,red, green, blue);

            SET_BIT(obj->plist[i].attr, POLY4DV2_ATTR_SHADE_MODE_GOURAUD);
        }

    }

    Compute_OBJECT4DV2_Poly_Normals(obj);
    Compute_OBJECT4DV2_Vertex_Normals(obj);

}

void UpdatePool(float delta, OBJECT4DV2_PTR obj)
{
    for (int vertex=0; vertex<obj->num_vertices; vertex++)
    {
        int xC = vertex%(gridCnt+1);
        int zC = vertex/(gridCnt+1);
        float distance = pow(xC - gridCnt*0.5, 2)+pow(zC - gridCnt*0.5, 2);
        distance = sqrtf(distance);
        //        distance/=3;
        distance += delta;
        distance *= 0.7;
        obj->vlist_local[vertex].y = 0.3*poolMaxHeight*cos(distance);
        obj->vlist_local[vertex].w = 1;

        SET_BIT(obj->vlist_local[vertex].attr, VERTEX4DTV1_ATTR_POINT);
    }
    Compute_OBJECT4DV2_Radius(obj);
    Compute_OBJECT4DV2_Poly_Normals(obj);
    Compute_OBJECT4DV2_Vertex_Normals(obj);
}



