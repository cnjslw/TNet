/**
 * @brief 创建工作线程,负责消息的传递
 * @version 0.1
 */
#include "exmsg.h"
#include "sys_plat.h"

net_err_t exmsg_init(void)
{
    return NET_ERR_OK;
}
/**
 * @brief 核心工作线程,负责从下层接收包,向上层发送;负责从上层接受包,传递到下层
 * @version 0.1
 * @param XXX CCCCCCC
 * @param XXX CCCCCCC
 */
static void work_thread(void* arg)
{
    plat_printf("exmsg is running......\n");
    while (1) {
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