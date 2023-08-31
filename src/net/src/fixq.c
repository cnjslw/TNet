/**
 * @brief 固定长度消息队列
 * @version 0.1
 */

#include "fixq.h"
#include "debug.h"

/**
 * @brief 定长队列初始化
 * @version 0.1
 * @param q 队列结构体
 * @param buf 消息缓存
 * @param size 消息队列大小
 * @param type 锁类型
 */
net_err_t fixq_init(fixq_t* q, void** buf, int size, nlocker_type_t type)
{
    q->size = size;
    q->in = q->out = q->cnt = 0;
    q->buf = buf;
    q->send_sem = SYS_SEM_INVALID;
    q->send_sem = SYS_SEM_INVALID;

    net_err_t err = nlocker_init(&q->locker, type);
    if (err < 0) {
        dbg_error(DBG_QUEUE, "init locker failed . ");
        return err;
    }
    q->send_sem = sys_sem_create(size);
    if (q->send_sem == SYS_SEM_INVALID) {
        dbg_error(DBG_QUEUE, "create sem failed . ");
        err = NET_ERR_SYS;
        goto init_failed;
    }
    q->recv_sem = sys_sem_create(size);
    if (q->recv_sem == SYS_SEM_INVALID) {
        dbg_error(DBG_QUEUE, "create sem failed . ");
        err = NET_ERR_SYS;
        goto init_failed;
    }
    return NET_ERR_OK;
init_failed:
    if (q->send_sem != SYS_SEM_INVALID)
        sys_sem_free(q->send_sem);
    if (q->recv_sem != SYS_SEM_INVALID)
        sys_sem_free(q->send_sem);
    nlocker_destroy(&q->locker);
    return err;
}

/**
 * @brief 发送消息
 * @version 0.1
 * @param q 消息队列
 * @param msg 发送消息
 * @param tmo 等待时长
 */
net_err_t fixq_send(fixq_t* q, void* msg, int tmo)
{
    nlocker_lock(&q->locker);
    if ((tmo < 0) && (q->cnt >= q->size)) {
        nlocker_unlock(&q->locker);
        return NET_ERR_FULL;
    }
    nlocker_unlock(&q->locker);
    if (sys_sem_wait(q->send_sem, tmo) < 0) {
        return NET_ERR_TMO;
    }
    nlocker_lock(&q->locker);
    q->buf[q->in++] = msg;
    if (q->in >= q->size)
        q->in = 0;
    nlocker_unlock(&q->locker);
    sys_sem_notify(q->recv_sem);
    return NET_ERR_OK;
}

/**
 * @brief 测试程序
 * @version 0.1
 * @param q CCCCCCC
 * @param tmo CCCCCCC
 */
void* fixq_recv(fixq_t* q, int tmo)
{
    nlocker_lock(&q->locker);
    if (!q->cnt && (tmo < 0)) {
        nlocker_unlock(&q->locker);
        return (void*)0;
    }
    nlocker_unlock(&q->locker);

    if (sys_sem_wait(q->recv_sem, tmo) < 0)
        return (void*)0;
    nlocker_lock(&q->locker);
    void* msg = q->buf[q->out++];
    if (q->out >= q->size)
        q->out = 0;
    nlocker_unlock(&q->locker);
    sys_sem_notify(q->send_sem);
    return msg;
}

/**
 * 销毁队列
 * @param list 待销毁的队列
 */
void fixq_destroy(fixq_t* q)
{
    nlocker_destroy(&q->locker);
    sys_sem_free(q->send_sem);
    sys_sem_free(q->recv_sem);
}

/**
 * @brief 取缓冲中消息的数量
 */
int fixq_count(fixq_t* q)
{
    nlocker_lock(&q->locker);
    int count = q->cnt;
    nlocker_unlock(&q->locker);
    return count;
}