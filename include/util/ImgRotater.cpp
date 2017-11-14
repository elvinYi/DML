#include "ImgRotater.h"

void CopyRow(const BYTE* src, BYTE* dst, int count)
{
	memcpy(dst, src, count);
}

void CopyPlane(const BYTE* src_y, int src_stride_y, BYTE* dst_y, int dst_stride_y, int width, int height)
{
	int y;

	// Coalesce rows.
	if (src_stride_y == width &&
		dst_stride_y == width) {
		width *= height;
		height = 1;
		src_stride_y = dst_stride_y = 0;
	}

	if (src_y == dst_y && src_stride_y == dst_stride_y) {
		return;
	}

	// Copy plane
	for (y = 0; y < height; ++y) {
		CopyRow(src_y, dst_y, width);
		src_y += src_stride_y;
		dst_y += dst_stride_y;
	}
}
int RGBCopy(const BYTE* src_rgb, int src_stride_rgb, BYTE* dst_rgb, int dst_stride_rgb, int width, int height)
{
		if (!src_rgb || !dst_rgb ||
			width <= 0 || height == 0) {
			return -1;
		}
		// Negative height means invert the image.
		if (height < 0) {
			height = -height;
			src_rgb = src_rgb + (height - 1) * src_stride_rgb;
			src_stride_rgb = -src_stride_rgb;
		}

		CopyPlane(src_rgb, src_stride_rgb, dst_rgb, dst_stride_rgb,
			width * 3, height);
		return 0;
}

int RGBRotate(const BYTE* src_rgb, BYTE* dst_rgb, int src_width, int src_height, int channels, RotateAngle mode)
{
	int src_stride_rgb, dst_stride_rgb;

	if (!src_rgb || src_width <= 0 || src_height == 0 || !dst_rgb) {
		return -1;
	}

	src_stride_rgb = src_width * channels;
	
	// Negative height means invert the image.
	if (src_height < 0) {
		src_height = -src_height;
		src_rgb = src_rgb + (src_height - 1) * src_stride_rgb;
		src_stride_rgb = -src_stride_rgb;
	}

	switch (mode) {
	case Angle_0:
		dst_stride_rgb = src_stride_rgb;
		// copy frame
		return RGBCopy(src_rgb, src_stride_rgb, dst_rgb, dst_stride_rgb, src_width, src_height);
	case Angle_270:
		dst_stride_rgb = src_height * channels;
		RGBRotate90(src_rgb, src_stride_rgb, dst_rgb, dst_stride_rgb, src_width, src_height);   // 90 is equal to 270
		return 0;
	case Angle_180:
		dst_stride_rgb = src_stride_rgb;
		RGBRotate180(src_rgb, src_stride_rgb, dst_rgb, dst_stride_rgb, src_width, src_height);
		return 0;
	case Angle_90:
		dst_stride_rgb = src_height * channels;
		RGBRotate270(src_rgb, src_stride_rgb, dst_rgb, dst_stride_rgb, src_width, src_height);  // 270 is equal to 90
		return 0;
	default:
		break;
	}
	return -1;
}

void RGBTranspose(const BYTE* src, int src_stride, BYTE* dst, int dst_stride, int width, int height)
{
	int i;
	int src_pixel_step = src_stride/3;

	for (i = 0; i < width; ++i) {  // column of source to row of dest.
		ScaleRGBRowDownEven(src, 0, src_pixel_step, dst, height);
		dst += dst_stride;
		src += 3;
	}

}

void ScaleRGBRowDownEven(const BYTE* src_rgb, ptrdiff_t src_stride,int src_stepx,BYTE* dst_rgb, int dst_width)
{
	int x;
	BYTE* src = (BYTE*)src_rgb;
	BYTE* dst = (BYTE*)dst_rgb;

	for (x = 0; x < dst_width; x ++) {
		dst[0] = src[0];
		dst[1] = src[1];
		dst[2] = src[2];

		src += src_stepx*3;
		dst += 3;
	}
}

int RGBRotate270(const BYTE* src_rgb, int src_stride_rgb, BYTE* dst_rgb, int dst_stride_rgb, int width, int height)
{
	src_rgb += src_stride_rgb * (height - 1);
	src_stride_rgb = -src_stride_rgb;
	RGBTranspose(src_rgb, src_stride_rgb, dst_rgb, dst_stride_rgb, width, height);

	return 0;
}

int RGBRotate180(const BYTE* src_rgb, int src_stride_rgb, BYTE* dst_rgb, int dst_stride_rgb, int width, int height)
{
	BYTE* row = (BYTE*)malloc(width * 3);
	const BYTE* src_bot = src_rgb + src_stride_rgb * (height - 1);
	BYTE* dst_bot = dst_rgb + dst_stride_rgb * (height - 1);
	int half_height = (height + 1) >> 1;
	int y;

	for (y = 0; y < half_height; ++y) {
		RGBMirrorRow(src_rgb, row, width);  // Mirror first row into a buffer
		RGBMirrorRow(src_bot, dst_rgb, width);  // Mirror last row into first row
		CopyRow(row, dst_bot, width * 3);  // Copy first mirrored row into last
		src_rgb += src_stride_rgb;
		dst_rgb += dst_stride_rgb;
		src_bot -= src_stride_rgb;
		dst_bot -= dst_stride_rgb;
	}

	free(row);

	return 0;
}

int RGBRotate90(const BYTE* src_rgb, int src_stride_rgb, BYTE* dst_rgb, int dst_stride_rgb, int width, int height)
{
	dst_rgb += dst_stride_rgb * (width - 1);
	dst_stride_rgb = -dst_stride_rgb;
	RGBTranspose(src_rgb, src_stride_rgb, dst_rgb, dst_stride_rgb, width, height);

	return 0;
}

void RGBMirrorRow(const BYTE* src, BYTE* dst, int width)
{
	int x;
	BYTE* src_rgb = (BYTE*)src;
	BYTE* dst_rgb = (BYTE*)dst;

	src_rgb += (width - 1) * 3;

	for (x = 0; x < width; x ++) {
		dst_rgb[x * 3] = src_rgb[0];
		dst_rgb[x * 3 + 1] = src_rgb[1];
		dst_rgb[x * 3 + 2] = src_rgb[2];

		src_rgb -= 3;
	}
}