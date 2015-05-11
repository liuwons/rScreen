#ifndef RS_ENCODE
#define RS_ENCODE

extern "C"
{
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#include "error.h"

namespace rs
{
	typedef void(*ProcDataCallback)(void* encoded_data, int encoded_data_len, void* pUser);
	void default_proc_data_callback(void* encoded_data, int encoded_data_len, void* pUser = 0);
	ErrCode encode_screen(int time_secs, ProcDataCallback callback = default_proc_data_callback, void* pUser = 0, AVCodecID codec_id = AV_CODEC_ID_MPEG1VIDEO);
}

#endif