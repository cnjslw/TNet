#ifndef NET_ERR_H
#define NET_ERR_H
typedef enum _net_err_t {
    NET_ERR_OK = 0, // 没有错误
    NET_ERR_SYS = -1, // 系统错误
} net_err_t;

#endif