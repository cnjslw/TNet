/**
 * @brief 特定网卡驱动的线程(PCAP)
 * @version 0.1
 */

#include "netif_pcap.h"
#include "exmsg.h"
#include "sys_plat.h"

/**
 * @brief 发送线程
 * @version 0.1
 */
void recv_thread(void* args)
{
    plat_printf("recv thread on PCAP is running ... \n");
    while (1) {
        sys_sleep(1);
    }
}

/**
 * @brief 接收线程
 * @version 0.1
 */
void xmit_thread(void* args)
{
    plat_printf("xmit thread on PCAP is running ... \n");
    while (1) {
        sys_sleep(1);
    }
}

net_err_t netif_pcap_open(void)
{
    sys_thread_create(recv_thread, (void*)0);
    sys_thread_create(xmit_thread, (void*)0);
    return NET_ERR_OK;
}