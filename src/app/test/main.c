/**
 * @brief 测试程序
 * @version 0.1
 */

#include "debug.h"
#include "net.h"
#include "net_err.h"
#include "netif_pcap.h"
#include "nlist.h"
#include "sys_plat.h"

static sys_sem_t sem;

net_err_t netdev_init(void)
{
    netif_pcap_open();
    return NET_ERR_OK;
}

#define DBG_TEST DBG_LEVEL_INFO

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
int main(void)
{
    // 日志测试
    dbg_info(DBG_TEST, "debug_info");
    dbg_warning(DBG_TEST, "debug_warning");
    dbg_error(DBG_TEST, "debug_error");
    // 断言测试
    dbg_assert(1 == 1, "fail");
    // 通用链表测试
    nlist_test();
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