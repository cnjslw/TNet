/**
 * @brief 测试程序
 * @version 0.1
 */

#include "debug.h"
#include "net.h"
#include "net_err.h"
#include "netif_pcap.h"
#include "sys_plat.h"

static sys_sem_t sem;

net_err_t netdev_init(void)
{
    netif_pcap_open();
    return NET_ERR_OK;
}

#define DBG_TEST DBG_LEVEL_INFO

int main(void)
{
    // 日志测试
    dbg_info(DBG_TEST, "debug_info");
    dbg_warning(DBG_TEST, "debug_warning");
    dbg_error(DBG_TEST, "debug_error");
    // 网络初始化
    net_init();
    net_start();

    // 不同网卡的初始化
    netdev_init();

    while (1) {
        sys_sleep(10);
    }
    return 0;
}