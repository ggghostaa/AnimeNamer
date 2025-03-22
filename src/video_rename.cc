#include "video_rename.h"

#include <fstream>
#include <iostream>
#include <ostream>

VideoRename::VideoRename(const std::string &path) {
    this->path_ = path;
}

/**
 * Start
 */
void VideoRename::Start() {
    FindVideos(this->path_);
    if (this->video_infos_.empty()) {
        std::cerr << "No videos found" << std::endl;
    }
    SortVideo();
    std::string in_ch;

    while (true) {
        Print();
        std::cout << "Is renaming file name(y/n/r):" << std::endl;
        std::getline(std::cin, in_ch);
        if (in_ch == "y") {
            Renaming();
            return;
        } else if (in_ch == "r") {
            ResetInfo();
        } else if (in_ch == "n") {
            return;
        }
    }
}

/**
 * Reset video info season, title
 */
void VideoRename::ResetInfo() {
    std::string in_season;
    std::string in_title;
    std::cout << "Please input season:" << std::endl;
    std::getline(std::cin, in_season);
    std::cout << "Please input title:" << std::endl;
    std::getline(std::cin, in_title);
    if (!in_season.empty()) {
        for (auto &video_info : this->video_infos_) {
            video_info.season_no_ = in_season;
        }
    }
    if (!in_title.empty()) {
        for (auto &video_info : this->video_infos_) {
            video_info.title_ = in_title;
        }
    }
}

/**
 * Renaming file
 */
void VideoRename::Renaming() {
    //write change record
    std::ofstream record_file(path_ + "info.txt");
    if (!record_file.is_open()) {
        std::cerr << "Error: record file create falue";
        std::cout << "Renameing file success!" << std::endl;
        return;
    }
    for (const auto &video_info : this->video_infos_) {
        std::string new_filename = video_info.title_ + " - " + "S" + video_info.season_no_
            + "E" + video_info.episode_no_ + " - " + video_info.info_ + "." + video_info.type_;
        record_file << video_info.path_ + video_info.filename_ << " -> " << video_info.path_ + new_filename;
        try {
            fs::rename(video_info.path_ + video_info.filename_, video_info.path_ + new_filename);
            record_file << " ok" << std::endl;
        } catch (std::exception &e) {
            record_file << " failed: " << std::endl;
            std::cerr << "Error renaming file: "<< e.what() << std::endl;
            return;
        }
    }
    record_file.close();
    std::cout << "Renaming file success!" << std::endl;
}

/**
 * Print renaming video file info
 */
void VideoRename::Print() {
    an::PrintFilledLine("VideoRename: ");
    for (const auto &video_info : this->video_infos_) {
        std::string new_filename = video_info.title_ + " - " + "S" + video_info.season_no_
            + "E" + video_info.episode_no_ + " - " + video_info.info_ + "." + video_info.type_;
        std::cout << video_info.filename_ << " ->" << new_filename << std::endl;
    }
    an::PrintFilledLine();
}

/**
 * Sort video by season, episode
 */
void VideoRename::SortVideo() {
    std::sort(this->video_infos_.begin(), this->video_infos_.end(),
        [] (const VideoInfo &a, const VideoInfo &b) {
            try {
                return std::stoi(a.episode_no_) < std::stoi(b.episode_no_);
            } catch (const std::exception &e) {
                std::cerr << e.what() << std::endl;
                return true;
            }
        });
}


/**
 * Is video file
 * @param filename std::string
 * @return bool
 */
bool VideoRename::IsVideo(const std::string& filename) {
    const std::vector<std::string> KvideoExtensions = {".mp4", ".avi", ".mkv", ".mov", ".wmv", ".flv", ".mpeg", ".webm"};
    auto it = std::find_if(KvideoExtensions.begin(), KvideoExtensions.end(), [&filename](const std::string& ext) {
      if (filename.size() <= ext.size()) return false;
      std::string file_extension = filename.substr(filename.size() - ext.size(), ext.size());
      for (std::string::size_type i = 0; i < file_extension.size(); ++i) {
        if (std::tolower(file_extension[i]) != ext[i]) {
            return false;
        }
      }
      return true;
    });
    return it != KvideoExtensions.end();
}

/**
 * Find video file
 * @param path std::string
 */
