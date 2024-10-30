#ifndef COMMON_H_
#define COMMON_H_

#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string>
#include <iomanip> // for std::setw
#include <vector>

struct VideoInfo {
  std::string title_;
  std::string episode_no_;
  std::string season_no_;
  std::string info_;
  std::string type_;
  std::string filename_;
  std::string path_;
};
struct ParamsIn {
  std::string season_no_;
};

// 声明全局变量（使用 extern 防止多重定义）
extern int terminal_width;

// 使用 inline 关键字避免重复定义
inline int kTerminalWidth = 80;  // 默认终端宽度

// 更新终端宽度的函数
inline void UpdateTerminalWidth() {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
        perror("ioctl");
        kTerminalWidth = 80;  // 获取失败时使用默认值
    } else {
        kTerminalWidth = w.ws_col;  // 获取成功，更新终端宽度
    }
}

// 输出填充行并在中间显示内容的函数
inline void PrintFilledLine(const std::string& content = "", char fill_char = '=') {
    // 计算填充字符的数量
    int content_length = content.length();
    int padding = kTerminalWidth - content_length - 2; // 2 for the spaces around content

    // 如果填充字符数量小于 0，内容太长，无法放下
    if (padding < 0) {
        std::cout << content << std::endl;
        return;
    }

    int left_padding = padding / 2; // 左侧填充数量
    int right_padding = padding - left_padding; // 右侧填充数量

    // 输出左侧填充
    std::cout << std::setw(left_padding) << std::setfill(fill_char) << "" 
              << content // 输出内容
              << std::setw(right_padding) << std::setfill(fill_char) << "" 
              << std::endl; // 输出右侧填充
}

inline void PrintVideoInfo(const VideoInfo& video_info) {
  std::cout << "filename: "<<video_info.filename_ << std::endl;
  std::cout <<"剧集名称:"<<video_info.title_ << std::endl;
  std::cout <<"集数:"<<video_info.episode_no_ << std::endl;
  std::cout <<"规格:"<<video_info.info_ << std::endl;
  std::cout <<"类型:"<< video_info.type_ << std::endl;
}

inline void PrintVideoInfos(const std::vector<VideoInfo>& video_infos) {
    PrintFilledLine("video info", '-');

    for (const VideoInfo& video_info : video_infos) {
        PrintVideoInfo(video_info);
    }
    PrintFilledLine("", '-');
}

#endif  // COMMON_H_
