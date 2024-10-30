
#include "video.h"
#include "common.h"
#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <ostream>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <regex>
#include <fstream>

namespace  fs = std::filesystem;

namespace Video {

int GetVideoInfoByFilename(VideoInfo& video_info, const std::string& filename);
/**
 * is video file
 * @parmas {std::string&} path
 */
bool IsVideoFile(const std::string& filename) {
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
 * 0 fail
 * 1 success
 */
int FindVideoFile(std::vector<VideoInfo>& video_infos,const std::string& path) {
  if (fs::exists(path) && fs::is_directory(path)) {
    for (const auto& entry : fs::directory_iterator(path)) {
      if (fs::is_regular_file(entry) && IsVideoFile(entry.path())) {
        VideoInfo video_info;
        if (GetVideoInfoByFilename(video_info, entry.path().filename())) {
          video_info.season_no_ = "01";
          video_info.filename_ = entry.path().filename();
          video_info.path_ = entry.path().parent_path().string() + "/";
          video_infos.push_back(video_info);
        }
      }
    }
  }
  return video_infos.empty() ? 0 : 1;
}

/**
 * [Boushoku no Berserk][01][BDRIP][1080P][H264_FLAC].mkv
 */
void FormatVideoInfoOne(VideoInfo& video_info, const std::smatch& match) {
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
    info = info + ")";
  }

  video_info.title_ = title;
  video_info.episode_no_ = episode_no;
  video_info.info_ = info;
  video_info.type_ = type; 
}

/**
 * [LowPower-Raws] Digimon Frontier - 17 (BD 1080P x264 FLAC).mkv
 */
void FormatVideoInfoTwo(VideoInfo& video_info, const std::smatch& match) {
  std::string title = match[1].str();
  std::string episode_no = match[2].str();
  std::string info = match[3].str();
  std::string type = match[4].str();

  video_info.title_ = title;
  video_info.episode_no_ = episode_no;
  video_info.info_ = info;
  video_info.type_ = type;
}

/**
 * '[AI-Raws] シティーハンター #002 (BD 1440x1080 FLAC)[120CFC1E].mkv'
 */
void FormatVideoInfoThree(VideoInfo& video_info, const std::smatch& match) {
  std::string title = match[1].str();
  std::string episode_no = match[2].str();
  std::string info = match[3].str();
  std::string type = match[4].str();

  video_info.title_ = title;
  video_info.episode_no_ = episode_no;
  video_info.info_ = info;
  video_info.type_ = type;
}


int GetVideoInfoByFilename(VideoInfo& video_info, const std::string& filename) {

  //[Boushoku no Berserk][01][BDRIP][1080P][H264_FLAC].mkv
  std::regex regex_one(R"(\[(.*?)\]\[(\d+)\](.*?)\.(mkv|MKV|mp4|MP4))");
  //[LowPower-Raws] Digimon Frontier - 17 (BD 1080P x264 FLAC).mkv
  std::regex regex_two(R"(\[.*?\]\s+(.*?)\s+-\s+(\d{2,3})\s+(.*?)\.(mkv|MKV|mp4|MP4))");
  //[AI-Raws] シティーハンター #002 (BD 1440x1080 FLAC)[120CFC1E].mkv
  std::regex regex_three(R"(\[.*?\]\s+(.*?)\s+#(\d{2,3})\s+(.*?)\[.*?\]\.(mkv|MKV|mp4|MP4))");
  std::smatch match;
  if (std::regex_match(filename, match, regex_one)) {
    FormatVideoInfoOne(video_info, match);
    return 1;
  } else if (std::regex_match(filename, match, regex_two)) {
    FormatVideoInfoTwo(video_info, match);
    return 2;
  } else if (std::regex_match(filename, match, regex_three)) {
    FormatVideoInfoThree(video_info, match);
    return 3;
  } 
  else {
    return 0;
  }
}

void RenameVideoFile(const std::vector<VideoInfo> &videos, const std::string& path) {
  //write change record
  std::ofstream record_file(path + "rename_info.txt");

  if (!record_file.is_open()) {
    std::cerr << "Error: record file create falue";
    std::cout << "Renameing file success!" << std::endl;
    return;
  }

  for (VideoInfo video_info : videos) {
    std::string new_file_name = video_info.title_ + " - " + "S" + video_info.season_no_ 
      + "E" + video_info.episode_no_ + " - " + video_info.info_ + "." + video_info.type_;
    record_file << video_info.path_ + video_info.filename_ << " -> " << video_info.path_ + new_file_name;
    try {
      fs::rename(video_info.path_ + video_info.filename_, video_info.path_  + new_file_name);
      record_file << "  ok" << std::endl;
    } catch(const fs::filesystem_error& e) {
      record_file << "  fail" << std::endl;
      std::cerr << "Error renameing file: " << e.what() << std::endl;
    }
    record_file.close();
  }
  std::cout << "Renameing file success!" << std::endl;
}

void PrintFindFile(const std::vector<VideoInfo> video_infos) {
  if (video_infos.size() > 0) {
    PrintFilledLine("find video file");
    int i = 1;
    for (VideoInfo video_info : video_infos) {
      std::cout << "[" << i++ << "]:  " << video_info.path_ + video_info.filename_ << std::endl;
    }
    PrintFilledLine();
  }
}

void SortVideoInfo(std::vector<VideoInfo>& video_infos) {
  std::sort(video_infos.begin(), video_infos.end(), [](const VideoInfo& v1, const VideoInfo& v2) {
    return v1.season_no_ < v2.season_no_ && v1.episode_no_ < v2.episode_no_;
  });
}
void PrintRenameingVideos(const std::vector<VideoInfo>& video_infos) {
  PrintFilledLine();
  for (const VideoInfo& video_info : video_infos) {
    std::string new_file_name = video_info.title_ + " - " + "S" + video_info.season_no_ 
      + "E" + video_info.episode_no_ + " - " + video_info.info_ + "." + video_info.type_;
    std::cout << video_info.filename_ << " -> " << new_file_name << std::endl;
  }
    PrintFilledLine();
}

/**
 * reset info
 */
void ResetVideoInfo(const VideoInfo& video_info_in, std::vector<VideoInfo>& video_infos) {
  for (VideoInfo& video_info : video_infos) {
    if (!video_info_in.season_no_.empty()) {
      video_info.season_no_ = video_info_in.season_no_;
    }
    
  }
}

int Renameing(const std::string& path) {
  std::vector<VideoInfo> video_infos;
  if (!FindVideoFile(video_infos, path)) {
    std::cout <<"not find video" << std::endl;
    std::cout <<"EXIT"<<std::endl;
    return -1;
  }
  SortVideoInfo(video_infos);
  PrintRenameingVideos(video_infos);
  std::cout << "size: " << video_infos.size() << std::endl;
  std::cout << "is renameing file name(y/n/r):";
  std::string in_ch;
  std::getline(std::cin, in_ch);
  if (in_ch == "y") {
    std::cout <<"rename start"<<std::endl;
    RenameVideoFile(video_infos, path);
  }
  return 0;
}
}
