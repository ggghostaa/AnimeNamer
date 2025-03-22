
#ifndef SUBTITLE_RENAME_H_
#define SUBTITLE_RENAME_H_

#include "Temp.h"
class SubtitleRename {
private:
    std::string path_;
    std::vector<std::string> video_filenames_;
    void MathVideoInfo();
    void FindVideoFiles();
public:
    SubtitleRename(std::string path);
    SubtitleRename(const std::vector<VideoInfo>& video_infos);
    ~SubtitleRename();
};


#endif //SUBTITLE_RENAME_H_