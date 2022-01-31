
#include "graphics/surface.h"
#include "video/video_decoder.h"
#include "common/stream.h"
#include "groovie/lzss.h"
#include "common/debug.h"
#include "common/debug-channels.h"
#include "common/textconsole.h"
#include "groovie/groovie.h"

namespace Groovie {

const uint TILE_SIZE = 4;      // Size of each tile on the image: only ever seen 4 so far
const uint VDX_IDENT = 0x9267; // 37479

static const uint16 vdxBlockMapLookup[] = {
	0xc800, 0xec80, 0xfec8, 0xffec, 0xfffe, 0x3100, 0x7310, 0xf731,
	0xff73, 0xfff7, 0x6c80, 0x36c8, 0x136c, 0x6310, 0xc631, 0x8c63,
	0xf000, 0xff00, 0xfff0, 0x1111, 0x3333, 0x7777, 0x6666, 0xcccc,
	0x0ff0, 0x00ff, 0xffcc, 0x0076, 0xff33, 0x0ee6, 0xccff, 0x6770,
	0x33ff, 0x6ee0, 0x4800, 0x2480, 0x1248, 0x0024, 0x0012, 0x2100,
	0x4210, 0x8421, 0x0042, 0x0084, 0xf888, 0x0044, 0x0032, 0x111f,
	0x22e0, 0x4c00, 0x888f, 0x4470, 0x2300, 0xf111, 0x0e22, 0x00c4,
	0xf33f, 0xfccf, 0xff99, 0x99ff, 0x4444, 0x2222, 0xccee, 0x7733,
	0x00f8, 0x00f1, 0x00bb, 0x0cdd, 0x0f0f, 0x0f88, 0x13f1, 0x19b3,
	0x1f80, 0x226f, 0x27ec, 0x3077, 0x3267, 0x37e4, 0x38e3, 0x3f90,
	0x44cf, 0x4cd9, 0x4c99, 0x5555, 0x603f, 0x6077, 0x6237, 0x64c9,
	0x64cd, 0x6cd9, 0x70ef, 0x0f00, 0x00f0, 0x0000, 0x4444, 0x2222
};

class VDXDecoder : public Video::VideoDecoder {
public:
	Common::SeekableReadStream *_file;
	uint8 _palBuf[3 * 256];
	Graphics::Surface *_surface;

	VDXDecoder() {
	}

	virtual ~VDXDecoder() {
		close();
	}

	bool loadStream(Common::SeekableReadStream *stream) {
		//close();
		_file = stream;
		return true;
	}

	void close() {
		Video::VideoDecoder::close();
	}

	void readNextPacket() {
		byte currRes = 0x80;
		Common::ReadStream *vdxData = nullptr;
		currRes = _file->readByte();

		// Skip unknown data: 1 byte, ref Edward
		byte tmp = _file->readByte();

		uint32 compSize = _file->readUint32LE();
		uint8 lengthmask = _file->readByte();
		uint8 lengthbits = _file->readByte();

		if (_file->eos())
			return;

		debugC(5, kDebugVideo | kDebugUnknown, "Groovie::VDX: Edward = 0x%04X", tmp);

		// Read the chunk data and decompress if needed
		if (compSize)
			vdxData = _file->readStream(compSize);

		if (lengthmask && lengthbits) {
			Common::ReadStream *decompData = new LzssReadStream(vdxData, lengthmask, lengthbits);
			delete vdxData;
			vdxData = decompData;
		}

		// Use the current chunk
		switch (currRes) {
		case 0x00:
			debugC(6, kDebugVideo, "Groovie::VDX: Replay frame");
			break;
		case 0x20:
			debugC(5, kDebugVideo, "Groovie::VDX: Still frame");
			getStill(vdxData);
			break;
		case 0x25:
			debugC(5, kDebugVideo, "Groovie::VDX: Animation frame");
			getDelta(vdxData);
			break;
		case 0x80:
			debugC(5, kDebugVideo, "Groovie::VDX: Sound resource");
			chunkSound(vdxData);
			break;
		default:
			error("Groovie::VDX: Invalid resource type: %d", currRes);
		}
		delete vdxData;
		vdxData = nullptr;
	}

