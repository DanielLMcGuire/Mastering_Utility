/**
 * @file MediaManager.cpp
 * @brief Implementation of the Mastering Utility's FFMPEG Helpers
 * @author Daniel McGuire
 */ 

// Copyright 2025 Daniel McGuire
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// !!!!!!!!!!!!!!!!!!!!!!!!
// THIS CODE IS NOT TESTED!
// !!!!!!!!!!!!!!!!!!!!!!!!

#include "MediaManager.h"

bool MediaManager::DecodeAudio(const std::filesystem::path& path, Song& song)
{
	AVFormatContext* fmt = nullptr;
	if (avformat_open_input(&fmt, path.string().c_str(), nullptr, nullptr) < 0)
		return false;

	if (avformat_find_stream_info(fmt, nullptr) < 0)
		return false;

	int streamIndex = av_find_best_stream(fmt, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
	if (streamIndex < 0)
		return false;

	AVStream* stream = fmt->streams[streamIndex];
	AVCodec* codec = avcodec_find_decoder(stream->codecpar->codec_id);
	if (!codec)
		return false;

	AVCodecContext* ctx = avcodec_alloc_context3(codec);
	avcodec_parameters_to_context(ctx, stream->codecpar);
	if (avcodec_open2(ctx, codec, nullptr) < 0)
		return false;

	AVPacket* pkt = av_packet_alloc();
	AVFrame* frame = av_frame_alloc();

	while (av_read_frame(fmt, pkt) >= 0) {
		if (pkt->stream_index != streamIndex) {
			av_packet_unref(pkt);
			continue;
		}
		if (avcodec_send_packet(ctx, pkt) < 0)
			break;

		while (avcodec_receive_frame(ctx, frame) == 0) {
			int dataSize = av_get_bytes_per_sample(ctx->sample_fmt) * frame->nb_samples * ctx->channels;
			size_t oldSize = song.AudioData.size();
			song.AudioData.resize(oldSize + dataSize);
			memcpy(song.AudioData.data() + oldSize, frame->data[0], dataSize);
		}
		av_packet_unref(pkt);
	}

	song.SampleRate = ctx->sample_rate;
	song.Channels = ctx->channels;
	song.SampleFormat = ctx->sample_fmt;
	song.Codec = codec->name;

	av_frame_free(&frame);
	av_packet_free(&pkt);
	avcodec_free_context(&ctx);
	avformat_close_input(&fmt);
	return true;
}

bool MediaManager::EncodeAudio(const std::filesystem::path& path, const Song& song, const std::string& codecName)
{
	const AVCodec* codec = avcodec_find_encoder_by_name(codecName.c_str());
	if (!codec)
		return false;

	AVCodecContext* ctx = avcodec_alloc_context3(codec);
	ctx->sample_rate = song.SampleRate;
	ctx->channels = song.Channels;
	ctx->channel_layout = av_get_default_channel_layout(ctx->channels);
	ctx->sample_fmt = codec->sample_fmts[0];
	ctx->time_base = {1, ctx->sample_rate};

	AVFormatContext* fmt = nullptr;
	avformat_alloc_output_context2(&fmt, nullptr, nullptr, path.string().c_str());
	if (!fmt) return false;

	AVStream* stream = avformat_new_stream(fmt, codec);
	avcodec_open2(ctx, codec, nullptr);
	avcodec_parameters_from_context(stream->codecpar, ctx);

	if (!(fmt->oformat->flags & AVFMT_NOFILE))
		avio_open(&fmt->pb, path.string().c_str(), AVIO_FLAG_WRITE);

	avformat_write_header(fmt, nullptr);

	size_t offset = 0;
	int frameSize = ctx->frame_size * ctx->channels * av_get_bytes_per_sample(ctx->sample_fmt);

	while (offset + frameSize <= song.AudioData.size()) {
		AVFrame* frame = av_frame_alloc();
		frame->nb_samples = ctx->frame_size;
		frame->format = ctx->sample_fmt;
		frame->channel_layout = ctx->channel_layout;
		av_frame_get_buffer(frame, 0);
		memcpy(frame->data[0], song.AudioData.data() + offset, frameSize);
		offset += frameSize;

		avcodec_send_frame(ctx, frame);
		AVPacket pkt;
		av_init_packet(&pkt);
		while (avcodec_receive_packet(ctx, &pkt) == 0) {
			av_write_frame(fmt, &pkt);
			av_packet_unref(&pkt);
		}
		av_frame_free(&frame);
	}

	av_write_trailer(fmt);
	if (!(fmt->oformat->flags & AVFMT_NOFILE))
		avio_closep(&fmt->pb);

	avcodec_free_context(&ctx);
	avformat_free_context(fmt);
	return true;
}

bool MediaManager::DecodeImage(const std::filesystem::path& path, Album& album)
{
	AVFormatContext* fmt = nullptr;
	if (avformat_open_input(&fmt, path.string().c_str(), nullptr, nullptr) < 0)
		return false;

	int streamIndex = av_find_best_stream(fmt, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
	if (streamIndex < 0)
		return false;

	AVStream* stream = fmt->streams[streamIndex];
	AVCodec* codec = avcodec_find_decoder(stream->codecpar->codec_id);
	if (!codec)
		return false;

	AVCodecContext* ctx = avcodec_alloc_context3(codec);
	avcodec_parameters_to_context(ctx, stream->codecpar);
	if (avcodec_open2(ctx, codec, nullptr) < 0)
		return false;

	AVPacket* pkt = av_packet_alloc();
	AVFrame* frame = av_frame_alloc();

	while (av_read_frame(fmt, pkt) >= 0) {
		if (pkt->stream_index != streamIndex) {
			av_packet_unref(pkt);
			continue;
		}
		if (avcodec_send_packet(ctx, pkt) < 0)
			break;

		while (avcodec_receive_frame(ctx, frame) == 0) {
			album.Width = frame->width;
			album.Height = frame->height;
			album.Channels = 3;

			int size = av_image_get_buffer_size(AV_PIX_FMT_RGB24, frame->width, frame->height, 1);
			album.AlbumArtData.resize(size);

			SwsContext* sws = sws_getContext(
				frame->width, frame->height, (AVPixelFormat)frame->format,
				frame->width, frame->height, AV_PIX_FMT_RGB24,
				SWS_BILINEAR, nullptr, nullptr, nullptr
			);

			uint8_t* dest[4] = { album.AlbumArtData.data(), nullptr, nullptr, nullptr };
			int destLinesize[4] = { 3 * frame->width, 0, 0, 0 };
			sws_scale(sws, frame->data, frame->linesize, 0, frame->height, dest, destLinesize);
			sws_freeContext(sws);
			break;
		}
		av_packet_unref(pkt);
	}

	av_frame_free(&frame);
	av_packet_free(&pkt);
	avcodec_free_context(&ctx);
	avformat_close_input(&fmt);
	return true;
}

bool MediaManager::EncodeImage(const std::filesystem::path& path, const Album& album, const std::string& format)
{
	const AVCodec* codec = avcodec_find_encoder_by_name(format.c_str());
	if (!codec) return false;

	AVCodecContext* ctx = avcodec_alloc_context3(codec);
	ctx->width = album.Width;
	ctx->height = album.Height;
	ctx->pix_fmt = AV_PIX_FMT_RGB24;
	ctx->time_base = {1, 1};

	AVFormatContext* fmt = nullptr;
	avformat_alloc_output_context2(&fmt, nullptr, nullptr, path.string().c_str());
	if (!fmt) return false;

	AVStream* stream = avformat_new_stream(fmt, codec);
	avcodec_open2(ctx, codec, nullptr);
	avcodec_parameters_from_context(stream->codecpar, ctx);

	if (!(fmt->oformat->flags & AVFMT_NOFILE))
		avio_open(&fmt->pb, path.string().c_str(), AVIO_FLAG_WRITE);

	avformat_write_header(fmt, nullptr);

	AVFrame* frame = av_frame_alloc();
	frame->format = ctx->pix_fmt;
	frame->width = ctx->width;
	frame->height = ctx->height;
	av_frame_get_buffer(frame, 0);

	memcpy(frame->data[0], album.AlbumArtData.data(), album.AlbumArtData.size());

	avcodec_send_frame(ctx, frame);
	AVPacket pkt;
	av_init_packet(&pkt);
	while (avcodec_receive_packet(ctx, &pkt) == 0) {
		av_write_frame(fmt, &pkt);
		av_packet_unref(&pkt);
	}
	av_write_trailer(fmt);

	if (!(fmt->oformat->flags & AVFMT_NOFILE))
		avio_closep(&fmt->pb);
	av_frame_free(&frame);
	avcodec_free_context(&ctx);
	avformat_free_context(fmt);
	return true;
}
