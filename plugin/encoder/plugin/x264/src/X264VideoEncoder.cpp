#include "BaseBuffer.h"
#include "X264VideoEncoder.h"
#include "Endian.h"

#ifdef __APPLE__

#include "dstr.h"
#include "darray.h"

#else

#include "dstr.h"
#include "darray.h"
#endif

#define MIN(a,b) ((a) > (b) ? (b) : (a))

#ifdef WIN32
#define  strcasecmp _stricmp
#else
#include <string.h>
#endif

#define DEBUG_TAG "X264VideoEncoder"

namespace MediaPlugin {
    
    struct X264Settings
    {
        SInt32  mWidth;
        SInt32  mHeight;
        SInt32  mFps;
        SInt32  mBitrate;
        Bool    mUseBufSize;
        SInt32  mBufferSize;
        SInt32  mKeyIntSec;
        SInt32  mCRF;
        Bool    mVFR;
        Bool    mCBR;
        
        Int8    mPreset[256];
        Int8    mProfile[256];
        Int8    mTune[256];
        Int8    mX264opts[256];
    };
    
    
    X264VideoEncoder::X264VideoEncoder()
    : mSettings(NULL),
    mX264Param(NULL),
    mX264Pic(NULL),
    mX264(NULL),
    mFlush(false),
    mDataSPS(NULL),
    mDataPPS(NULL),
    mDataAVCC(NULL),
    mState(UNINITIALIZED)
    {
        
    }
    
    X264VideoEncoder::~X264VideoEncoder()
    {
        
    }
    
    SInt32 X264VideoEncoder::initialize(SInt32 codecType)
    {
        if (mState == UNINITIALIZED)
        {
            if (!mX264Param)
            {
                mX264Param = new x264_param_t;
                memset(mX264Param, 0, sizeof(x264_param_t));

				x264_param_default(mX264Param);
            }
            
            if (!mX264Pic)
            {
                mX264Pic = new x264_picture_t;
                memset(mX264Pic, 0, sizeof(x264_picture_t));
            }
            
            if (!mSettings)
            {
                mSettings = new X264Settings;
                memset(mSettings, 0, sizeof(X264Settings));
                X264Defaults();
            }
            
            mState = INITIALIZED;
        }
        return OK;
    }
    
    //++rayman
    Void X264VideoEncoder::X264Defaults()
    {
        mSettings->mFps         = 30;
        mSettings->mBitrate     = 2500;
        mSettings->mUseBufSize  = Value_False;
        mSettings->mBufferSize  = 2500;
        mSettings->mKeyIntSec   = 5;
        mSettings->mCRF         = 23;
        mSettings->mVFR         = Value_False;
        mSettings->mCBR         = Value_True;
        
        strcpy(mSettings->mPreset, "veryfast");
    }
    
    SInt32 X264VideoEncoder::X264UpdateBufSize(Bool useBufSize, SInt32 bufSize)
    {
        if (useBufSize)
        {
            if (bufSize < 0 || bufSize > 10000000)
            {
                return BAD_VALUE;
            }
            mSettings->mUseBufSize = Value_True;
            mSettings->mBufferSize = bufSize;
        }
        
        return OK;
    }
    
    SInt32 X264VideoEncoder::X264UpdateCBR(SInt32 bitrateKbps)
    {
        //if (bitrateKbps < 10 || bitrateKbps > 20000)
        if (bitrateKbps < 10)
        {
            return BAD_VALUE;
        }
        //mSettings->mCBR = Value_True;
        mSettings->mBitrate = bitrateKbps;
        
        return OK;
    }
    
    SInt32 X264VideoEncoder::X264UpdateCRF(SInt32 value /*1~51*/)
    {
        if (value < 1 || value > 50)
        {
            return BAD_VALUE;
        }
        mSettings->mCBR = Value_False;
        mSettings->mCRF = value;
        
        return OK;
    }
    
    static bool getparam(const char *param, char **name, const char **value)
    {
        const char *assign;
        
        if (!param || !*param || (*param == '='))
        {
            //LogPrintf_Warr(DEBUG_TAG,__LINE__,"[getparam]: exit fail with param invailed\n");
            return false;
        }

        assign = strchr(param, '=');
        if (!assign || !*assign || !*(assign + 1))
        {
            //LogPrintf_Warr(DEBUG_TAG,__LINE__,"[getparam]: exit fail with assign invailed\n");
            return false;
        }
        
        
        *name = bstrdup_n(param, assign - param);
        *value = assign + 1;
        return true;
    }
    
