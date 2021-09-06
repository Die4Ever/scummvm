/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "clangame.h"

namespace Groovie {

// This a list of files for background music. These list is hard-coded in the TLC player.
const Common::String kClanMusicFiles[] = {"ACT01MUS.MPG", "ACT03MUS.MPG", "ACT04MUS.MPG", "ACT05MUS.MPG", "ACT06MUS.MPG", "ACT07MUS.MPG", "ACT08MUS.MPG", "ACT09MUS.MPG", "ACT10MUS.MPG",
										  "ACT13MUS.MPG", "ACT14MUS.MPG", "ACT15MUS.MPG", "ACT16MUS.MPG", "ACT17MUS.MPG", "ACT18MUS.MPG", "ACT19MUS.MPG", "ACT20MUS.MPG", "ACT21MUS.MPG",
										  "ACT22MUS.MPG", "ACT23MUS.MPG", "ACT24MUS.MPG", "ACT25MUS.MPG", "ACT26MUS.MPG", "MBF_ARB1.MPG", "MBF_ARM1.MPG", "MBF_BAL1.MPG", "MBF_BOL1.MPG",
										  "MBF_C1P1.MPG", "MBF_C1R1.MPG", "MBF_C2P1.MPG", "MBF_C2P2.MPG", "MBF_C2R1.MPG", "MBF_C3P1.MPG", "MBF_C3P2.MPG", "MBF_C3R1.MPG", "MBF_C4P1.MPG",
										  "MBF_C4P2.MPG", "MBF_C4R1.MPG", "MBF_CBK1.MPG", "MBF_MNE1.MPG", "MBF_UPH1.MPG"};

// Gets the filename of the background music file.
Common::String ClanGame::getClanMusicFilename(int musicId) {
	return kClanMusicFiles[musicId];
}

} // namespace Groovie
