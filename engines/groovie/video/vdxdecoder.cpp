
#include "graphics/surface.h"
#include "video/video_decoder.h"

namespace Video {

class VDXDecoder : public VideoDecoder {
public:
	VDXDecoder() {
	}

	virtual ~VDXDecoder() {
		close();
	}

	bool loadStream(Common::SeekableReadStream *stream) {
		close();
		return true;
	}

	void close() {
		VideoDecoder::close();
	}

	void readNextPacket() {
	}

};

VDXDecoder vdxdec;

}; // namespace Video