	void getDelta(Common::ReadStream *in) {
		uint16 k, l;

		// Get the size of the local palette
		uint16 palSize = in->readUint16LE();

		// Load the palette if it isn't empty
		if (palSize) {
			uint16 palBitField[16];

			// Load the bit field
			for (l = 0; l < 16; l++) {
				palBitField[l] = in->readUint16LE();
			}

			// Load the actual palette
			for (l = 0; l < 16; l++) {
				int flag = 1 << 15;
				for (uint16 j = 0; j < 16; j++) {
					int palIndex = (l * 16) + j;

					if (flag & palBitField[l]) {
						for (k = 0; k < 3; k++) {
							_palBuf[(palIndex * 3) + k] = in->readByte();
						}
					}
					flag = flag >> 1;
				}
			}
		}

		uint8 currOpCode = in->readByte();
		uint8 param1, param2, param3;

		uint16 currentLine = 0;
		uint32 offset = 0;
		while (!in->eos()) {
			byte colors[16];
			if (currOpCode < 0x60) {
				param1 = in->readByte();
				param2 = in->readByte();
				expandColorMap(colors, vdxBlockMapLookup[currOpCode], param1, param2);
				decodeBlockDelta(offset, colors, 640);
				offset += TILE_SIZE;
			} else if (currOpCode > 0x7f) {
				param1 = in->readByte();
				param2 = in->readByte();
				param3 = in->readByte();
				expandColorMap(colors, (param1 << 8) + currOpCode, param2, param3);
				decodeBlockDelta(offset, colors, 640);
				offset += TILE_SIZE;
			} else
				switch (currOpCode) {
				case 0x60: /* Fill tile with the 16 colors given as parameters */
					for (l = 0; l < 16; l++) {
						colors[l] = in->readByte();
					}
					decodeBlockDelta(offset, colors, 640);
					offset += TILE_SIZE;
					break;
				case 0x61: /* Skip to the end of this line, next block is start of next */
					/* Note this is used at the end of EVERY line */
					currentLine++;
					offset = currentLine * TILE_SIZE * 640;
					break;
				case 0x62:
				case 0x63:
				case 0x64:
				case 0x65:
				case 0x66:
				case 0x67:
				case 0x68:
				case 0x69:
				case 0x6a:
				case 0x6b: /* Skip next param1 blocks (within line) */
					offset += (currOpCode - 0x62) * TILE_SIZE;
					break;
				case 0x6c:
				case 0x6d:
				case 0x6e:
				case 0x6f:
				case 0x70:
				case 0x71:
				case 0x72:
				case 0x73:
				case 0x74:
				case 0x75: /* Next param1 blocks are filled with color param2 */
					param1 = currOpCode - 0x6b;
					param2 = in->readByte();
					for (l = 0; l < 16; l++) {
						colors[l] = param2;
					}
					for (k = 0; k < param1; k++) {
						decodeBlockDelta(offset, colors, 640);
						offset += TILE_SIZE;
					}
					break;
				case 0x76:
				case 0x77:
				case 0x78:
				case 0x79:
				case 0x7a:
				case 0x7b:
				case 0x7c:
				case 0x7d:
				case 0x7e:
				case 0x7f: /* Next bytes contain colors to fill the next param1 blocks in the current line*/
					param1 = currOpCode - 0x75;
					for (k = 0; k < param1; k++) {
						param2 = in->readByte();
						for (l = 0; l < 16; l++) {
							colors[l] = param2;
						}
						decodeBlockDelta(offset, colors, 640);
						offset += TILE_SIZE;
					}
					break;
				default:
					error("Groovie::VDX: Broken somehow");
				}
			currOpCode = in->readByte();
		}
	}

