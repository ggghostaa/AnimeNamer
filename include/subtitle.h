
#ifndef SUBTITLE_H_
#define SUBTITLE_H_

#include "common.h"
#include "video.h"

struct SubtitleInfo {
  std::string path_;
  std::string language_;
  std::string type_;
  std::string episodes_no_;
  std::string filename_;
  std::string video_filename_;
};

namespace Subtitle {
void Renameing(const std::string& path);
}

#endif // !SUBTITLE_H_
