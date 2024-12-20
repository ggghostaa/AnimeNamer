
#ifndef VIDEO_RENAME.H_
#define VIDEO_RENAME_H_

#include <vector>

#include "Temp.h"
#include <stdio.h>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

class VideoRename
{
private:
    std::vector<VideoInfo> video_infos_;
    unsigned int season_ = 0;

    std::vector<std::string> FindVideos(const std::string& path);
    void MatchVideo(const std::string& file_path);
    void SortVideo();
    void Print();
    void RestartInfo();
    bool IsVideo(const std::string& filename);
    VideoInfo GetVideoInfo(const fs::path& path);
    
public:
    VideoRename(const std::string& path);
    ~VideoRename();
    void Start();
};


#endif //VIDEO_RENAME_H_