    static const Int8 *validate(const Int8 *val, const Int8 *name, const Int8 *const *list)
    {
        if (!val || !*val)
            return val;
        
        while (*list)
        {
            if (strcmp(val, *list) == 0)
                return val;
            
            list++;
        }
        //LogPrintf_Warr(DEBUG_TAG,__LINE__,"[validate]: exit fail with assign invailed, name:%s\n", name);
        //printf("Invalid %s: %s\n", name, val);
        return NULL;
    }
    
    Void X264VideoEncoder::X264OverrideBaseParam(const Int8 *param, Int8 **preset, Int8 **profile, Int8 **tune)
    {
        Int8       *name;
        const Int8 *val;
        
        if (getparam(param, &name, &val))
        {
            if (astrcmpi(name, "preset") == 0)
            {
                const Int8 *valid_name = validate(val, "preset", x264_preset_names);
                if (valid_name)
                {
                    bfree(*preset);
                    *preset = bstrdup(val);
                }
                
            }
            else if (astrcmpi(name, "profile") == 0)
            {
                const Int8 *valid_name = validate(val, "profile", x264_profile_names);
                if (valid_name)
                {
                    bfree(*profile);
                    *profile = bstrdup(val);
                }
                
            }
            else if (astrcmpi(name, "tune") == 0)
            {
                const Int8 *valid_name = validate(val, "tune", x264_tune_names);
                if (valid_name)
                {
                    bfree(*tune);
                    *tune = bstrdup(val);
                }
            }
            
            bfree(name);
        }
    }
    
    Void X264VideoEncoder::X264OverrideBaseParams(Int8 **params, Int8 **preset, Int8 **profile, Int8 **tune)
    {
        while (*params)
            X264OverrideBaseParam(*(params++), preset, profile, tune);
    }
    
    Void X264VideoEncoder::X264SetParam(const Int8 *param)
    {
        Int8       *name;
        const Int8 *val;
        
        if (getparam(param, &name, &val))
        {
            if (strcmp(name, "preset") != 0 &&
                strcmp(name, "profile") != 0 &&
                strcmp(name, "tune") != 0 &&
                strcmp(name, "fps") != 0 &&
                strcmp(name, "force-cfr") != 0 &&
                strcmp(name, "width") != 0 &&
                strcmp(name, "height") != 0)
            {
                if (x264_param_parse(mX264Param, name, val) != 0) {
                    //LogPrintf_Warr(DEBUG_TAG,__LINE__,"[X264SetParam]: x264 param: %s failed", validate);
                    //printf("[%s]: x264 param: %s failed", __FUNCTION__, param);
                }
            }
            
            bfree(name);
        }
    }
    
    Void X264VideoEncoder::X264ApplyProfile(const Int8 *profile)
    {
		SInt32 ret = 0;
		if (!mX264 && profile && *profile)
        {
            ret = x264_param_apply_profile(mX264Param, profile);
            if (ret != 0) {
                printf("[%s]: Failed to set x264 profile '%s'\n", __FUNCTION__, profile);
                //LogPrintf_Warr(DEBUG_TAG,__LINE__,"[%s]: Failed to set x264 profile '%s'\n", __FUNCTION__, profile);
            }
        }
    }
    
    const Int8* X264VideoEncoder::X264ValidatePreset(const Int8 *preset)
    {
        const Int8 *new_preset = validate(preset, "preset", x264_preset_names);
        return new_preset ? new_preset : "veryfast";
    }
    
    Bool X264VideoEncoder::X264ResetParams(const Int8 *preset, const Int8 *tune)
    {
        int ret = x264_param_default_preset(mX264Param,
                                            X264ValidatePreset(preset), validate(tune, "tune", x264_tune_names));
        return ret == 0 ? Value_True : Value_False;
    }
    
    static void log_x264(void *param, int level, const char *format, va_list args)
    {
        char str[1024] = {0};
        
        vsnprintf(str, 1024, format, args);
        blog(level, "%s", str);
		printf("[x264] : %s \n", str);
        //LogPrintf_Warr(DEBUG_TAG, __LINE__, "%s", str);
    }
    
    const Int8* X264VideoEncoder::X264GetColorSpaceName(VideoColorSpace cs)
    {
        switch (cs)
        {
            case VIDEO_CS_DEFAULT:
            case VIDEO_CS_601:
                return "undef";
            case VIDEO_CS_709:;
        }
        
        return "bt709";
    }
    
