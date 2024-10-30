
#include <iostream>
#include <stdio.h>
#include <vector>
#include <filesystem>

namespace  fs = std::filesystem;

#include "video.h"
#include "common.h"
#include "subtitle.h"


int GetParmasIn(int argc, char* argv[], VideoInfo& video_info_in);

int main (int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Error Parmas" << std::endl;
    return -1;
  }
  std::string path = argv[argc - 1];
  std::string operator_type = argv[1];
  if (!fs::is_directory(path)) {
    std::cerr << "Error not valid path:" << path << std::endl;
  }
  ParamsIn parmas_in;
  PrintFilledLine("Renameing");
  if (operator_type == "-s") {
    Subtitle::Renameing(path);
  } else {
    Video::Renameing(path);
  }
  PrintFilledLine();
  return 0;
}

int GetParmasIn(int argc, char* argv[], VideoInfo& video_info_in) {
  if (argc < 2) {
    std::cerr << "Error Parmas" << std::endl;
    exit(0);
  }

  std::string path = argv[argc - 1];
  if (!fs::is_directory(path)) {
    std::cerr << "Error Path: " << path << std::endl;
  }

  for (int i = 1; i < argc - 1; ++i) {
    std::string prefix_season = "--season=";
    
    std::string arg = argv[i];
    if (arg.find(prefix_season) == 0) {
      video_info_in.season_no_ = arg.substr(prefix_season.length());
    } else {
      std::cerr << "Error Please enter the correct params" << std::endl;
    }
    std::cout << arg << std::endl;
  }
  return 1;
}
