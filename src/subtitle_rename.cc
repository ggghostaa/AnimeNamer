//
// Created by ggghost on 2025/1/12.
//

#include "subtitle_rename.h"

#include "video_rename.h"
#include <regex>

SubtitleRename::SubtitleRename(std::string path) {
    std::regex regex_str( R"(\s-\sS\(d{2,3})E.*?\.(mkv|MKV|mp4|MP4))");
    if (fs::exists(path) && fs::is_directory(path)) {
        for (const auto &entry : fs::directory_iterator(path)) {
            std::string filename = entry.path().filename().string();
            if (fs::is_regular_file(entry) && VideoRename::IsVideo(filename)) {
                std::smatch smatch;
                if (std::regex_search(filename, smatch, regex_str)) {
                }
            }
        }
    }
}
SubtitleRename::SubtitleRename(const std::vector<VideoInfo> &video_infos) {
    std::string str_len = video_infos.back().episode_no_;
    std::vector<std::string> video_filenames = std::vector<std::string> (std::stoi(str_len));
    for (const VideoInfo &video_info : video_infos) {
        std::string video_filename = video_info.title_ + " - " + "S" + video_info.season_no_
            + "E" + video_info.episode_no_ + " - " + video_info.info_;
        int index = std::stoi(video_info.episode_no_);
        video_filenames[index - 1] = video_filename;
    }
}