    SInt32 X264VideoEncoder::X264GetCSVal(VideoColorSpace cs, const char *const names[])
    {
        const Int8 *name = X264GetColorSpaceName(cs);
        SInt32 idx = 0;
        do
        {
            if (strcmp(names[idx], name) == 0)
                return idx;
        } while (!!names[++idx]);
        
        return 0;
    }
    
    Void X264VideoEncoder::X264UpdateParams(X264Settings* settings, Int8 **params)
    {
        SInt32 fps          = settings->mFps;
        SInt32 bitrate      = settings->mBitrate;
        SInt32 buffer_size  = settings->mBufferSize;
        SInt32 keyint_sec   = settings->mKeyIntSec;
        SInt32 crf          = settings->mCRF;
        SInt32 width        = settings->mWidth;
        SInt32 height       = settings->mHeight;
        Bool   use_bufsize  = settings->mUseBufSize;
        Bool   vfr          = settings->mVFR;
        Bool   cbr          = settings->mCBR;
        
        if (keyint_sec)
            mX264Param->i_keyint_max = keyint_sec * fps;
        
        if (!use_bufsize)
            buffer_size = bitrate;
        
      //  mX264Param->b_vfr_input = vfr;
        mX264Param->rc.i_vbv_max_bitrate = bitrate;
        mX264Param->rc.i_vbv_buffer_size = buffer_size;
        mX264Param->rc.i_bitrate = bitrate;
        mX264Param->i_width = width;
        mX264Param->i_height = height;
        mX264Param->i_fps_num = fps;
        mX264Param->i_fps_den = 1;
        mX264Param->pf_log = log_x264;
        mX264Param->p_log_private = NULL;
        mX264Param->i_log_level = X264_LOG_WARNING;
		mX264Param->i_bframe = 0;
		mX264Param->i_threads = 1;
		mX264Param->i_slice_count = 1;
		//mX264Param->rc.i_lookahead = 0;
		//mX264Param->i_sync_lookahead = 0;
		//mX264Param->b_sliced_threads = 1;
		//mX264Param->b_vfr_input = 0;
		//mX264Param->rc.b_mb_tree = 0;
        
        //mX264Param->vui.i_transfer =
        //X264GetCSVal(VIDEO_CS_709, x264_transfer_names);
        //mX264Param->vui.i_colmatrix =
        //X264GetCSVal(VIDEO_CS_709, x264_colmatrix_names);
        //mX264Param->vui.i_colorprim =
        //X264GetCSVal(VIDEO_CS_709, x264_colorprim_names);
        //mX264Param->vui.b_fullrange = Value_True;

		//mX264Param->b_cabac = 0;
        
        /* use the new filler method for CBR to allow real-time adjusting of
         * the bitrate */
//        if (cbr)
//        {
//            mX264Param->rc.f_rf_constant = 0.0f;
//            mX264Param->rc.i_rc_method = X264_RC_ABR;
//            
//#if X264_BUILD >= 139
//            // modified by zjp
//            mX264Param->rc.b_filler = true;
//            mX264Param->i_nal_hrd = X264_NAL_HRD_CBR;
//#else
//            mX264Param->i_nal_hrd = X264_NAL_HRD_CBR;
//#endif
//        }
//        else
//        {
//            mX264Param->rc.i_rc_method = X264_RC_CRF;
//            mX264Param->rc.f_rf_constant = (float)crf;
//        }
//        
//        // current only consider yuv420p only.
//        mX264Param->i_csp = X264_CSP_I420;
        
        while (*params)
            X264SetParam(*(params++));
    }
    
