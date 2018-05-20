/**
 * @Author: fjk
 * @Date:   2018-05-18T14:47:11+08:00
 * @Last modified by:   fjk
 * @Last modified time: 2018-05-20T16:10:45+08:00
 */
#ifndef __GET_TUPLE_H__
#define __GET_TUPLE_H__
#include <pthread.h>
#include <stdint.h>
#define NETLINK_GET_TUPLE_GROUP (2)
#define PTHREAD_COND (3)
#define TUPLE_MESSAGE_DATA (128)
#define STATE_CLOSE (0)
#define STATE_RUN (1)

#define PERR(fmt, args...)                                                     \
  do {                                                                         \
    fprintf(stderr, "[%s:%d]" fmt, __FUNCTION__, __LINE__, ##args);            \
  } while (0)

#pragma pack(push, 1)
struct TupleMessage_t {
  uint8_t protocol;
  uint32_t saddr, daddr;
  uint16_t sport, dport;
};
#pragma pack(pop)

struct NetLinkSocket_t {
  int fd;
  void *cond[PTHREAD_COND];
  int pos;
  int state;
  const char *name;
  int (*SaveNetLinkReacvData)(const char *, struct TupleMessage_t *, int);
};
int CreateNetLinkSocket(struct NetLinkSocket_t *ns);
int ReacvNetLinkMessage(struct NetLinkSocket_t *ns);
void DeleteNetLinkSocket(struct NetLinkSocket_t *ns);
#endif
