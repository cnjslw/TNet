#ifndef __DBG_H__
#define __DBG_H__

/**
 * @brief 调试信息
 * @version 0.3 增加断言
 */

// 调试信息的显示样式设置
#define DBG_STYLE_RESET "\033[0m" // 复位显示
#define DBG_STYLE_ERROR "\033[31m" // 红色显示
#define DBG_STYLE_WARNING "\033[36m" // 蓝色显示

// 开启的信息输出配置，值越大，输出的调试信息越多
#define DBG_LEVEL_NONE 0 // 不开启任何输出
#define DBG_LEVEL_ERROR 1 // 只开启错误信息输出
#define DBG_LEVEL_WARNING 2 // 开启错误和警告信息输出
#define DBG_LEVEL_INFO 3 //

void dbg_print(int level, int s_level, const char* file, const char* func, int line, const char* fmt, ...);
#define dbg_info(level, fmt, ...) dbg_print(level, DBG_LEVEL_INFO, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define dbg_error(level, fmt, ...) dbg_print(level, DBG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define dbg_warning(level, fmt, ...) dbg_print(level, DBG_LEVEL_WARNING, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

#define dbg_assert(expr, msg)                                                                                               \
    {                                                                                                                       \
        if (!(expr)) {                                                                                                      \
            dbg_print(DBG_LEVEL_ERROR, DBG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, "assert failed: " #expr "," msg); \
            while (1)                                                                                                       \
                ;                                                                                                           \
        }                                                                                                                   \
    }

#endif