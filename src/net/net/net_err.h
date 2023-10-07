#ifndef NET_ERR_H
#define NET_ERR_H
typedef enum _net_err_t {
    NET_ERR_OK = 0, // 没有错误
    NET_ERR_SYS = -1, // 系统错误
    NET_ERR_MEM = -2, // 存储错误
    NET_ERR_FULL = -3, // 缓存满
    NET_ERR_TMO = -4, // 超时
    NET_ERR_NONE = -5, // 没有资源
    NET_ERR_SIZE = -6, // 大小错误
} net_err_t;

#endif