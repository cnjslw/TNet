#ifndef __DBG_H__
#define __DBG_H__

/*code here*/

/**
 * @brief 调试信息
 * @version 0.1
 */
void dbg_print(const char* file, const char* func, int line, const char* fmt, ...);
#define dbg_info(fmt, ...) dbg_print(__FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

#endif