    Bool X264VideoEncoder::X264UpdateSettings(X264Settings* settings)
    {
        Int8 *preset = bstrdup(settings->mPreset);
        Int8 *profile = bstrdup(settings->mProfile);
        Int8 *tune = bstrdup(settings->mTune);
        const Int8 *opts = settings->mX264opts;
        
        Int8 **paramlist;
        Bool success = Value_True;
        
        paramlist = strlist_split(opts, ' ', false);
        
        //blog(LOG_INFO, "---------------------------------");
        
        if (!mX264)
        {
            X264OverrideBaseParams(paramlist, &preset, &profile, &tune);
            
            if (preset  && *preset) {
                //LogPrintf_Debug(DEBUG_TAG, __LINE__, "preset: %s", preset);
                //blog(LOG_INFO, "preset: %s", preset);
            }
            if (profile && *profile) {
                //LogPrintf_Debug(DEBUG_TAG, __LINE__, "profile: %s", profile);
                //blog(LOG_INFO, "profile: %s", profile);
            }
            if (tune && *tune) {
                //LogPrintf_Debug(DEBUG_TAG, __LINE__, "tune: %s", tune);
                //blog(LOG_INFO, "tune: %s", tune);
            }
            
            success = X264ResetParams(preset, tune);
        }
        
        if (success)
        {
            X264UpdateParams(settings, paramlist);
            if (opts && *opts) {
                //LogPrintf_Debug(DEBUG_TAG, __LINE__, "custom settings: %s", opts);
                //blog(LOG_INFO, "custom settings: %s", opts);
            }
            
            if (!mX264)
                X264ApplyProfile(profile);
        }
        
		mX264Param->b_repeat_headers = true;
        
        strlist_free(paramlist);
        bfree(preset);
        bfree(profile);
        bfree(tune);
        
        return success;
    }
    
    Bool X264VideoEncoder::X264Update(X264Settings* settings)
    {
        Bool success = X264UpdateSettings(settings);
        SInt32 ret;
        
        if (success)
        {
            ret = x264_encoder_reconfig(mX264, mX264Param);
            if (ret != 0) {
                //LogPrintf_Debug(DEBUG_TAG, __LINE__, "[X264Update]: Failed to reconfigure: %d", ret);
                //blog(LOG_WARNING, "[%s]: Failed to reconfigure: %d", __FUNCTION__, ret);
            }
            return ret == 0 ? Value_True : Value_False;
        }
        
        return Value_False;
    }
    
    SInt32 X264VideoEncoder::X264Create(X264Settings* settings)
    {
        if (X264UpdateSettings(settings))
        {
            mX264 = x264_encoder_open(mX264Param);
            
//            if (mX264 == NULL)
//                blog(LOG_WARNING, "x264 failed to load");
        }
        else
        {
            blog(LOG_WARNING, "bad settings specified");
        }
        
        if (!mX264)
        {
            //LogPrintf_Error(DEBUG_TAG,__LINE__,"[X264Create]: exit fail with mX264 == NULL\n");
            return DEAD_OBJECT;
        }
        
        return OK;
    }
    
    Void X264VideoEncoder::X264Destroy()
    {
        if (mX264)
        {
            x264_encoder_close(mX264);
        }
    }
    //--
    
