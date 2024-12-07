
#include "common.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <filesystem>
#include <regex>
namespace fs = std::filesystem;
#include "subtitle.h"


int GetSubtitleInfo(const std::string& filename, SubtitleInfo& subtitle_info);
int FindSubtitleFile(const std::string& path, std::vector<SubtitleInfo>& subtitle_infos, std::vector<SubtitleInfo>& un_subtitle_infos);
void PrintSubtitleInfo(const SubtitleInfo& subtitle_info);
void MatchVideoInfo(SubtitleInfo& subtitle_info, const std::string& path);
void RenameingSubtitleInfo(const std::vector<SubtitleInfo>& subtitle_infos);
void PrintRenamingInfo(const std::vector<SubtitleInfo>& subtitle_infos);

void Subtitle::Renameing(const std::string& path) {
  std::vector<SubtitleInfo> subtitle_infos;
  std::vector<SubtitleInfo> un_subtitle_infos;
  FindSubtitleFile(path, subtitle_infos, un_subtitle_infos);
  for (SubtitleInfo& s: subtitle_infos) {
    MatchVideoInfo(s, path);
  }
  PrintRenamingInfo(subtitle_infos);
  std::cout << "is renameing file name(y/n/r):";
  std::string in_ch;
  std::getline(std::cin, in_ch);
  if (in_ch == "y") {
    std::cout <<"rename start"<<std::endl;
    RenameingSubtitleInfo(subtitle_infos);
  }
}
/**
 * return int
 * 0  not found 
 * 1 match  
 * 2 undo match episodes
 */
int FindSubtitleFile(const std::string& path, std::vector<SubtitleInfo>& subtitle_infos, std::vector<SubtitleInfo>& un_subtitle_infos) {
  if (fs::exists(path) && fs::is_directory(path)) {
    for (const auto& entry : fs::directory_iterator(path)) {
      if (fs::is_regular_file(entry)) {
        SubtitleInfo subtitle_info;
        subtitle_info.path_ = entry.path().parent_path().string() + "/";
        subtitle_info.filename_ = entry.path().filename();
        int rs = GetSubtitleInfo(entry.path().filename(), subtitle_info);
        if (rs == 1) {
          subtitle_infos.push_back(subtitle_info);
        } else if (rs == 2) {
          //un_subtitle_infos.push_back(subtitle_info);
        }
      }
    }
    if (!un_subtitle_infos.empty()) {
      return 2;
    } else if (!subtitle_infos.empty()) {
      return 1;
    }

  }
  std::cout << "not match subtitle!!" << std::endl;
  return 0;
}

void PrintSubtitleInfo(const SubtitleInfo& subtitle_info) {
  std::cout << "filename_:" << subtitle_info.filename_ << std::endl;
  std::cout << "language_:" << subtitle_info.language_ << std::endl;
  std::cout << "episodes_no_:" << subtitle_info.episodes_no_ << std::endl; 
  std::cout << "type_:" << subtitle_info.type_ << std::endl;
  std::cout <<"path:" << subtitle_info.path_ << std::endl;
  std::cout <<"video_filename:" << subtitle_info.video_filename_ << std::endl;
}

/**
 * return int
 * 0 not match
 * 1 match success
 * 2 not match episodes
 */
int GetSubtitleInfo(const std::string& filename, SubtitleInfo& subtitle_info) {
  std::regex extension_reg (R"((\.[A-Za-z\d]+)?\.(ass|ASS|srt|SRT|sub|SUB|IDX|idx)$)");
  std::smatch match;
  if (std::regex_search(filename, match, extension_reg)) {
    if (match[1].matched) {
      subtitle_info.language_ = match[1].str();
    }
    subtitle_info.type_ = match[2].str();

    std::vector<std::regex> patterns = {
      std::regex(R"(第(\d{2,3})話)"),
      std::regex(R"(\[(\d{2,3})\])"),
      std::regex(R"(\b(\d{2,3})\b)"),
      std::regex(R"(\s+(\d{2,3})\s+)")
    };

    for (const auto& pattern : patterns) {
      if (std::regex_search(filename, match, pattern)) {
        subtitle_info.episodes_no_ = match[1].str();
        return 1;
      }
    }
    return 2;
  }
  return 0;
}

void MatchVideoInfo(SubtitleInfo& subtitle_info, const std::string& path) {
  std::string regex_str = R"(\s-\sS\d{2,3}E)" + subtitle_info.episodes_no_+ R"(\s-.*?\.(mkv|MKV|mp4|MP4))";
  if (fs::exists(path) && fs::is_directory(path)) {
    for (const auto& entry : fs::directory_iterator(path)) {
      std::string video_filename = entry.path().filename();
      std::smatch match;
      if (std::regex_search(video_filename, match, std::regex(regex_str))) {
         subtitle_info.video_filename_ = fs::path(video_filename).stem().string();
         return;
      }
    }
  }
}
void RenameingSubtitleInfo(const std::vector<SubtitleInfo>& subtitle_infos) {
  std::string temp_filename;
  for (const auto& subtitle_info : subtitle_infos) {
    if (!subtitle_info.video_filename_.empty()) {
      temp_filename = subtitle_info.video_filename_;
      break;
    }
  }
  for (const auto& subtitle_info : subtitle_infos) { 
    std::string new_filename = subtitle_info.video_filename_.empty() ? temp_filename : subtitle_info.video_filename_;
    new_filename = subtitle_info.video_filename_ + subtitle_info.language_ + "." + subtitle_info.type_;
    fs::rename(subtitle_info.path_ + subtitle_info.filename_, subtitle_info.path_+ new_filename);
  }
}

void PrintRenamingInfo(const std::vector<SubtitleInfo>& subtitle_infos) {
  PrintFilledLine("Renameing info", '=');
  std::string temp_filename;
  for (const auto& subtitle_info : subtitle_infos) {
    if (!subtitle_info.video_filename_.empty()) {
      temp_filename = subtitle_info.video_filename_;
      break;
    }
  }
  for (const auto& subtitle_info : subtitle_infos) { 
    std::string new_filename = subtitle_info.video_filename_.empty() ? temp_filename : subtitle_info.video_filename_;
    new_filename = subtitle_info.video_filename_ + subtitle_info.language_ + "." + subtitle_info.type_;
    std::cout << subtitle_info.path_ + subtitle_info.filename_ << " -> " << subtitle_info.path_ + new_filename << std::endl;
  }
}
