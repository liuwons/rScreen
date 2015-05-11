#include <windows.h>
#include <stdio.h>
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

#include "utils.h"

int encode_screen(const char *filename, AVCodecID codec_id)
{
    char* buf = new char[1920 * 1080 * 3];
    AVCodec *codec;
    AVCodecContext *codex_context = NULL;
    int i, ret, x, y, got_output;
    FILE *f;
    AVFrame* rgb_frame;
    AVFrame* yuv_frame;
    AVPacket pkt;
    uint8_t endcode[] = { 0, 0, 1, 0xb7 };

    codec = avcodec_find_encoder(codec_id);
    if (!codec)
    {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }
    else
    {
        printf("find encoder succeed\n");
    }

    codex_context = avcodec_alloc_context3(codec);
    if (!codex_context)
    {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }
    else
    {
        printf("allocate video codec context succeed\n");
    }

    codex_context->bit_rate = 400000;

    codex_context->width = 1920;
    codex_context->height = 1080;
    AVRational ar = { 1, 25 };
    codex_context->time_base = ar;
    codex_context->gop_size = 10;
    codex_context->max_b_frames = 1;
    codex_context->pix_fmt = AV_PIX_FMT_YUV420P;

    if (codec_id == AV_CODEC_ID_H264)
        av_opt_set(codex_context->priv_data, "preset", "slow", 0);

    if (avcodec_open2(codex_context, codec, NULL) < 0)
    {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }
    else
    {
        printf("open codec succeed\n");
    }

    f = fopen(filename, "wb");
    if (!f)
    {
        fprintf(stderr, "Could not open %s\n", filename);
        exit(1);
    }
    printf("open file %s succeed\n", filename);

    rgb_frame = av_frame_alloc();
    yuv_frame = av_frame_alloc();
    if (!rgb_frame || ! yuv_frame)
    {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    else
    {
        printf("allocate video frame succeed\n");
    }

    yuv_frame->format = codex_context->pix_fmt;
    yuv_frame->width = codex_context->width;
    yuv_frame->height = codex_context->height;

    rgb_frame->format = AV_PIX_FMT_RGB24;
    rgb_frame->width = codex_context->width;
    rgb_frame->height = codex_context->height;


    ret = av_image_alloc(yuv_frame->data, yuv_frame->linesize, codex_context->width, codex_context->height, codex_context->pix_fmt, 32);
    //ret = av_image_alloc(rgb_frame->data, rgb_frame->linesize, codex_context->width, codex_context->height, AV_PIX_FMT_RGB24, 32);

    if (ret < 0) 
    {
        fprintf(stderr, "Could not allocate raw picture buffer\n");
        exit(1);
    }
    else
    {
        printf("allocate raw picture buffer succeed\n");
    }

    SwsContext* sws_context = sws_getContext(1920, 1080, PIX_FMT_RGB24, codex_context->width, codex_context->height, PIX_FMT_YUV420P, SWS_FAST_BILINEAR, NULL, NULL, NULL);
    printf("get sws context succeed\n");

    for (i = 0; i < 10000; i++) 
    {
        int w, h;
        ScreenCap(buf, w, h);
        printf("screen cap succeed\n");

        av_init_packet(&pkt);
        pkt.data = NULL;    // packet data will be allocated by the encoder
        pkt.size = 0;

        fflush(stdout);

        avpicture_fill((AVPicture*)rgb_frame, (uint8_t*)buf, AV_PIX_FMT_RGB24, w, h);
        printf("fill picture rgb_frame succeed\n");
        yuv_frame->pts = i;

        sws_scale(sws_context, rgb_frame->data, rgb_frame->linesize, 0, codex_context->height, yuv_frame->data, yuv_frame->linesize);          // converting frame size and format

        printf("sws scale succeed\n");

        ret = avcodec_encode_video2(codex_context, &pkt, yuv_frame, &got_output);
        if (ret < 0)
        {
            fprintf(stderr, "Error encoding frame\n");
            exit(1);
        }

        if (got_output)
        {
            printf("Write frame %3d (size=%5d)\n", i, pkt.size);
            fwrite(pkt.data, 1, pkt.size, f);
            av_free_packet(&pkt);
        }
    }

    for (got_output = 1; got_output; i++)
    {
        fflush(stdout);

        ret = avcodec_encode_video2(codex_context, &pkt, NULL, &got_output);
        if (ret < 0)
        {
            fprintf(stderr, "Error encoding frame\n");
            exit(1);
        }

        if (got_output)
        {
            printf("Write frame %3d (size=%5d)\n", i, pkt.size);
            fwrite(pkt.data, 1, pkt.size, f);
            av_free_packet(&pkt);
        }
    }

    fwrite(endcode, 1, sizeof(endcode), f);
    fclose(f);

    avcodec_close(codex_context);
    av_free(codex_context);
    av_freep(&yuv_frame->data[0]);
    av_frame_free(&yuv_frame);
    printf("\n");
}

int main()
{
    avcodec_register_all();

    printf("avcodec register all succeed");
    encode_screen("test.mp4", AV_CODEC_ID_MPEG2VIDEO);

    return 0;
}