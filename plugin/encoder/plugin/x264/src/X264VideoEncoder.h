#ifndef X264_VIDEOENCODER_H_
#define X264_VIDEOENCODER_H_

#include "BaseDataTypes.h"
#include "BaseBuffer.h"
#include "BaseCodec.h"
extern "C"
{
#include <stdint.h>
#include "x264.h"
}

namespace MediaPlugin {
    
//    enum X264VideoMetaIndex
//    {
//        PID_VIDEO_X264_ENC_CRF      = (PID_VIDEO_BASE | 0X0100),
//        PID_VIDEO_X264_ENC_BUFSIZE  = (PID_VIDEO_BASE | 0X0101),
//        PID_VIDEO_X264_ENC_PRESET   = (PID_VIDEO_BASE | 0X0102),
//        PID_VIDEO_X264_ENC_PROFILE  = (PID_VIDEO_BASE | 0X0103),
//        PID_VIDEO_X264_ENC_OPTARG   = (PID_VIDEO_BASE | 0X0104),
//        PID_VIDEO_X264_ENC_TUNE     = (PID_VIDEO_BASE | 0X0105),
//#if 0
//        PID_VIDEO_X264_ENC_MINQP = (PID_VIDEO_BASE | 0X0100),
//        PID_VIDEO_X264_ENC_MAXQP = (PID_VIDEO_BASE | 0X0101),
//        PID_VIDEO_X264_ENC_CABAC = (PID_VIDEO_BASE | 0X0102),
//        PID_VIDEO_X264_ENC_PSKIP = (PID_VIDEO_BASE | 0X0103),
//        PID_VIDEO_X264_ENC_SPEED = (PID_VIDEO_BASE | 0X0104),
//        PID_VIDEO_X264_ENC_PROFILE = (PID_VIDEO_BASE | 0x0105),
//        PID_VIDEO_X264_ENC_OPTARG = (PID_VIDEO_BASE | 0x0106),
//        PID_VIDEO_X264_ENC_I_FRAME_MIN_INTERTVAL = (PID_VIDEO_BASE | 0X0107),
//        PID_VIDEO_X264_ENC_I_FRAME_MAX_INTERTVAL = (PID_VIDEO_BASE | 0X0108)
//#endif
//    };
    
    //++rayman
    struct X264Settings;
    //--
    class X264VideoEncoder : public BaseCodec
    {
    public:
        X264VideoEncoder();
        ~X264VideoEncoder();
        
        SInt32      initialize(SInt32 codecType);
        SInt32      uninitialize();
        
        SInt32      internalGetParameter(SInt32 index, Void* params);
        SInt32      internalSetParameter(SInt32 index, Void* params);
        
        SInt32      start();
        SInt32      stop();
        
        SInt32      processInput(BaseBuffer* inputBuffer);
        SInt32      processOutput(BaseBuffer* outputBuffer);
        
    protected:
        bool        X264_EncodeVideo(BaseBuffer* inputBuffer, BaseBuffer* outputBuffer);
        bool        X264_FlushDelayedFrames(BaseBuffer* outputBuffer);
        //    UInt32      X264_GetInitDelay();
        SInt32      X264_GetCurrentFrames();    // deprecated
        SInt32      X264_GetBitrate();          // deprecated
        Void        X264_EncodeHeader();
        
        //++rayman
        Void        X264Defaults();
        SInt32      X264UpdateBufSize(Bool useBufSize, SInt32 bufSize);
        SInt32      X264UpdateCBR(SInt32 bitrateKbps = 2500);
        SInt32      X264UpdateCRF(SInt32 value /*1~51*/ );
        Void        X264OverrideBaseParam(const Int8 *param, Int8 **preset, Int8 **profile, Int8 **tune);
        Void        X264OverrideBaseParams(Int8 **params, Int8 **preset, Int8 **profile, Int8 **tune);
        Void        X264SetParam(const Int8 *param);
        Void        X264ApplyProfile(const Int8 *profile);
        const Int8* X264ValidatePreset(const Int8 *preset);
        Bool        X264ResetParams(const Int8 *preset, const Int8 *tune);
        const Int8* X264GetColorSpaceName(VideoColorSpace cs);
        SInt32      X264GetCSVal(VideoColorSpace cs, const char *const names[]);
        Void        X264UpdateParams(X264Settings* settings, Int8 **params);
        Bool        X264UpdateSettings(X264Settings* settings);
        Bool        X264Update(X264Settings* settings);
        SInt32      X264Create(X264Settings* settings);
        Void        X264Destroy();
        //--rayman
        
    protected:
        //X264VideoConfig *mVideoConfig;
        State           mState;
        BaseBuffer      *mInputBuffer;
        bool            mFlush;
        
        x264_picture_t  *mX264Pic;
        x264_t          *mX264;
        x264_param_t    *mX264Param;
        
        UInt32         mInitDelay;
        
        UInt8          *mDataAVCC;
        UInt8          *mDataSPS;
        UInt8          *mDataPPS;
        UInt32         mDataAVCCLength;
        SInt32         mDataSPSLength;
        SInt32         mDataPPSLength;
        
        //++rayman
        X264Settings*   mSettings;
        //--
        
        bool            mIsFirstPicAllocFlag;
        
    };
    
} // namespace MediaPlugin

#endif //X264_VIDEOENCODER_H_