void VideoRename::FindVideos(const std::string& path) {
    if (fs::exists(path) && fs::is_directory(path)) {
        for (const auto& entry : fs::directory_iterator(path)) {
            if (fs::is_regular_file(entry) && IsVideo(entry.path().filename())) {
                an::ErrorCode error_code;
                VideoInfo video_info = GetVideoInfo(entry, error_code);
                if (error_code.val_ == an::ErrorType::SUCCESS) {
                    video_infos_.push_back(video_info);
                }
            }
        }
    }
}

/**
 * Get video info by video filename
 * @param path std::filesystem::path
 * @param error_code an:ErrorCode
 * @return VideoInfo
 */
VideoInfo VideoRename::GetVideoInfo(const fs::path &path, an::ErrorCode &error_code) {
    VideoInfo video_info;
    const std::string& filename = path.filename().string();
    video_info.path_ = path.parent_path().string();
    //[Boushoku no Berserk][01][BDRIP][1080P][H264_FLAC].mkv
    std::regex regex_one(R"(\[(.*?)\]\[(\d+)\](.*?)\.(mkv|MKV|mp4|MP4))");
    //[LowPower-Raws] Digimon Frontier - 17 (BD 1080P x264 FLAC).mkv
    std::regex regex_two(R"(\[.*?\]\s+(.*?)\s+-\s+(\d{2,3})\s+(.*?)\.(mkv|MKV|mp4|MP4))");
    //[AI-Raws] シティーハンター #002 (BD 1440x1080 FLAC)[120CFC1E].mkv
    std::regex regex_three(R"(\[.*?\]\s+(.*?)\s+#(\d{2,3})\s+(.*?)\[.*?\]\.(mkv|MKV|mp4|MP4))");
    std::smatch match;
    if (std::regex_match(filename, match, regex_one)) {
        MatchVideoInfoOne(video_info, match, error_code);
    } else if (std::regex_match(filename, match, regex_two)) {
        MatchVideoInfoTwo(video_info, match, error_code);
    } else if (std::regex_match(filename, match, regex_three)) {
        MatchVideoInfoThree(video_info, match, error_code);
    }
    return video_info;
}

/**
 * [Boushoku no Berserk][01][BDRIP][1080P][H264_FLAC].mkv
 */
void VideoRename::MatchVideoInfoOne(VideoInfo &video_info, const std::smatch &match, an::ErrorCode &error_code) {
    try {
        std::string title = match[1].str();
        std::string episode_no = match[2].str();
        std::string info = match[3].str();
        std::string type = match[4].str();
        //去除压制组信息
        if (size_t found_index = title.find("]["); found_index != std::string::npos) {
            title = title.substr(found_index + 2, title.size() - found_index);
        }
        size_t pos;
        while ((pos = info.find("][")) != std::string::npos) {
            info.replace(pos, 2, " ");
        }
        if (info[0] == '[') {
            info[0] = '(';
        } else {
            info = "(" + info;
        }
        if (info[info.size() - 1] == ']') {
            info[info.size() - 1] = ')';
        } else {
            info += ")";
        }
        video_info.title_ = title;
        video_info.episode_no_ = episode_no;
        video_info.info_ = info;
        video_info.type_ = type;
        error_code.val_ = an::ErrorType::SUCCESS;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        error_code.val_ = an::ErrorType::ERROR;
    }
}

/**
 * [LowPower-Raws] Digimon Frontier - 17 (BD 1080P x264 FLAC).mkv
 */
void VideoRename::MatchVideoInfoTwo(VideoInfo &video_info, const std::smatch &match, an::ErrorCode &error_code) {
    try {
        std::string title = match[1].str();
        std::string episode_no = match[2].str();
        std::string info = match[3].str();
        std::string type = match[4].str();

        video_info.title_ = title;
        video_info.episode_no_ = episode_no;
        video_info.info_ = info;
        video_info.type_ = type;
        error_code.val_ = an::ErrorType::SUCCESS;
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        error_code.val_ = an::ErrorType::ERROR;
    }
}

/**
 * '[AI-Raws] シティーハンター #002 (BD 1440x1080 FLAC)[120CFC1E].mkv'
 */
void VideoRename::MatchVideoInfoThree(VideoInfo &video_info, const std::smatch &match, an::ErrorCode &error_code) {
    try {
        std::string title = match[1].str();
        std::string episode_no = match[2].str();
        std::string info = match[3].str();
        std::string type = match[4].str();

        video_info.title_ = title;
        video_info.episode_no_ = episode_no;
        video_info.info_ = info;
        video_info.type_ = type;
        error_code.val_ = an::ErrorType::SUCCESS;
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        error_code.val_ = an::ErrorType::ERROR;
    }
}





