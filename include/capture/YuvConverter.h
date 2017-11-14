#ifndef __YUV_CONVERTER_H__
#define __YUV_CONVERTER_H__

#include <stdint.h>

#include "libavutil/pixfmt.h"
extern "C"
{
#include <libavcodec/avcodec.h>
};

struct SwsContext;


        class YuvConverter
        {
        public:
            enum YuvFormat
            {
                YUV420 = 0,
                YUV422      //暂时不支持
            };
            YuvConverter(YuvFormat src_format, uint32_t src_width, uint32_t src_height, 
                YuvFormat dst_format, uint32_t dst_width, uint32_t dst_height, bool geometric);
            virtual ~YuvConverter(void);
            bool convert(const uint8_t *src_buffer, uint8_t *dst_buffer);

        private:
            bool ScaleYUVSize(uint8_t* Srcbuffer,int SrcWidth,int SrcHeight,uint8_t* Dstbuffer,int DstWidth,int DstHeight);
            void get_centered_video_position(uint32_t src_width, uint32_t src_height, uint32_t dst_width, uint32_t dst_height,
                uint32_t &picture_x, uint32_t &picture_y, uint32_t &picture_width, uint32_t &picture_height);
            bool mix_video(uint8_t* pData, uint32_t iDataLen, uint32_t iVideoWidth, uint32_t iVideoHeight, 
                uint8_t* pData_1, uint32_t iDataLen_1, uint32_t iWidth, uint32_t iHeight, uint32_t iOW, uint32_t iOH);
        private:
            uint32_t            src_width_;
            uint32_t            src_height_;
            uint32_t            dst_width_;
            uint32_t            dst_height_;
            uint32_t            picture_x_;
            uint32_t            picture_y_;
            uint32_t            picture_width_;
            uint32_t            picture_height_;
            uint8_t *           picture_buffer_;
            SwsContext *        sws_context_;
            PixelFormat         src_pixel_;
            PixelFormat         dst_pixel_;
            AVFrame *           FrameSrc_;
            AVFrame *           FrameDst_;
        };

#endif