    SInt32 X264VideoEncoder::internalSetParameter(SInt32 index, Void* params)
    {
        SInt32 result = OK;
        switch (index)
        {
            case PID_VIDEO_WIDTH:
            {
                mSettings->mWidth = *(SInt32*)params;
                break;
            }
            case PID_VIDEO_HEIGHT:
            {
                mSettings->mHeight = *(SInt32*)params;
                break;
            }
            case PID_VIDEO_BITRATE:
            {
                SInt32 bitrateKbps = *(SInt32*)params;
                X264UpdateCBR(bitrateKbps / 1000);
                break;
            }
            case PID_VIDEO_ENC_FPS:
            {
                SInt32 fps = *(SInt32*)params;
                mSettings->mFps = fps;
                break;
            }
            case PID_VIDEO_ENC_I_FRAME_INTERTVAL:
            {
                SInt32 iFrameInterval = *(SInt32*)params;
                mSettings->mKeyIntSec = iFrameInterval / mSettings->mFps; // ugly...
                break;
            }
            case PID_VIDEO_ENC_CRF:
            {
                SInt32 crf = *(SInt32*)params;
                X264UpdateCRF(crf);
                break;
            }
            case PID_VIDEO_ENC_BUFSIZE:
            {
                SInt32 bufSize = *(SInt32*)params;
                X264UpdateBufSize(Value_True, bufSize);
                break;
            }
			case PID_VIDEO_ENC_PRESET:
			{
				enum X264Preset {
					X264_PRESET_ULTRA_FAST = 0,
					X264_PRESET_SUPER_FAST,
					X264_PRESET_VERY_FAST,
					X264_PRESET_FASTER,
					X264_PRESET_FAST,
					X264_PRESET_MEDIUM,
					X264_PRESET_SLOW,
					X264_PRESET_SLOWER,
				};

				char *str_preset[8] = {
					"ultrafast",
					"superfast",
					"veryfast",
					"faster",
					"fast",
					"medium",
					"slow",
					"slower",
				};

				X264Preset p = *(X264Preset*)params;
				if (p > 8) {
					return BAD_VALUE;
				}

				memset(mSettings->mPreset, 0, 256);
                strncpy(mSettings->mPreset, (Int8*)str_preset[p], strlen(str_preset[p]));
				break;
			}
            case PID_VIDEO_ENC_PROFILE:
            {
#define MAIN_PROFILE 0
#define HIGH_PROFILE 1
#define STR_MAIN "main"
#define STR_HIGH "high"
                memset(mSettings->mProfile, 0, 256);
				int p = *(int*)params;
				char* profile = STR_HIGH;
				if (p == MAIN_PROFILE) {
					profile = STR_MAIN;
				}
                strncpy(mSettings->mProfile, (Int8*)profile, strlen(profile));
                break;
            }
            case PID_VIDEO_ENC_OPTARG:
            {
                BaseBuffer* optargBuf = (BaseBuffer*)params;
                memset(mSettings->mX264opts, 0, 256);
                strncpy(mSettings->mX264opts, (Int8*)optargBuf->getData(), optargBuf->getLength());
                break;
            }
            case PID_VIDEO_ENC_TUNE:
            {
				enum X264Tune {
					X264_TUNE_FILM = 0,
					X264_TUNE_ANIMATION,
					X264_TUNE_GRAIN,
					X264_TUNE_STILLIAMGE,
					X264_TUNE_PSNR,
					X264_TUNE_SSIM,
					X264_TUNE_FASTDECODE,
					X264_TUNE_ZEROLATENCY,
				};

				char *str_tune[8] = {
					"film", 
					"animation", 
					"grain", 
					"stillimage", 
					"psnr", 
					"ssim", 
					"fastdecode", 
					"zerolatency"
				};

				X264Tune p = *(X264Tune*)params;
				if (p >= 8) {
					return BAD_VALUE;
				}
				memset(mSettings->mTune, 0, 256);
				strncpy(mSettings->mTune, (Int8*)str_tune[p], strlen(str_tune[p]));

				/*   BaseBuffer* tuneBuf = (BaseBuffer*)params;
				   memset(mSettings->mTune, 0, 256);
				   strncpy(mSettings->mTune, (Int8*)tuneBuf->getData(), tuneBuf->getLength());*/
                break;
            }
			case PID_VIDEO_CPU_COUNT:
			{
				int cpu = *(int*)params;
				if (cpu != 0) {
					mX264Param->cpu = cpu;
					mX264Param->i_threads = X264_THREADS_AUTO;
				}
				break;
			}
            case CODEC_FLAGS_FLUSH:
            {
                mFlush = true;
            }
            default:
                break;
        }
        return result;
    }
    
    SInt32 X264VideoEncoder::internalGetParameter(SInt32 index, Void* params)
    {
        switch (index)
        {
            case CODEC_FLAGS_HEADDATA:
            {
                BaseBuffer* headerBuffer = (BaseBuffer*)params;
                if (!mDataSPSLength || !mDataPPSLength)
                {
                    //printf("get codec header data failed, encoder has not started!\n");
                    //LogPrintf_Warr(DEBUG_TAG,__LINE__,"[internalGetParameter]: exit fail with mDataSPSLength =%d,mDataPPSLength =%d\n",mDataSPSLength,mDataPPSLength);
                    return INVALID_OPERATION;
                }
                UInt8* data = headerBuffer->getData();
                memcpy(data, mDataSPS, mDataSPSLength);
                memcpy(data + mDataSPSLength, mDataPPS, mDataPPSLength);
                headerBuffer->setLength(mDataSPSLength + mDataPPSLength);
                headerBuffer->setFlags(CODEC_FLAGS_HEADDATA);
                break;
            }
            case CODEC_FLAGS_HEADDATA_LENGTH:
            {
                *(SInt32*)params = mDataSPSLength + mDataPPSLength;
                break;
            }
            case PID_VIDEO_FORMAT:
            {
                // todo: yuv420 and nv12
                *(SInt32*)params = VIDEO_COLOR_FORMAT_NV12;
                break;
            }
            default:
                return BAD_TYPE;
                break;
        }
        return OK;
    }
    
