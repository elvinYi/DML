#ifndef __IMG_ROTATERT_
#define __IMG_ROTATERT_

#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTHBYTES(bits) ((DWORD)(((bits)+31) & (~31)) / 8)

enum RotateAngle
{
	Angle_0,
	Angle_90,
	Angle_180,
	Angle_270,
};

void CopyRow(const BYTE* src, BYTE* dst, int count);
void CopyPlane(const BYTE* src_y, int src_stride_y, BYTE* dst_y, int dst_stride_y, int width, int height);
void RGBMirrorRow(const BYTE* src, BYTE* dst, int width);
void RGBTranspose(const BYTE* src, int src_stride, BYTE* dst, int dst_stride, int width, int height);
void ScaleRGBRowDownEven(const BYTE* src_rgb, ptrdiff_t src_stride, int src_stepx, BYTE* dst_rgb, int dst_width);

int  RGBCopy(const BYTE* src_rgb, int src_stride_rgb, BYTE* dst_rgb, int dst_stride_rgb, int width, int height);
int  RGBRotate90(const BYTE* src_rgb, int src_stride_rgb, BYTE* dst_rgb, int dst_stride_rgb, int width, int height);
int  RGBRotate180(const BYTE* src_rgb, int src_stride_rgb, BYTE* dst_rgb, int dst_stride_rgb, int width, int height);
int  RGBRotate270(const BYTE* src_rgb, int src_stride_rgb, BYTE* dst_rgb, int dst_stride_rgb, int width, int height);

// src_stride_rgb = frame.width * channels, 
// dst_stride_rgb = newframe.width * channels,
int RGBRotate(const BYTE* src_rgb, BYTE* dst_rgb, int src_width, int src_height, int channels, RotateAngle mode);

#endif
