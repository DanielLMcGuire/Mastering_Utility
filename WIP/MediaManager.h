/**
 * @file MediaManager.h
 * @brief Implementation of the Mastering Utility's FFMPEG Helpers
 * @author Daniel McGuire
 */ 

 // Copyright 2025 Daniel McGuire
 //
 // This program is free software: you can redistribute it and/or modify
 // it under the terms of the GNU General Public License as published by
 // the Free Software Foundation, either version 3 of the License, or
 // (at your option) any later version.
 //
 // This program is distributed in the hope that it will be useful,
 // but WITHOUT ANY WARRANTY; without even the implied warranty of
 // MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 // GNU General Public License for more details.
 //
 // You should have received a copy of the GNU General Public License
 // along with this program.  If not, see <https://www.gnu.org/licenses/>.

// !!!!!!!!!!!!!!!!!!!!!!!!
// THIS CODE IS NOT TESTED!
// !!!!!!!!!!!!!!!!!!!!!!!!

#pragma once
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavutil/samplefmt.h>
#include <libavutil/channel_layout.h>
}

#include <string>
#include <vector>
#include <filesystem>
#include "MasteringUtility.h"

class MediaManager
{
public:
	static bool DecodeAudio(const std::filesystem::path& path, Song& song);
	static bool EncodeAudio(const std::filesystem::path& path, const Song& song, const std::string& codecName);

	static bool DecodeImage(const std::filesystem::path& path, Album& album);
	static bool EncodeImage(const std::filesystem::path& path, const Album& album, const std::string& format = "png");
};
