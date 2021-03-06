//
//  enginePlus.h
//  Render
//
//  Created by XuMingQi on 2016/12/19.
//  Copyright © 2016年 XuMingQi. All rights reserved.
//

#ifndef enginePlus_h
#define enginePlus_h

#include <stdio.h>
#include <math.h>
#include "engine.h"

BITMAP_IMAGE_PTR LoadMyBitmap(const char *strFile);
void GenerateTerrain(OBJECT4DV2_PTR obj);
void GeneratePool(OBJECT4DV2_PTR obj);
void UpdatePool(float delta, OBJECT4DV2_PTR obj);
#endif /* enginePlus_h */
