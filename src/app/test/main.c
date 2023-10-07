/**
 * @brief 测试程序
 * @version 0.1
 */

#include "debug.h"
#include "mblock.h"
#include "net.h"
#include "net_err.h"
#include "netif_pcap.h"
#include "nlist.h"
#include "nlocker.h"
#include "pktbuf.h"
#include "sys_plat.h"
#define DBG_TEST DBG_LEVEL_INFO

static sys_sem_t sem;

net_err_t netdev_init(void)
{
    netif_pcap_open();
    return NET_ERR_OK;
}

/**
 * @brief 链表测试
 */
typedef struct _tnode_t {
    int id;
    nlist_node_t node;
} tnode_t;
void nlist_test()
{

    tnode_t node[4];
    nlist_t list;
    nlist_init(&list);
    nlist_node_t* p;
    plat_printf("insert first\n");

    for (int i = 0; i < 4; i++) {
        node[i].id = i;
        nlist_insert_first(&list, &node[i].node);
    }

    nlist_for_each(p, &list)
    {
        tnode_t* tnode = nlist_entry(p, tnode_t, node);
        plat_printf("id : %d\n", tnode->id);
    }
    plat_printf("remove first\n");
    for (int i = 0; i < 4; i++) {
        p = nlist_remove_first(&list);
        plat_printf("id: %d\n", nlist_entry(p, tnode_t, node)->id);
    }
    plat_printf("insert to last\n");
    nlist_init(&list);
    for (int i = 0; i < 4; i++) {
        node[i].id = i;
        nlist_insert_last(&list, &node[i].node);
    }
    nlist_for_each(p, &list)
    {
        tnode_t* tnode = nlist_entry(p, tnode_t, node);
        plat_printf("id : %d\n", tnode->id);
    }
    plat_printf("remove last\n");
    for (int i = 0; i < 4; i++) {
        p = nlist_remove_last(&list);
        plat_printf("id: %d\n", nlist_entry(p, tnode_t, node)->id);
    }

    plat_printf("insert to after\n");
    nlist_init(&list);
    for (int i = 0; i < 4; i++) {
        node[i].id = i;
        nlist_insert_after(&list, nlist_first(&list), &node[i].node);
    }
    nlist_for_each(p, &list)
    {
        tnode_t* tnode = nlist_entry(p, tnode_t, node);
        plat_printf("id : %d\n", tnode->id);
    }
}

/**
 * @brief 内存控制块初始化
 */
void mblocker_test(void)
{
    mblock_t blist;
    static uint8_t buffer[100][10];
    mblock_init(&blist, buffer, 100, 10, NLOCKER_THREAD);
    void* temp[10];
    for (int i = 0; i < 10; i++) {
        temp[i] = mblock_alloc(&blist, 0);
        plat_printf("block : %p , free_count : %d\n", temp[i], mblock_free_cnt(&blist));
    }
}

/**
 * @brief 数据包测试
 */
void pktbuf_test(void)
{
    pktbuf_t* buf = pktbuf_alloc(2000);
    pktbuf_free(buf);
}

int main(void)
{
    // 日志测试
    // dbg_info(DBG_TEST, "debug_info");
    // dbg_warning(DBG_TEST, "debug_warning");
    // dbg_error(DBG_TEST, "debug_error");
    // 断言测试
    // dbg_assert(1 == 1, "fail");
    // 通用链表测试
    // nlist_test();
    // 内存控制块测试
    // mblocker_test();
    // 网络初始化
    // net_init();
    // net_start();
    pktbuf_test();

    // 不同网卡的初始化
    netdev_init();

    while (1) {
        sys_sleep(10);
    }
    return 0;
}