
#ifndef VIDEO_RENAME.H_
#define VIDEO_RENAME_H_

#include <vector>

#include "Temp.h"
#include <string>
#include <filesystem>
#include <regex>
namespace fs = std::filesystem;

class VideoRename
{
private:
    std::vector<VideoInfo> video_infos_;
    unsigned int season_ = 0;
    std::string path_;

    void Renaming();
    void FindVideos(const std::string& path);
    void MatchVideo(const std::string& file_path);
    void SortVideo();
    void Print();
    void ResetInfo();
    VideoInfo GetVideoInfo(const fs::path& path, an::ErrorCode& error_code);

    void MatchVideoInfoOne(VideoInfo& video_info, const std::smatch& match, an::ErrorCode& error_code);
    void MatchVideoInfoTwo(VideoInfo& video_info, const std::smatch& match, an::ErrorCode& error_code);
    void MatchVideoInfoThree(VideoInfo& video_info, const std::smatch& match, an::ErrorCode& error_code);

public:
    static bool IsVideo(const std::string& filename);
    VideoRename(const std::string& path);
    ~VideoRename();
    void Start();
};


#endif //VIDEO_RENAME_H_