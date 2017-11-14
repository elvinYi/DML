#ifndef __BITMAP_CONVERTER_FOR_CAMERA_H__
#define __BITMAP_CONVERTER_FOR_CAMERA_H__


#include <boost/cstdint.hpp>


extern "C"
{
    #include "libavcodec/avcodec.h"
    #include "libavutil/pixfmt.h"
    #include "libswscale/swscale.h"
};

class BitmapConverterForCamera
{
public:
    BitmapConverterForCamera(boost::uint32_t src_width, boost::uint32_t src_height,
        boost::uint32_t dst_width, boost::uint32_t dst_height);
    ~BitmapConverterForCamera(void);

    void convert(const boost::uint8_t *src_buffer, boost::uint8_t *dst_buffer );

private:
    BitmapConverterForCamera(const BitmapConverterForCamera &) {}
    BitmapConverterForCamera &operator=(const BitmapConverterForCamera &) {}

    static void get_camera_converted_image_size(boost::uint32_t src_width, boost::uint32_t src_height, 
        boost::uint32_t dest_width, boost::uint32_t dest_height,
        boost::uint32_t &convert_width, boost::uint32_t &convert_height);
    static void get_camera_converted_image_position(boost::uint32_t converted_width, boost::uint32_t converted_height, 
        boost::uint32_t dest_width, boost::uint32_t dest_height, 
        boost::uint32_t &x, boost::uint32_t &y );

private:
    SwsContext *        sws_context_;
    boost::uint32_t     src_width_;
    boost::uint32_t     src_height_;
    boost::uint32_t     dst_width_;
    boost::uint32_t     dst_height_;
    boost::uint32_t     converted_width_;
    boost::uint32_t     converted_height_;
    boost::uint32_t     slice_point_x_;
    boost::uint32_t     slice_point_y_;
    AVFrame *           src_frame_;
    AVFrame *           dst_frame_;
    boost::uint32_t     dst_buffer_offset_;
    boost::uint32_t     dst_buffer_linesize_;
};

#endif