    SInt32 X264VideoEncoder::uninitialize()
    {
        if (mState != INITIALIZED && mState != STOPPED)
        {
            return INVALID_OPERATION;
        }
        
        if (mX264Param)
        {
            delete mX264Param;
            mX264Param = NULL;
        }
        if (mX264Pic)
        {
            delete mX264Pic;
            mX264Pic = NULL;
        }
        
        if (mSettings)
        {
            delete mSettings;
            mSettings = NULL;
        }
        
        return OK;
    }
    
    SInt32 X264VideoEncoder::start()
    {
        if (mState != INITIALIZED && mState != STOPPED)
        {
            return INVALID_OPERATION;
        }
        
        if (X264Create(mSettings) != OK)
        {
            //printf("[%s]: failed to create internal context!\n");
            //LogPrintf_Error(DEBUG_TAG,__LINE__,"[start]: exit fail with X264Create() failed\n");
            return DEAD_OBJECT;
        }
        
        X264_EncodeHeader();
        
        // Create a new pic. modified by zjp, use for multi media color format
        //x264_picture_alloc(mX264Pic, X264_CSP_I420, mSettings->mWidth, mSettings->mHeight);
        
        mIsFirstPicAllocFlag = false;
        mFlush = false;
        mState = STARTED;
        
        return OK;
    }
    
    SInt32 X264VideoEncoder::processInput(BaseBuffer* inputBuffer)
    {
        if (mState != STARTED){
            //LogPrintf_Error(DEBUG_TAG,__LINE__,"[processInput]: exit fail with mState != STARTED\n");
            return INVALID_OPERATION;
        }
        
        // performs buffer check;
        if (inputBuffer == NULL || inputBuffer->getData() == NULL){
            //LogPrintf_Error(DEBUG_TAG,__LINE__,"[processInput]: exit fail with inputBuffer == NULL || inputBuffer->getData() == NULL\n");
            return BAD_VALUE;
        }
        
        mInputBuffer = inputBuffer;
        
        return OK;
    }
    
    SInt32 X264VideoEncoder::processOutput(BaseBuffer* outputBuffer)
    {
        if (mState != STARTED){
            //LogPrintf_Error(DEBUG_TAG,__LINE__,"[processOutput]: exit fail with mState != STARTED\n");
            return INVALID_OPERATION;
        }
        
        if (mFlush){
            if (!X264_FlushDelayedFrames(outputBuffer)){
                // do not call inline
                //LogPrintf_Error(DEBUG_TAG,__LINE__,"[processOutput]: exit fail with X264_FlushDelayedFrames() failed\n");
                //printf("[processOutput]:no more delayed frames in encoder!\n");
                mFlush = false;
                return NOT_ENOUGH_DATA;
            }
        }
        else if (mInputBuffer != NULL){
            if (!X264_EncodeVideo(mInputBuffer, outputBuffer)){
                ////LogPrintf_Error(DEBUG_TAG,__LINE__,"[processOutput]: exit fail with X264_EncodeVideo() failed\n");
                return UNKNOWN_ERROR;
            }
            mInputBuffer = NULL;
        }else{
            //LogPrintf_Error(DEBUG_TAG,__LINE__,"[processOutput]: exit fail with mInputBuffer == NULL\n");
            //printf("[processOutput]: no input data, need to call processInput!\n");
            return INVALID_OPERATION;
        }
        return OK;
    }
    
    SInt32 X264VideoEncoder::stop()
    {
        if (mState != STARTED)
        {
            //LogPrintf_Error(DEBUG_TAG,__LINE__,"[stop]: exit fail with mState != STARTED\n");
            return INVALID_OPERATION;
        }
        
        X264Destroy();
        x264_picture_clean(mX264Pic);
        
        mFlush = false;
        mState = STOPPED;
        
        mIsFirstPicAllocFlag = false;
        return OK;
    }
    
    SInt32 X264VideoEncoder::X264_GetBitrate()
    {
        return mX264Param->rc.i_bitrate;
    }
    
