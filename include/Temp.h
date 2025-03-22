#ifndef TEMP_H_
#define TEMP_H_
#include <iomanip>
#include <iostream>
#include <unistd.h>
#include <sys/ioctl.h>


struct VideoInfo {
  std::string title_;
  std::string episode_no_;
  std::string season_no_;
  std::string info_;
  std::string type_;
  std::string filename_;
  std::string path_;
};

struct SubtitleInfo {
    std::string video_filename_;
    std::string episode_no_;
    std::string season_no_;
    std::string type_;
    std::string filename_;
    std::string path_;
    std::string language_;
};
namespace an {
struct ErrorCode {
    int val_;
    std::string msg_;
};
enum ErrorType {
    ERROR,
    SUCCESS
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


}

#endif //TEMP_H_
