/**
 * @brief 创建工作线程,负责消息的传递
 * @version 0.2
 */
#include "exmsg.h"
#include "debug.h"
#include "fixq.h"
#include "mblock.h"
#include "net_cfg.h"
#include "sys_plat.h"

/**
 * @brief 定长队列管理消息,整体的逻辑:
 * 任何线程要向msg_queue发送消息,
 * 首先要从msg_buff申请一块内存,
 * 将消息写入这块内存,
 * 然后放入msg_queue
 * 被工作线程处理完成后,释放后重新放回msg_buff
 * 分配和释放依赖通用的msg_block管理
 */
static void* msg_tbl[NET_EXMSG_CNT]; // 指向任意类型不同的消息
static fixq_t msg_queue; // 消息队列

static exmsg_t msg_buff[NET_EXMSG_CNT]; // 存放消息使用的内存空间
static mblock_t msg_block; // 存放消息的内存空间使用通用链表进行管理

/**
 * @brief 消息交换结构的初始化
 */
net_err_t exmsg_init(void)
{
    dbg_info(DBG_MSG, "exmsg init");
    net_err_t err = fixq_init(&msg_queue, msg_tbl, NET_EXMSG_CNT, EXMSG_LOCKER);
    if (err < 0) {
        dbg_error(DBG_MSG, "fixq init fail\n");
        return err;
    }
    err = mblock_init(&msg_block, msg_buff, sizeof(exmsg_t), NET_EXMSG_CNT, EXMSG_LOCKER);
    if (err < 0) {
        dbg_error(DBG_MSG, "mblock init fail\n");
        return err;
    }
    dbg_info(DBG_MSG, "mblock init Success\n");
    return NET_ERR_OK;
}

/**
 * @brief 核心工作线程,负责从下层接收包,向上层发送;负责从上层接受包,传递到下层
 * @version 0.1
 */
static void work_thread(void* arg)
{
    dbg_info(DBG_MSG, "exmsg is running.....\n");
    while (1) {
        exmsg_t* msg = (exmsg_t*)fixq_recv(&msg_queue, 0);
        // plat_printf("recv a msg type : %d  ,  id : %d", msg->type, msg->id);
        mblock_free(&msg_block, msg);
        sys_sleep(1);
    }
}

/**
 * @brief 创建工作线程
 * @version 0.1
 */
net_err_t exmsg_start()
{
    sys_thread_t thread = sys_thread_create(work_thread, (void*)0);
    if (thread == SYS_THREAD_INVALID) {
        return NET_ERR_SYS;
    }
    return NET_ERR_OK;
}

/**
 * @brief 接收网卡数据
 * @version 0.1
 */
net_err_t exmsg_netif_in(void)
{
    exmsg_t* msg = mblock_alloc(&msg_block, -1); // 可能会丢包
    if (!msg) {
        dbg_warning(DBG_MSG, "no free msg");
        return NET_ERR_MEM;
    }
    static int id = 0;
    msg->type = NET_EXMSG_NETIF_IN;
    msg->id = id++;
    net_err_t err = fixq_send(&msg_queue, msg, -1);
    if (err < 0) {
        dbg_warning(DBG_MSG, "fixq full");
        mblock_free(&msg_block, msg);
        return err;
    }

    return err;
}