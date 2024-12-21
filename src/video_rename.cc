#include "video_rename.h"


VideoRename::Start() {
}
VideoRename::Print() {
    
}
bool VideoRename::IsVideo(const std::string& filename) {
    
}

std::vector<> VideoRename::FindVideos(const std::string& path) {
    if (fs::exists(path) && fs::is_directory(path)) {
        for (const auto& entry : fs::directory_iterator(path)) {
            if (fs::is_regular_file(entry) && IsVideo(entry.path().filename())) {
                VideoInfo video_info = GetVideoInfo(entry);
                video_infos_.push_back(video_info);
            }
        }
    }
}