	void getStill(Common::ReadStream *in) {
		uint16 numXTiles = in->readUint16LE();
		debugC(5, kDebugVideo, "Groovie::VDX: numXTiles=%d", numXTiles);
		uint16 numYTiles = in->readUint16LE();
		debugC(5, kDebugVideo, "Groovie::VDX: numYTiles=%d", numYTiles);

		// It's skipped in the original:
		uint16 colorDepth = in->readUint16LE();
		debugC(5, kDebugVideo, "Groovie::VDX: colorDepth=%d", colorDepth);

		uint16 imageWidth = TILE_SIZE * numXTiles;

		uint8 mask = 0;
		byte *buf = (byte *)_surface->getPixels();

		// Read the palette
		in->read(_palBuf, 3 * 256);

		byte colors[16];
		for (uint16 j = 0; j < numYTiles; j++) {
			byte *currentTile = buf + j * TILE_SIZE * imageWidth;
			for (uint16 i = numXTiles; i; i--) {
				uint8 color1 = in->readByte();
				uint8 color0 = in->readByte();
				uint16 colorMap = in->readUint16LE();
				expandColorMap(colors, colorMap, color1, color0);
				decodeBlockStill(currentTile, colors, 640, mask);

				currentTile += TILE_SIZE;
			}
		}
	}

	void expandColorMap(byte *out, uint16 colorMap, uint8 color1, uint8 color0) {
		// It's a bit faster to start from the end
		out += 16;
		for (int i = 16; i; i--) {
			// Set the corresponding color
			// The following is an optimized version of:
			// *--out = (colorMap & 1) ? color1 : color0;
			uint8 selector = -(colorMap & 1);
			*--out = (selector & color1) | (~selector & color0);

			// Update the flag map to test the next color
			colorMap >>= 1;
		}
	}

	void decodeBlockStill(byte *buf, byte *colors, uint16 imageWidth, uint8 mask) {
		assert(TILE_SIZE == 4);

		for (int y = TILE_SIZE; y; y--) {
			*((uint32 *)buf) = *((uint32 *)colors);
			colors += 4;

			// Point to the start of the next line
			buf += imageWidth;
		}
	}

	void decodeBlockDelta(uint32 offset, byte *colors, uint16 imageWidth) {
		assert(TILE_SIZE == 4);

		byte *dest;
		// TODO: Verify just the else block is required
		//if (_flagOne) {
		// Paint to the foreground
		//dest = (byte *)_fg->getPixels() + offset;
		//} else {
		dest = (byte *)_surface->getPixels() + offset;
		//}

		for (int y = TILE_SIZE; y; y--) {
			// Paint directly
			*((uint32 *)dest) = *((uint32 *)colors);
			colors += 4;

			// Move to the next line
			dest += imageWidth;
		}
	}

	void chunkSound(Common::ReadStream *in) {
		/*if (getOverrideSpeed())
			setOverrideSpeed(false);

		if (!_audioStream && !isFastForwarding()) {
			_audioStream = Audio::makeQueuingAudioStream(22050, false);
			g_system->getMixer()->playStream(Audio::Mixer::kSpeechSoundType, &_soundHandle, _audioStream);
		}

		byte *data = (byte *)malloc(60000);
		int chunksize = in->read(data, 60000);
		if (!isFastForwarding()) {
			_audioStream->queueBuffer(data, chunksize, DisposeAfterUse::YES, Audio::FLAG_UNSIGNED);
		} else {
			free(data);
		}*/
	}

	void fadeIn(uint8 *targetpal) {
		// Don't do anything if we're asked to skip palette changes
		/*if (_flagSkipPalette)
			return;

		// TODO: Is it required? If so, move to an appropiate place
		// Copy the foreground to the background
		memcpy((byte *)_vm->_graphicsMan->_foreground.getPixels(), (byte *)_vm->_graphicsMan->_background.getPixels(), 640 * 320);

		// Start a fadein
		_vm->_graphicsMan->fadeIn(targetpal);

		// Show the background
		_vm->_graphicsMan->updateScreen(_bg);*/
	}

	void setPalette(uint8 *palette) {
		/*if (_flagSkipPalette)
			return;

		debugC(7, kDebugVideo, "Groovie::VDX: Setting palette");
		_syst->getPaletteManager()->setPalette(palette, 0, 256);*/
	}

};

VDXDecoder vdxdec;

}; // namespace Groovie
