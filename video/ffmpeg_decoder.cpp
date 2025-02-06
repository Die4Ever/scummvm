/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#include "common/endian.h"
#include "common/rect.h"
#include "common/stream.h"
#include "common/system.h"
#include "common/textconsole.h"
#include "graphics/surface.h"
#include "video/ffmpeg_decoder.h"

namespace Video {

FfmpegDecoder::FfmpegDecoder() {
}

FfmpegDecoder::~FfmpegDecoder() {
	close();
	//av_packet_unref(&_packet);
	avformat_close_input(&_formatContext);
}

bool FfmpegDecoder::loadFile(const Common::Path &filename) {
	close();

	_formatContext = nullptr;
	Common::String string_filename = filename.toString();
	const char *sfilename = string_filename.c_str();

	if (avformat_open_input(&_formatContext, sfilename, nullptr, nullptr) != 0) {
		error("Could not open file %s\n", sfilename);
		return false;
	}

	if (avformat_find_stream_info(_formatContext, nullptr) < 0) {
		error("Could not find stream information\n");
		return false;
	}

	setEndTime(Audio::Timestamp(_formatContext->duration, 30));

	// Print detailed information about the file
	//av_dump_format(_formatContext, 0, sfilename, 0);

	int videoStreamIndex = -1;
	for (int i = 0; i < _formatContext->nb_streams; i++) {
		if (_formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoStreamIndex = i;
			break;
		}
	}

	if (videoStreamIndex == -1) {
		error("No video stream found\n");
		avformat_close_input(&_formatContext);
		return false;
	}

	AVStream *videoStream = _formatContext->streams[videoStreamIndex];
	addTrack(new FfmpegVideoTrack(videoStream));
	return true;
}

bool FfmpegDecoder::loadStream(Common::SeekableReadStream *stream) {
	// TODO
	return loadFile("yay.mp4");
}

void FfmpegDecoder::readNextPacket() {
	AVPacket _packet;

	while (av_read_frame(_formatContext, &_packet) >= 0) {
		if (_packet.stream_index == ((FfmpegVideoTrack *)_nextVideoTrack)->getIndex()) {
			((FfmpegVideoTrack *)_nextVideoTrack)->sendPacket(_packet);
			av_packet_unref(&_packet);
			break;
		}
		av_packet_unref(&_packet);
	}
}


FfmpegDecoder::FfmpegVideoTrack::FfmpegVideoTrack(AVStream *videoStream) {
	_videoStream = videoStream;
	_curFrame = 0;

	AVCodecParameters *codecParams = videoStream->codecpar;

	const AVCodec *codec = avcodec_find_decoder(codecParams->codec_id);
	if (codec == nullptr) {
		error("Codec not found\n");
		return;
	}

	_codecContext = avcodec_alloc_context3(codec);
	if (avcodec_parameters_to_context(_codecContext, codecParams) < 0) {
		error("Failed to copy codec parameters to codec context\n");
		return;
	}

	if (avcodec_open2(_codecContext, codec, nullptr) < 0) {
		error("Failed to open codec\n");
		avcodec_free_context(&_codecContext);
		return;
	}
		
	_frame = av_frame_alloc();

	_surface = new Graphics::Surface();
	_surface->create(_codecContext->width, _codecContext->height, Graphics::PixelFormat::PixelFormat(4, 0, 0, 0, 0, 24, 16, 8, 0));
	//_surface->create(640, 480, Graphics::PixelFormat::PixelFormat(4, 0, 0, 0, 0, 24, 16, 8, 0));
	_rgbFrame = av_frame_alloc();
	_rgbFrame->linesize[0] = _surface->w * 3;

	_swsContext = sws_getContext(_codecContext->width, _codecContext->height, _codecContext->pix_fmt,
				_surface->w, _surface->h, AV_PIX_FMT_RGBA, SWS_POINT,
				nullptr, nullptr, nullptr);
}

FfmpegDecoder::FfmpegVideoTrack::~FfmpegVideoTrack() {
	_surface->free();
	delete _surface;

	av_frame_free(&_frame);
	av_frame_free(&_rgbFrame);
	sws_freeContext(_swsContext);
	avcodec_free_context(&_codecContext);
}

int FfmpegDecoder::FfmpegVideoTrack::getIndex() const {
	return _videoStream->index;
}

void FfmpegDecoder::FfmpegVideoTrack::sendPacket(AVPacket &packet) {
	if (avcodec_send_packet(_codecContext, &packet) < 0) {
		error("Error sending packet to the decoder\n");
		return;
	}
}

const Graphics::Surface *FfmpegDecoder::FfmpegVideoTrack::decodeNextFrame() {
	while (avcodec_receive_frame(_codecContext, _frame) >= 0) {
		_rgbFrame->data[0] = (byte *)_surface->getPixels();

		sws_scale(_swsContext, _frame->data, _frame->linesize, 0, _codecContext->height,
				  _rgbFrame->data, _rgbFrame->linesize);
	}

	_nextFrameStartTime = _nextFrameStartTime.addFrames(33);

	return _surface;
}

Graphics::PixelFormat FfmpegDecoder::FfmpegVideoTrack::getPixelFormat() const {
	return _surface->format;
}

} // End of namespace Video
