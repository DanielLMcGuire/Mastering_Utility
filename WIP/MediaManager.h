/**
 * @file MediaManager.h
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
