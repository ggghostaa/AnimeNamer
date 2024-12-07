#ifndef TEMP_H_
#define TEMP_H_


struct VideoInfo {
  std::string title_;
  unsigned int episode_no_;
  unsigned int season_no_;
  std::string info_;
  std::string type_;
  std::string filename_;
  std::string path_;
};

struct SubititleInfo {
    std::string video_filename_;
    unsigned int episode_no_;
    unsigned int season_no_;
    std::string type_;
    std::string filename_;
    std::string path_;
    std::string language_;
};


#endif //TEMP_H_
