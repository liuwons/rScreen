#include "utils.h"
#include "error.h"
#include "encode.h"

#include <windows.h>


namespace rs
{
	const char default_file_name[] = "screen.avi";

	void default_proc_data_callback(void* encoded_data, int encoded_data_len, void* pUser)
	{
		FILE* f;
		fopen_s(&f, default_file_name, "ab");
		if (f == NULL)
		{
			fprintf(stderr, "open file %s failed!\n", default_file_name);
			exit(0);
		}
		fwrite(encoded_data, 1, encoded_data_len, f);
		fclose(f);
	}

	ErrCode encode_screen(int time_secs, ProcDataCallback callback, void* pUser, AVCodecID codec_id)
	{
		avcodec_register_all();

		int screen_width = get_screen_width();
		int screen_height = get_screen_height();

		printf("screen:width[%d], height[%d]\n", screen_width, screen_height);
		
		unsigned char* buf = new unsigned char[screen_width * screen_height * 4];

		AVCodec *codec;
		AVCodecContext *codec_context = 0;
		int ret, got_output;
		AVFrame* rgb_frame;
		AVFrame* yuv_frame;
		AVPacket pkt;
		uint8_t endcode[] = { 0, 0, 1, 0xb7 };
		SwsContext* sws_context;

		codec = avcodec_find_encoder(codec_id);
		if (!codec)
			return ERR_CODEC_NOT_FOUND;

		codec_context = avcodec_alloc_context3(codec);
		if (!codec_context)
			ERR_COULD_NOT_ALLOCATE_VIDEO_CODEC_CONTEXT;

		codec_context->bit_rate = 400000;
		codec_context->width = screen_width;
		codec_context->height = screen_height;
		AVRational ar = { 1, 25 };
		codec_context->time_base = ar;
		codec_context->gop_size = 10;
		codec_context->max_b_frames = 1;
		codec_context->pix_fmt = AV_PIX_FMT_YUV420P;

		if (codec_id == AV_CODEC_ID_H264)
			av_opt_set(codec_context->priv_data, "preset", "slow", 0);

		if (avcodec_open2(codec_context, codec, NULL) < 0)
			return ERR_COULD_NOT_OPEN_CODEC;

		rgb_frame = av_frame_alloc();
		yuv_frame = av_frame_alloc();
		if (!rgb_frame || !yuv_frame)
			return ERR_COULD_NOT_ALLOCATE_VIDEO_FRAME;

		yuv_frame->format = codec_context->pix_fmt;
		yuv_frame->width = codec_context->width;
		yuv_frame->height = codec_context->height;
		
		rgb_frame->format = AV_PIX_FMT_BGRA;
		rgb_frame->width = codec_context->width;
		rgb_frame->height = codec_context->height;

		sws_context = sws_getContext(codec_context->width, codec_context->height, PIX_FMT_BGRA, codec_context->width, codec_context->height, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

		ret = av_image_alloc(yuv_frame->data, yuv_frame->linesize, codec_context->width, codec_context->height, codec_context->pix_fmt, 32);
		if (ret < 0)
			return ERR_COULD_NOT_ALLOCATE_RAW_VIDEO_BUFFER;

		int frame_id = 0;

		int frame_period = 40;
		int frame_total = 25 * time_secs;
		DWORD start_time = GetTickCount();
		int last_encode_time = 0;

		while (1)
		{
			if (frame_id >= frame_total)
				break;

			int current_time = GetTickCount() - start_time;
			if (current_time - last_encode_time < frame_period)
				Sleep(frame_period - (current_time - last_encode_time));
			current_time = GetTickCount() - start_time;
			last_encode_time = current_time;

			ScreenCap(buf);

			avpicture_fill((AVPicture*)rgb_frame, (uint8_t*)buf, AV_PIX_FMT_BGRA, codec_context->width, codec_context->height); 

			av_init_packet(&pkt);
			pkt.data = NULL;
			pkt.size = 0;

			sws_scale(sws_context, rgb_frame->data, rgb_frame->linesize, 0, codec_context->height, yuv_frame->data, yuv_frame->linesize);

			yuv_frame->pts = frame_id;
			frame_id++;

			ret = avcodec_encode_video2(codec_context, &pkt, yuv_frame, &got_output);
			if (ret < 0)
				return ERR_ENCODE_FRAME;

			if (got_output)
			{
				callback(pkt.data, pkt.size, pUser);
				av_free_packet(&pkt);
			}
		}

		for (got_output = 1; got_output; frame_id++)
		{
			ret = avcodec_encode_video2(codec_context, &pkt, NULL, &got_output);
			if (ret < 0)
				return ERR_ENCODE_FRAME;

			if (got_output)
			{
				callback(pkt.data, pkt.size, pUser);
				av_free_packet(&pkt);
			}
		}

		callback(endcode, sizeof(endcode), pUser);

		avcodec_close(codec_context);
		av_free(codec_context);
		av_freep(&yuv_frame->data[0]);
		av_frame_free(&yuv_frame);
		av_free(&rgb_frame->data[0]);
		av_frame_free(&yuv_frame);

		return ERR_NO;
	}
}
