/****************************************************************************/
/// @file    GUIVideoEncoder.h
/// @author  Michael Behrisch
/// @date    Dec 2015
/// @version $Id$
///
// A simple video encoder from RGBA pics to anything ffmpeg can handle.
// Tested with h264 only.
// Based on work by Lei Xiaohua, Philip Schneider and Fabrice Bellard, see
// https://github.com/leixiaohua1020/simplest_ffmpeg_video_encoder and
// https://github.com/codefromabove/FFmpegRGBAToYUV
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIVideoEncoder_h
#define GUIVideoEncoder_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <stdio.h>
#include <iostream>
#include <stdexcept>

#define __STDC_CONSTANT_MACROS

extern "C"
{
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}


class GUIVideoEncoder {
public:
    GUIVideoEncoder(const char* const out_file, const int width, const int height, double frameDelay) {
        av_register_all();
        AVFormatContext* const pFormatCtx = myFormatContext = avformat_alloc_context();

        //Guess Format
        pFormatCtx->oformat = av_guess_format(NULL, out_file, NULL);
        if (pFormatCtx->oformat == 0) {
            throw std::runtime_error("Unknown format!");
        }

        //Open output URL
        if (avio_open(&pFormatCtx->pb, out_file, AVIO_FLAG_READ_WRITE) < 0) {
            throw std::runtime_error("Failed to open output file!");
        }

        // @todo maybe warn about default and invalid framerates
        int framerate = 25;
        if (frameDelay > 0.) {
            framerate = (int)(1000. / frameDelay);
            if (framerate <= 0) {
                framerate = 1;
            }
        }
        AVStream* const video_st = avformat_new_stream(pFormatCtx, 0);
        video_st->time_base.num = 1;
        video_st->time_base.den = framerate;

        //Param that must set
        AVCodecContext* const pCodecCtx = video_st->codec;
        //pCodecCtx->codec_id =AV_CODEC_ID_HEVC;
        pCodecCtx->codec_id = pFormatCtx->oformat->video_codec;
        pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
        pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
        pCodecCtx->width = width;
        // @todo maybe warn about one missing line for odd height
        pCodecCtx->height = (height / 2) * 2;
        pCodecCtx->time_base.num = 1;
        pCodecCtx->time_base.den = framerate;
        pCodecCtx->bit_rate = 4000000;
        pCodecCtx->gop_size = 250;
        //H264
        //pCodecCtx->me_range = 16;
        //pCodecCtx->max_qdiff = 4;
        //pCodecCtx->qcompress = 0.6;
        pCodecCtx->qmin = 10;
        pCodecCtx->qmax = 51;

        //Optional Param
        pCodecCtx->max_b_frames = 3;

        // Set Option
        AVDictionary* param = 0;
        //H.264
        if (pCodecCtx->codec_id == AV_CODEC_ID_H264) {
            av_dict_set(&param, "preset", "slow", 0);
            av_dict_set(&param, "tune", "zerolatency", 0);
            //av_dict_set(&param, "profile", "main", 0);
        }
        //H.265
        if (pCodecCtx->codec_id == AV_CODEC_ID_HEVC) {
            av_dict_set(&param, "preset", "ultrafast", 0);
            av_dict_set(&param, "tune", "zero-latency", 0);
        }

        //Show some Information
        //av_dump_format(pFormatCtx, 0, out_file, 1);

        AVCodec* const pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
        if (!pCodec) {
            throw std::runtime_error("Can not find encoder!");
        }
        if (avcodec_open2(pCodecCtx, pCodec, &param) < 0) {
            throw std::runtime_error("Failed to open encoder!");
        }

        myFrame = av_frame_alloc();
        myFrame->format = pCodecCtx->pix_fmt;
        myFrame->width  = pCodecCtx->width;
        myFrame->height = pCodecCtx->height;
        av_image_alloc(myFrame->data, myFrame->linesize, pCodecCtx->width, pCodecCtx->height,
                       pCodecCtx->pix_fmt, 32);

        mySwsContext = sws_getContext(pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGBA,
                                      pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P,
                                      0, 0, 0, 0);
        //Write File Header
        avformat_write_header(pFormatCtx, NULL);
        myFrameIndex = 0;
    }

    ~GUIVideoEncoder() {
        AVFormatContext* fmt_ctx = myFormatContext;
        int ret = 1;
        int got_frame;
        AVPacket enc_pkt;
        if (!(fmt_ctx->streams[0]->codec->codec->capabilities &
                CODEC_CAP_DELAY)) {
            ret = 0;
        }
        while (ret > 0) {
            enc_pkt.data = NULL;
            enc_pkt.size = 0;
            av_init_packet(&enc_pkt);
            ret = avcodec_encode_video2(fmt_ctx->streams[0]->codec, &enc_pkt,
                                        NULL, &got_frame);
            av_frame_free(NULL);
            if (ret < 0) {
                break;
            }
            if (!got_frame) {
                ret = 0;
                break;
            }
            /* mux encoded frame */
            ret = av_write_frame(fmt_ctx, &enc_pkt);
        }

        if (ret == 0) {
            //Write file trailer
            av_write_trailer(fmt_ctx);

            //Clean
            if (fmt_ctx->streams[0]) {
                avcodec_close(fmt_ctx->streams[0]->codec);
                av_freep(&myFrame->data[0]);
                av_frame_free(&myFrame);
            }
            avio_close(fmt_ctx->pb);
            avformat_free_context(fmt_ctx);
        }
    }

    void writeFrame(uint8_t* buffer) {
        AVStream* const video_st = myFormatContext->streams[0];
        AVCodecContext* const pCodecCtx = video_st->codec;

        uint8_t* inData[1] = { buffer }; // RGBA32 have one plane
        int inLinesize[1] = { 4 * pCodecCtx->width }; // RGBA stride
        sws_scale(mySwsContext, inData, inLinesize, 0, pCodecCtx->height,
                  myFrame->data, myFrame->linesize);

        av_init_packet(&myPkt);
        myPkt.data = NULL;
        myPkt.size = 0;
        //PTS
        myFrame->pts = myFrameIndex;
        int got_picture = 0;
        //Encode
        int ret = avcodec_encode_video2(pCodecCtx, &myPkt, myFrame, &got_picture);
        if (ret < 0) {
            throw std::runtime_error("Failed to encode!");
        }
        if (got_picture == 1) {
            myPkt.stream_index = video_st->index;
            ret = av_write_frame(myFormatContext, &myPkt);
            av_free_packet(&myPkt);
            myFrameIndex++;
        }
    }

private:
    AVFormatContext* myFormatContext;
    SwsContext* mySwsContext;
    AVFrame* myFrame;
    AVPacket myPkt;
    int myFrameIndex;

};


#endif

/****************************************************************************/
