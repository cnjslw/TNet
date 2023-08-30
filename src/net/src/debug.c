/**
 * @brief 错误日志
 * @version 0.2
 */
#include "debug.h"
#include "sys_plat.h"
#include <stdarg.h>

void dbg_print(int level, int s_level, const char* file, const char* func, int line, const char* fmt, ...)
{
    static const char* title[] = {
        [DBG_LEVEL_ERROR] = DBG_STYLE_ERROR "error",
        [DBG_LEVEL_WARNING] = DBG_STYLE_WARNING "warning",
        [DBG_LEVEL_INFO] = "info",
        [DBG_LEVEL_NONE] = "none"
    };
    if (level >= s_level) {
        const char* end = file + plat_strlen(file); // 处理file字符串,只需要文件名
        while (end >= file) {
            if ((*end == '\\') || (*end == '/')) {
                break;
            }
            end--;
        }
        end++;
        plat_printf("%s(%s-%s-%d):", title[s_level], end, func, line);

        // 处理可变参数
        char str_buf[128];
        va_list args;
        va_start(args, fmt);
        plat_vsprintf(str_buf, fmt, args);
        plat_printf("%s\n" DBG_STYLE_RESET, str_buf);
        va_end(args);
    }
}