    Void X264VideoEncoder::X264_EncodeHeader()
    {
        x264_nal_t *nal = NULL;
        int i_nal = 0;
        uint8_t* pps;
        uint8_t* sps;
        int ppsLength = 0;
        int spsLength = 0;
        
        x264_encoder_headers(mX264, &nal, &i_nal);
        
        bool haveSps = false;
        bool havePps = false;
        for (int i = 0; i < i_nal; i++)
        {
            if (nal[i].i_type == 7)  //sps
            {
                haveSps = true;
                sps = nal[i].p_payload;//sps = nal[i].p_payload + 4;
                spsLength = nal[i].i_payload;//iSPSLen = nal[i].i_payload - 4;
            }
            else if (nal[i].i_type == 8) //pps
            {
                havePps = true;
                pps = nal[i].p_payload;//pps = nal[i].p_payload + 4;
                ppsLength = nal[i].i_payload;//iPPSLen = nal[i].i_payload - 4;
            }
            else
            {
                continue;
            }
        }
        
        if (!haveSps || !havePps) {
            //LogPrintf_Error(DEBUG_TAG,__LINE__,"[X264_EncodeHeader]: no sps or pps \n");
            return;
        }
        
        mDataSPS = new UInt8[spsLength];
        memcpy(&mDataSPS[0], sps, spsLength);
        mDataSPSLength = spsLength;
        
        mDataPPS = new UInt8[ppsLength];
        memcpy(&mDataPPS[0], pps, ppsLength);
        mDataPPSLength = ppsLength;
        
        // skip start code and construct AVC1 data.
        sps += 4;
        pps += 4;
        spsLength -= 4;
        ppsLength -= 4;
        mDataAVCC = new uint8_t[11 + spsLength + ppsLength];
        
        mDataAVCC[0] = 1;
        mDataAVCC[1] = sps[1];
        mDataAVCC[2] = sps[2];
        mDataAVCC[3] = sps[3];
        mDataAVCC[4] = 0xff;
        mDataAVCC[5] = 0xe1;
        
        *((uint16_t*)&mDataAVCC[6]) = SwapBE16(spsLength);
        memcpy((uint8_t*)&mDataAVCC[8], sps, spsLength);
        mDataAVCC[spsLength + 8] = 1;
        *((uint16_t*)&mDataAVCC[spsLength + 9]) = SwapBE16(ppsLength);
        memcpy(&mDataAVCC[spsLength + 11], pps, ppsLength);
        
        mDataAVCCLength = 11 + spsLength + ppsLength;
    }
    
