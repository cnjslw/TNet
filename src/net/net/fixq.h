#ifndef __FIXQ_H__
#define __FIXQ_H__

#include "nlocker.h"
#include "sys.h"

/*code here*/
typedef struct _fixq_t {
    int size; // 消息容量
    void** buf; // 消息缓存,指向任意不同类型的消息
    int in, out, cnt; // 读写位置索引，消息数
    nlocker_t locker; // 访问的锁
    sys_sem_t recv_sem; // 接收消息时使用的信号量
    sys_sem_t send_sem; // 发送消息时使有的信号量
} fixq_t;

net_err_t fixq_init(fixq_t* q, void** buf, int size, nlocker_type_t type);
net_err_t fixq_send(fixq_t* q, void* msg, int tmo);
void* fixq_recv(fixq_t* q, int tmo);
void fixq_destory(fixq_t* q);
int fixq_count(fixq_t* q);
#endif