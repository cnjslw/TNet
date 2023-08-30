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

int main(void)
{
    dbg_info("debug_info");
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