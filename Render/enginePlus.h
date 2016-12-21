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

void LoadMyBitmap(const char *strFile, BITMAP_IMAGE_PTR tex);
void GenerateTerrain(OBJECT4DV2_PTR obj);
void GeneratePool(OBJECT4DV2_PTR obj);
void UpdatePool(float delta, OBJECT4DV2_PTR obj);
#endif /* enginePlus_h */
