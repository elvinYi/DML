#include "BitmapConverter.h"

#include <boost/assert.hpp>

extern "C"
{
#ifndef __STDC_CONSTANT_MACROS
#  define __STDC_CONSTANT_MACROS
#endif

#ifdef WIN32
#include "libavformat/avformat.h"
#endif
#include "libavutil/avutil.h"
#include "libswscale/swscale.h"
};
#include <boost/system/error_code.hpp>
#include <framework/logger/LoggerSection.h>
#include <framework/logger/LoggerFormatRecord.h>

using namespace framework::logger;
using namespace boost::system;
FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("BitmapConverter", 0);


        BitmapConverter::BitmapConverter(
            StreamFormat src_format, 
            uint32_t src_width, 
            uint32_t src_height, 
            StreamFormat dst_format, 
            uint32_t dst_width, 
            uint32_t dst_height, 
            bool reverse, 
            bool geometric, 
            bool auto_fill,
            int32_t line_size)
            : sws_context_(NULL)
            , reverse_(reverse)
            , src_width_(src_width)
            , src_height_(src_height)
            , dst_width_(dst_width)
            , dst_height_(dst_height)
            , picture_x_(0)
            , picture_y_(0)
            , picture_width_(0)
            , picture_height_(0)
            , back_buffer_(NULL)
            , back_fill_buffer_(NULL)
            , src_frame_(NULL)
            , dst_frame_(NULL)
            , temp_src_(NULL)
            , line_size_(line_size)
        {
            switch(src_format)
            {
            case RGB32:
                src_pixel_=PIX_FMT_RGB32;
                break;
            case BGR32:
                src_pixel_=PIX_FMT_BGR32;
                break;
            case RGB24:
                src_pixel_=PIX_FMT_RGB24;
                break;
            case BGR24:
                src_pixel_=PIX_FMT_BGR24;
                break;
            case PAL8:
                src_pixel_=PIX_FMT_PAL8;
                break;
            case YUV420P:
                src_pixel_=PIX_FMT_YUV420P;
                break;
            case YUV422:
                src_pixel_=PIX_FMT_YUYV422;
                break;
            case YUV444:
                src_pixel_=PIX_FMT_YUV444P;
                break;
            default:
                src_pixel_=PIX_FMT_BGR24;
                break;
            }

            switch(dst_format)
            {
            case BGR32:
                dst_pixel_=PIX_FMT_BGR32;
                break;
            case RGB32:
                dst_pixel_=PIX_FMT_RGB32;
                break;
            case RGB24:
                dst_pixel_=PIX_FMT_RGB24;
                break;
            case BGR24:
                dst_pixel_=PIX_FMT_BGR24;
                break;
            case PAL8:
                dst_pixel_=PIX_FMT_PAL8;
                break;
            case YUV420P:
                dst_pixel_=PIX_FMT_YUV420P;
                break;
            case YUV422:
                dst_pixel_=PIX_FMT_YUYV422;
                break;
            case YUV444:
                dst_pixel_=PIX_FMT_YUV444P;
                break;
            default:
                dst_pixel_=PIX_FMT_BGR24;
                break;
            }

            if (auto_fill && src_width >= dst_width && src_height >= dst_height) {
                 get_filled_video_position(
                     src_width, src_height, 
                     dst_width, dst_height, 
                     picture_x_, picture_y_, 
                     picture_width_, picture_height_);
                 picture_width_ = (picture_width_ >> 3) << 3;
                 picture_height_ = (picture_height_ >> 3) << 3;
                 back_fill_buffer_ = new uint8_t[picture_width_ * picture_height_ * 3];
            } else {
                if (geometric) {
                    get_centered_video_position(src_width, src_height, dst_width, dst_height, picture_x_, picture_y_, picture_width_, picture_height_);
                    picture_width_ = (picture_width_ >> 3) << 3;
                    picture_height_ = (picture_height_ >> 3) << 3;
                    if (picture_x_ != 0) {
                        back_buffer_ = new uint8_t[picture_width_ * picture_height_ * 4];
                    }
                } else {
                    picture_x_ = 0;
                    picture_y_ = 0;
                    picture_width_ = dst_width;
                    picture_height_ = dst_height;
                }
            }
            src_frame_ = avcodec_alloc_frame();
            dst_frame_ = avcodec_alloc_frame();
            temp_src_ = new uint8_t[src_width_* (src_height_ + 1) * 3];
            sws_context_ = sws_getContext(src_width, src_height, src_pixel_, picture_width_, picture_height_, dst_pixel_, SWS_BICUBIC, NULL, NULL, NULL);
        }

        BitmapConverter::~BitmapConverter()
        {
            if (back_buffer_)
            {
                delete [] back_buffer_;
                back_buffer_ = NULL;
            }
            if (back_fill_buffer_) {
                delete [] back_fill_buffer_;
                back_fill_buffer_ = NULL;
            }
            if (sws_context_)
            {
                sws_freeContext(sws_context_);
                sws_context_ = NULL;
            }
            if (temp_src_)
            {
                delete []temp_src_;
                temp_src_ = NULL;
            }
            if(src_frame_)
            {
                av_free(src_frame_);
                src_frame_ = NULL;
            }
            if (dst_frame_)
            {
                av_free(dst_frame_);
                dst_frame_ = NULL;
            }
        }

        void BitmapConverter::convert(const uint8_t *src_buffer, uint8_t *dst_buffer)
        {
            uint8_t * output_buffer = NULL;
            if (!temp_src_ || !dst_buffer || !src_buffer)
            return;

            LOG_F(Logger::kLevelDebug, "src3_linesize %d, dst3_linesize %d\n" % src_frame_->linesize[0] % dst_frame_->linesize[0]);
            // set sorce frame for sacle
            memset(dst_buffer, 0, dst_width_ * dst_height_ * 3);

            if (src_width_==OUTPUT_WIDTH&&src_pixel_==PIX_FMT_BGR24)
            {
                memcpy(temp_src_,src_buffer,src_height_*src_width_*3);
                avpicture_fill( reinterpret_cast<AVPicture *>(src_frame_), const_cast<uint8_t *>(temp_src_), src_pixel_, src_width_, src_height_ );
            }
            else
            {
                avpicture_fill( reinterpret_cast<AVPicture *>(src_frame_), const_cast<uint8_t *>(src_buffer), src_pixel_, src_width_, src_height_ );
            }
            LOG_F(Logger::kLevelDebug, "src2_linesize %d, dst2_linesize %d\n" % src_frame_->linesize[0] % dst_frame_->linesize[0]);
            switch(src_pixel_)
            {
            case PIX_FMT_RGB32:
            case PIX_FMT_BGR32:
                 src_frame_->linesize[0] = src_width_ * 4;
                 break;
            case PIX_FMT_RGB24:
            case PIX_FMT_BGR24:
                {
                    if (line_size_ != 0) {
                        src_frame_->linesize[0] = line_size_;
                    }
                    else {
                        if((src_width_*3)%4!=0)
                        {
                            src_frame_->linesize[0] = src_width_ * 3+ (4-(src_width_*3)%4);
                        }
                        else
                        {
                            src_frame_->linesize[0] = src_width_ * 3;
                        }
                    }
                    break;
                }
            case PIX_FMT_PAL8:
                src_frame_->linesize[0] = src_width_;
                break;
            case PIX_FMT_YUV420P:
                src_frame_->linesize[0] = src_width_;
                src_frame_->linesize[1] = src_width_ >> 1;
                src_frame_->linesize[2] = src_width_ >> 1;
                break;
            }

            if (back_fill_buffer_) {
                output_buffer = back_fill_buffer_;
                assert(dst_pixel_ == PIX_FMT_RGB24 || dst_pixel_ == PIX_FMT_BGR24);
            } else {
                if (back_buffer_ == NULL) {
                    output_buffer = dst_buffer + picture_y_ * picture_width_ * 3;
                } else {
                    output_buffer = back_buffer_;
                }
            }
            

            avpicture_fill( reinterpret_cast<AVPicture *>(dst_frame_), output_buffer, dst_pixel_, picture_width_, picture_height_);
            LOG_F(Logger::kLevelDebug, "src0_linesize %d, dst0_linesize %d\n" % src_frame_->linesize[0] % dst_frame_->linesize[0]);
            dst_frame_->linesize[0] = picture_width_ * 3;

            if (reverse_)
            {
                if (src_pixel_ != PIX_FMT_YUV420P)
                {
                    src_frame_->data[0] += src_frame_->linesize[0] * (src_height_ - 1);
                    src_frame_->linesize[0] *= -1;
                }

            }
			LOG_F(Logger::kLevelDebug, "src_linesize %d, dst_linesize %d\n" % src_frame_->linesize[0] % dst_frame_->linesize[0]);
            if(sws_context_ != NULL)
            {
                if (sws_scale(
                    sws_context_, 
                    src_frame_->data, 
                    src_frame_->linesize, 
                    0, 
                    src_height_, 
                    dst_frame_->data, 
                    dst_frame_->linesize) != picture_height_) {
                        int _temp = 0;
                        _temp = _temp;
                }
            }

            if (back_fill_buffer_) {
                if (picture_x_ > 0) {
                    assert(picture_y_ == 0);
                    uint32_t obj_offet = 0;
                    uint8_t * line_buffer = NULL;
                    assert((picture_width_ - picture_x_) > dst_width_);
                    for (uint32_t _i = 0; _i < picture_height_; ++_i)
                    {
                        line_buffer = back_fill_buffer_ + (picture_width_ * 3 * _i);
                        memcpy(dst_buffer + obj_offet, line_buffer + picture_x_ * 3, dst_width_ * 3);
                        obj_offet += (dst_width_ * 3);
                    }
                }
                if (picture_y_ > 0) {
                    assert(picture_x_ == 0);
                    uint32_t offset = picture_y_ * dst_width_ * 3;
                    assert((picture_width_ * picture_height_ * 3 - offset) > (dst_width_ * dst_height_ * 3));
                    memcpy(dst_buffer, back_fill_buffer_ + offset, dst_width_ * dst_height_ * 3);
                }
                if ( (picture_x_ == 0) && (picture_y_ == 0) )
                {
                    assert(picture_width_ == dst_width_);
                    assert(picture_height_ == dst_height_);
                    memcpy(dst_buffer, back_fill_buffer_, dst_width_ * dst_height_ * 3);
                }
            } else {
                if (back_buffer_ != NULL) {
                    for (uint32_t i = 0; i < picture_height_; ++i) {
                        memcpy(
                            dst_buffer + i * dst_width_ * 3 + picture_x_ * 3, 
                            back_buffer_ + i * picture_width_  * 3, 
                            picture_width_ * 3);
                    }
                }
            }

        }

        void BitmapConverter::get_centered_video_position(uint32_t src_width, uint32_t src_height, uint32_t dst_width, uint32_t dst_height,
                                                          uint32_t &picture_x, uint32_t &picture_y, uint32_t &picture_width, uint32_t &picture_height)
        {
            if ( dst_width * src_height <= dst_height * src_width)
            {
                picture_x       = 0;
                picture_y       = (dst_height - dst_width * src_height / src_width ) >> 1;
                picture_width   = dst_width;
                picture_height  = dst_width * src_height / src_width;
            }
            else
            {
                picture_x       = (dst_width - dst_height * src_width / src_height) >> 1;
                picture_y       = 0;
                picture_width   = dst_height * src_width / src_height;
                picture_height  = dst_height;
            }
        }

        void BitmapConverter::get_filled_video_position(
            uint32_t src_width, 
            uint32_t src_height, 
            uint32_t dst_width, 
            uint32_t dst_height,
            uint32_t &picture_x, 
            uint32_t &picture_y, 
            uint32_t &picture_width, 
            uint32_t &picture_height)
        {
            if ( src_width * dst_height <= src_height * dst_width ) { // keep width
                picture_width = dst_width;
                picture_height = dst_width * src_height / src_width;
                picture_width = picture_width / 2 * 2;
                picture_height = picture_height / 2 * 2;
                assert(picture_height >= dst_height);
                picture_x = 0;
                picture_y = (picture_height - dst_height) / 2;
            } else {
                picture_width = src_width * dst_height / src_height;
                picture_height = dst_height;
                picture_width = picture_width / 2 * 2;
                picture_height = picture_height / 2 * 2;
                assert(picture_width >= dst_width);
                picture_x = (picture_width - dst_width) / 2;
                picture_y = 0;
            }
        }
