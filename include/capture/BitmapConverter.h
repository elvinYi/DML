#ifndef __BITMAP_CONVERTER_H__
#define __BITMAP_CONVERTER_H__

#include <stdint.h>
#include <ppshow/ppshowsdk/base_define.h>

#include "libavutil/pixfmt.h"

extern "C"
{
#include "libavcodec/avcodec.h"
};

struct SwsContext;


        class BitmapConverter 
        {
        public:
            enum StreamFormat
            {
				BGR32,
				RGB32,
                RGB24,
                BGR24,
				PAL8,
                YUV420P,
                YUV422,
                YUV444
            };

            BitmapConverter(
                StreamFormat src_format, 
                uint32_t src_width, 
                uint32_t src_height,
                StreamFormat dst_format, 
                uint32_t dst_width, 
                uint32_t dst_height, 
                bool reverse = false, 
                bool geometric = false, 
                bool auto_fill = false,
                int32_t line_size = 0);

            ~BitmapConverter();
            
            void convert(const uint8_t *src_buffer, uint8_t *dst_buffer );

            
        private:
            BitmapConverter(const BitmapConverter &) {}
            BitmapConverter &operator=(const BitmapConverter &) {}

            void get_centered_video_position(uint32_t src_width, uint32_t src_height, uint32_t dst_width, uint32_t dst_height,
                uint32_t &picture_x, uint32_t &picture_y, uint32_t &picture_width, uint32_t &picture_height);

            void get_filled_video_position(
                uint32_t src_width, 
                uint32_t src_height, 
                uint32_t dst_width, 
                uint32_t dst_height,
                uint32_t &picture_x, 
                uint32_t &picture_y, 
                uint32_t &picture_width, 
                uint32_t &picture_height);

            SwsContext *        sws_context_;
            bool                reverse_;
            uint32_t            src_width_;
            uint32_t            src_height_;
            uint32_t            dst_width_;
            uint32_t            dst_height_;
            uint32_t            picture_x_;
            uint32_t            picture_y_;
            uint32_t            picture_width_;
            uint32_t            picture_height_;
            uint8_t *           back_buffer_;
            uint8_t *           back_fill_buffer_;
            PixelFormat         src_pixel_;
            PixelFormat         dst_pixel_;
            AVFrame *           src_frame_;
            AVFrame *           dst_frame_;
            uint8_t *           temp_src_;
            int32_t             line_size_;
        };

#endif