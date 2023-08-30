/**
 * @brief 错误日志
 * @version 0.1
 */
#include "debug.h"
#include "sys_plat.h"
#include <stdarg.h>

void dbg_print(const char* file, const char* func, int line, const char* fmt, ...)
{
    const char* end = file + plat_strlen(file); // 处理file字符串,只需要文件名
    while (end >= file) {
        if ((*end == '\\') || (*end == '/')) {
            break;
        }
        end--;
    }
    end++;
    plat_printf("(%s-%s-%d):", end, func, line);

    // 处理可变参数
    char str_buf[128];
    va_list args;
    va_start(args, fmt);
    plat_vsprintf(str_buf, fmt, args);
    plat_printf("%s\n", str_buf);
    va_end(args);
}