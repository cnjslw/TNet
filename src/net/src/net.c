/**
 * @brief 网络启动
 * @version 0.1
 */
#include "net.h"
#include "exmsg.h"
#include "net_plat.h"

/**
 * @brief 网络初始化
 * @version 0.1
 */
net_err_t net_init(void)
{
    net_plat_init(); // 平台相关初始化
    exmsg_init();
    return NET_ERR_OK;
}

net_err_t net_start(void)
{
    exmsg_start();
    return NET_ERR_OK;
}