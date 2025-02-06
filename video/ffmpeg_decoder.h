#pragma once
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

#ifndef VIDEO_FFMPEGDECODER_H
#define VIDEO_FFMPEGDECODER_H

#include "video/video_decoder.h"

namespace Common {
class SeekableReadStream;
}

namespace Graphics {
struct PixelFormat;
struct Surface;
} // namespace Graphics

struct AVFormatContext;
struct AVFrame;
struct AVCodecContext;
struct SwsContext;
struct AVStream;
struct AVPacket;

namespace Video {

/**
 * Decoder for wrapping FFMPEG.
 */

class FfmpegDecoder : public VideoDecoder {
public:
	FfmpegDecoder();
	virtual ~FfmpegDecoder();

	virtual bool loadFile(const Common::Path &filename);
	virtual bool loadStream(Common::SeekableReadStream *stream);

	virtual void readNextPacket();
	virtual bool hasFramesLeft() const { return true; }

protected:
	AVFormatContext *_formatContext;

	class FfmpegVideoTrack : public VideoTrack {
	public:
		FfmpegVideoTrack(AVStream *videoStream);
		~FfmpegVideoTrack();
		void sendPacket(AVPacket &packet);
		virtual const Graphics::Surface *decodeNextFrame();
		int getCurFrame() const { return _curFrame; }

		uint16 getWidth() const { return _surface->w; }
		uint16 getHeight() const { return _surface->h; }

		Graphics::PixelFormat getPixelFormat() const;
		virtual uint32 getNextFrameStartTime() const { return 0; }
		virtual bool endOfTrack() const { return false; }
		//int getFrameCount() const { return _frameCount; }
		int getIndex() const;

	protected:		
		Graphics::Surface *_surface;
		AVFrame *_frame, *_rgbFrame;
		AVStream *_videoStream;
		AVCodecContext *_codecContext;
		SwsContext *_swsContext;

		int _curFrame;
		Audio::Timestamp _nextFrameStartTime;
	};
};

} // End of namespace Video

#endif
