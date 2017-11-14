#include "BitmapConverterForCamera.h"

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

BitmapConverterForCamera::BitmapConverterForCamera(boost::uint32_t src_width, boost::uint32_t src_height,
    boost::uint32_t dst_width, boost::uint32_t dst_height)
    : sws_context_(NULL), 
     src_width_(src_width),
     src_height_(src_height),
     dst_width_(dst_width),
     dst_height_(dst_height),
     converted_width_(0),
     converted_height_(0),
     slice_point_x_(0),
     slice_point_y_(0),
     src_frame_(NULL),
     dst_frame_(NULL),
     dst_buffer_offset_(0),
     dst_buffer_linesize_(0)
{
    src_frame_ = avcodec_alloc_frame();
    dst_frame_ = avcodec_alloc_frame();
    get_camera_converted_image_size(src_width, src_height, dst_width, dst_height_, converted_width_, converted_height_);
    get_camera_converted_image_position(converted_width_, converted_height_, dst_width, dst_height_, slice_point_x_, slice_point_y_);

    int sws_algorithm = SWS_BICUBIC;
    //if (converted_width_ * converted_height_ <=  src_width * src_height)
    //{
    //    sws_algorithm = SWS_POINT;
    //}
    //else
    //{
    //    sws_algorithm = SWS_FAST_BILINEAR;
    //}
    sws_context_ = sws_getContext(src_width, src_height, PIX_FMT_BGR24, converted_width_, converted_height_, PIX_FMT_RGB24,
        sws_algorithm, NULL, NULL, NULL);
    dst_buffer_offset_ = (slice_point_y_ * dst_width_ + slice_point_x_) * 3;
    dst_buffer_linesize_ = dst_width * 3;
    
}

BitmapConverterForCamera::~BitmapConverterForCamera(void)
{
    if (sws_context_)
    {
        sws_freeContext(sws_context_);
        sws_context_ = NULL;
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

void BitmapConverterForCamera::convert(const boost::uint8_t *src_buffer, boost::uint8_t *dst_buffer)
{
    avpicture_fill( reinterpret_cast<AVPicture *>(src_frame_), const_cast<uint8_t *>(src_buffer), PIX_FMT_BGR24, src_width_, src_height_ );
    avpicture_fill( reinterpret_cast<AVPicture *>(dst_frame_), dst_buffer + dst_buffer_offset_, PIX_FMT_RGB24, converted_width_, converted_height_);
    src_frame_->linesize[0] = src_width_ * 3;
    dst_frame_->linesize[0] = dst_buffer_linesize_;

    src_frame_->data[0] += src_frame_->linesize[0] * (src_height_ - 1);
    src_frame_->linesize[0] *= -1;

    sws_scale(sws_context_, src_frame_->data, src_frame_->linesize, 0, src_height_, dst_frame_->data, dst_frame_->linesize);
}

void BitmapConverterForCamera::get_camera_converted_image_size(boost::uint32_t src_width, 
        boost::uint32_t src_height, boost::uint32_t dest_width, 
        boost::uint32_t dest_height, boost::uint32_t &convert_width, boost::uint32_t &convert_height)
{
    convert_width = dest_width;
    convert_height = dest_height;
    if (src_width * dest_height > src_height * dest_width)
    {
        convert_height = convert_width * src_height / src_width;
    }
    else if (src_width * dest_height < src_height * dest_width)
    {
        convert_width = convert_height * src_width / src_height;
    }
    convert_width = convert_width >> 3 << 3;
    convert_height = convert_height >> 3 << 3;
}

void BitmapConverterForCamera::get_camera_converted_image_position(boost::uint32_t converted_width, 
        boost::uint32_t converted_height,boost::uint32_t dest_width,
        boost::uint32_t dest_height, boost::uint32_t &x, boost::uint32_t &y)
{
    x = 0;
    y = 0;
    if (converted_width * dest_height > converted_height * dest_width)
    {
        y = ( dest_height - converted_height ) >> 1;
    }
    else if (converted_width * dest_height < converted_height * dest_width)
    {
        x = ( dest_width - converted_width ) >> 1;
    }
}
