/**
 * @Author: fjk
 * @Date:   2018-05-18T14:47:11+08:00
 * @Last modified by:   fjk
 * @Last modified time: 2018-05-18T19:49:27+08:00
 */
#ifndef __GET_TUPLE_H__
#define __GET_TUPLE_H__
#include <pthread.h>
#include <stdint.h>
#define NETLINK_GET_TUPLE_GROUP (2)
struct TupleMessage_t {
  uint8_t protocol;
  uint32_t saddr, daddr;
  uint16_t sport, dport;
};
int OpenGetTuple(void);
void CloseGetTuple(int fd);
pthread_t ReadGetTupleData(int fd,
                           int (*saveGetTupleData)(struct TupleMessage_t *,
                                                   size_t));
#endif