    bool X264VideoEncoder::X264_EncodeVideo(BaseBuffer* inputBuffer, BaseBuffer* outputBuffer)
    {
        VideoBuffer* videoBuffer = (VideoBuffer*)inputBuffer->getData();
        
        SInt32 width = videoBuffer->mWidth;
        SInt32 height = videoBuffer->mHeight;
        
        assert(width == mSettings->mWidth);
        assert(height == mSettings->mHeight);
        
        bool sendHeader = false;
        
        if (!mIsFirstPicAllocFlag) {
            if (videoBuffer->mColorType == VIDEO_COLOR_FORMAT_YUV420P) {
                // Create a new pic.
                x264_picture_alloc(mX264Pic, X264_CSP_I420, mSettings->mWidth, mSettings->mHeight);
            }
            else if (videoBuffer->mColorType == VIDEO_COLOR_FORMAT_NV12) {
                // Create a new pic.
                x264_picture_alloc(mX264Pic, X264_CSP_NV12, mSettings->mWidth, mSettings->mHeight);
            }
            mIsFirstPicAllocFlag = true;
            sendHeader = true;
        }
        
        if (videoBuffer->mColorType == VIDEO_COLOR_FORMAT_YUV420P) {
            UInt8* buf = mX264Pic->img.plane[0];
            memcpy(buf, videoBuffer->mData[0], width * height);
            buf += width * height;
            memcpy(buf, videoBuffer->mData[1], width * height / 4);
            buf += width * height / 4;
            memcpy(buf, videoBuffer->mData[2], width * height / 4);
            
            mX264Pic->img.i_stride[0] = width;
            mX264Pic->img.i_stride[1] = width >> 1;
            mX264Pic->img.i_stride[2] = width >> 1;
        }
        else if (videoBuffer->mColorType == VIDEO_COLOR_FORMAT_NV12) {
            UInt8* buf = mX264Pic->img.plane[0];
            memcpy(buf, videoBuffer->mData[0], width * height);
            buf += width * height;
            memcpy(buf, videoBuffer->mData[1], width * height / 2);
            //            buf += width * height / 4;
            //            memcpy(buf, videoBuffer->mData[2], width * height / 4);
            
            mX264Pic->img.i_stride[0] = width;
            mX264Pic->img.i_stride[1] = width;
            mX264Pic->img.i_stride[2] = 0;
        }
        
        //     // ‘⁄’‚¿Ô»Áπ˚≤ª≥À“‘param.i_fps_den, ‘Ú‘⁄œ¬√Êµƒptsº∆À„÷–‘Úø…“‘≤ª≥˝param.i_fps_den.
        //     mX264Pic->i_pts = (int64_t)mX264Param->i_frame_total;
        mX264Pic->i_pts = videoBuffer->mTime;
        mX264Pic->i_dts = videoBuffer->mTime;
        mX264Param->i_frame_total++;
        
        x264_picture_t pic_out;
        x264_nal_t *nal = NULL;
        int i_nal = 0, iSize = 0;
        
        if (mX264Pic)
        {
            mX264Pic->i_type = X264_TYPE_AUTO;
            mX264Pic->i_qpplus1 = 0;
        }
        
        iSize = x264_encoder_encode(mX264, &nal, &i_nal, mX264Pic, &pic_out);
        
        outputBuffer->setLength(0);
        if (iSize <= 0)
        {
//            // delete by zhangjianping, do not send head frame. get it after starting
//            if (sendHeader) {
//                outputBuffer->setFlags(HEAD_FRAME);
//                if (mDataSPS && mDataPPS) {
//                    UInt8* outData = outputBuffer->getData();
//                    memcpy(outData, mDataSPS, mDataSPSLength);
//                    memcpy(outData + mDataSPSLength, mDataPPS, mDataPPSLength);
//                    outputBuffer->setLength(mDataSPSLength + mDataPPSLength);
//                    outputBuffer->setTimestamp(0);
//                    outputBuffer->setDts(0);
//                    return true;
//                }
//                else {
//                    outputBuffer->setLength(0);
//                    return false;
//                }
//            }
            
            if (iSize < 0) {
                //LogPrintf_Error(DEBUG_TAG,__LINE__,"[X264_EncodeVideo]: exit the size is < 0\n");
            }
            return false;
        }
        
        UInt32 encodedLength = 0;
        UInt8* outData = outputBuffer->getData();

        for (int i = 0; i < i_nal; i++)
        {
            memcpy(outData, nal[i].p_payload, nal[i].i_payload);
            outData += nal[i].i_payload;
            encodedLength += nal[i].i_payload;
        }
        outputBuffer->setLength(encodedLength);
        outputBuffer->setTimestamp(pic_out.i_pts);
        outputBuffer->setFlags(pic_out.i_type == X264_TYPE_IDR ? 2 : 0);
        outputBuffer->setDts(pic_out.i_dts);
        
        return true;
    }
    
    bool X264VideoEncoder::X264_FlushDelayedFrames(BaseBuffer* outputBuffer)
    {
        x264_nal_t *nal;
        x264_picture_t pic_out;
        int nal_num;
        //UInt32 length = 0;
        
        if (!mX264)
        {
            //LogPrintf_Error(DEBUG_TAG,__LINE__,"[X264_FlushDelayedFrames]: exit fail with mX264 invalid!\n");
            return false;
        }
        
        if (!x264_encoder_delayed_frames(mX264))
        {
            //LogPrintf_Error(DEBUG_TAG,__LINE__,"[X264_FlushDelayedFrames]: exit fail with x264_encoder_delayed_frames() failed\n");
            return false;
        }
        
        
        if (x264_encoder_encode(mX264, &nal, &nal_num, NULL, &pic_out) < 0)
        {
            //LogPrintf_Error(DEBUG_TAG,__LINE__,"[X264_FlushDelayedFrames]: exit fail with x264_encoder_encode() failed\n");
            return false;
        }
        
        
        UInt32 encodedLength = 0;
        UInt8* outData = outputBuffer->getData();
        for (int i = 0; i < nal_num; i++)
        {
            memcpy(outData, nal[i].p_payload, nal[i].i_payload);
            outData += nal[i].i_payload;
            encodedLength += nal[i].i_payload;
        }
        outputBuffer->setLength(encodedLength);
        outputBuffer->setTimestamp(pic_out.i_pts);
        outputBuffer->setFlags(pic_out.i_type == X264_TYPE_IDR ? 1 : 0);
        outputBuffer->setDts(pic_out.i_dts);
        
        return true;
    }
    
    SInt32 X264VideoEncoder::X264_GetCurrentFrames()
    {
        return mX264Param->i_frame_total;
    }
    
} // namespace MediaPlugin