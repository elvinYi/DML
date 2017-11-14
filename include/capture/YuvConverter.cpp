#include "YuvConverter.h"
#include <string>
#include <boost/cstdint.hpp>
#include <boost/system/error_code.hpp>
#include <framework/logger/Logger.h>
#include <framework/logger/LoggerSection.h>
#include <framework/logger/LoggerStreamRecord.h>
using namespace framework::configure;
using namespace framework::logger;
using namespace boost::system;
FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("YuvConverter", 0);
extern "C"
{
    #include "libswscale/swscale.h"
};


        YuvConverter::YuvConverter(YuvFormat src_format, uint32_t src_width, uint32_t src_height, 
            YuvFormat dst_format, uint32_t dst_width, uint32_t dst_height, bool geometric)
            : src_width_(src_width)
            , src_height_(src_height)
            , dst_width_(dst_width)
            , dst_height_(dst_height)
            , picture_x_(0)
            , picture_y_(0)
            , picture_width_(0)
            , picture_height_(0)
            , picture_buffer_(NULL)
            , sws_context_(NULL)
            , FrameSrc_(NULL)
            , FrameDst_(NULL)
        {
            src_pixel_ = (src_format == YUV420) ? PIX_FMT_YUV420P : PIX_FMT_YUYV422;
            dst_pixel_ = (dst_format == YUV420) ? PIX_FMT_YUV420P : PIX_FMT_YUYV422;
            if (geometric)
            {
                get_centered_video_position(src_width, src_height, dst_width, dst_height_, picture_x_, picture_y_, picture_width_, picture_height_);
                picture_width_ = (picture_width_ >> 2) << 2;
                picture_height_ = (picture_height_ >> 2) << 2;
            }
            else
            {
                picture_x_ = 0;
                picture_y_ = 0;
                picture_width_ = dst_width;
                picture_height_ = dst_height;
            }
            picture_buffer_ = new uint8_t[picture_width_ * picture_height_ * 1.5];
            sws_context_ = sws_getContext(src_width, src_height, src_pixel_, picture_width_, picture_height_, dst_pixel_, SWS_BICUBIC, NULL, NULL, NULL);
            FrameSrc_ = avcodec_alloc_frame();
            FrameDst_ = avcodec_alloc_frame();
        }

        YuvConverter::~YuvConverter(void)
        {
            if (sws_context_)
            {
                sws_freeContext(sws_context_);
                sws_context_ = NULL;
            }
            if (FrameSrc_)
            {
                av_free(FrameSrc_);
                FrameSrc_ = NULL;
            }
            if (FrameDst_)
            {
                av_free(FrameDst_);
                FrameDst_ = NULL;
            }
            
            if (picture_buffer_)
            {
                delete [] picture_buffer_;
                picture_buffer_ = NULL;
            }
        }

        bool YuvConverter::convert(const uint8_t *src_buffer, uint8_t *dst_buffer)
        {
            if (dst_buffer && src_buffer)
            {
                memset(dst_buffer, 0, dst_width_ * dst_height_ * 1.5);
                memset(dst_buffer + (dst_width_ * dst_height_), 128, (dst_width_ >> 1) * (dst_height_ >> 1) << 1);   //yuv ºÚÉ«
                ScaleYUVSize((uint8_t*)src_buffer, src_width_, src_height_, picture_buffer_, picture_width_, picture_height_);
                mix_video(dst_buffer, dst_width_ * dst_height_ * 1.5, dst_width_, dst_height_, picture_buffer_, picture_width_ * picture_height_ * 1.5,
                 picture_width_, picture_height_, picture_x_, picture_y_);
                
                return true;
            }

            LOG_S(Logger::kLevelDebug, "[YuvConverter::convert] failed!");

            return false;
        }

        bool YuvConverter::ScaleYUVSize(uint8_t* Srcbuffer,int SrcWidth,int SrcHeight,uint8_t* Dstbuffer,int DstWidth,int DstHeight)
        {
            int32_t ret = 0;
            ret = avpicture_fill((AVPicture*)FrameSrc_,(uint8_t*)Srcbuffer,PIX_FMT_YUV420P,SrcWidth,SrcHeight);
            ret = avpicture_fill((AVPicture*)FrameDst_,(uint8_t*)Dstbuffer,PIX_FMT_YUV420P,DstWidth,DstHeight);

            ret = sws_scale(sws_context_,FrameSrc_->data,FrameSrc_->linesize,0,SrcHeight,FrameDst_->data,FrameDst_->linesize);
            if (ret != DstHeight){
                LOG_S(Logger::kLevelDebug, "[ScaleYUVSize] fail, ret = "<< ret << "SrcHeight = " << SrcHeight);
                return false;
            }
            return true;
        }

        void YuvConverter::get_centered_video_position(uint32_t src_width, uint32_t src_height, uint32_t dst_width, uint32_t dst_height,
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

        bool YuvConverter::mix_video(uint8_t* pData, uint32_t iDataLen, uint32_t iVideoWidth, uint32_t iVideoHeight, 
            uint8_t* pData_1, uint32_t iDataLen_1, uint32_t iWidth, uint32_t iHeight, uint32_t iOW, uint32_t iOH)
        {
            uint32_t iDataSize = 0; 
            uint32_t iTimeStamp  = 0;
            uint8_t* pY = NULL;
            uint8_t* pU = NULL;
            uint8_t* pV = NULL;
            uint8_t* pY1 = NULL;
            uint8_t* pU1 = NULL;
            uint8_t* pV1 = NULL;
            uint32_t iLength = 0;
            uint32_t iLength1 = 0;
            uint32_t iOffset = 0;

            if (!pData && !pData_1)
            {
                LOG_S(Logger::kLevelDebug, "MixVideo Fun  !pData or !pData_1 !");
                return false;
            }

            iLength  = iVideoWidth * iVideoHeight;
            iLength1 = iHeight * iWidth;
            iOffset  = (iOH) * iVideoWidth;

            if ((iOffset < iLength) && (iLength < iDataLen) )
            {
                pY  = pData + iOffset;       
                iOffset = (iOH >> 1) * (iVideoWidth >> 1);
                pU  = pData + iLength + iOffset;
                pV  = pData + iLength + (iLength >> 2) + iOffset;

                pY1 = pData_1;
                pU1 = pData_1 + iLength1;
                pV1 = pData_1 + iLength1 + (iLength1 >> 2);
            }
            else
            {
                LOG_S(Logger::kLevelDebug, "MixVideo Fun  if(iOffset < iLength) && (iLength < iDataLen)");
                return false;
            }

            int i = 0;
            for (i = 0; i < (iHeight >> 1); i++)
            {
                //Replace Y
                memcpy(pY + iOW, pY1 + ((i * iWidth) << 1), iWidth);
                pY += iVideoWidth;
                memcpy(pY + iOW, pY1 + ((i << 1)  + 1) * iWidth, iWidth);
                pY += iVideoWidth;
                //Replace U
                memcpy(pU + (iOW >> 1), pU1 + i * (iWidth >> 1), (iWidth >> 1));
                pU += (iVideoWidth >> 1);
                //Replace V
                memcpy(pV + (iOW >> 1), pV1 + i * (iWidth >> 1), (iWidth >> 1));
                pV += (iVideoWidth >> 1);
            }		

            return true;
        }
