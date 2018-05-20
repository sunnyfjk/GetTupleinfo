/**
 * @Author: fjk
 * @Date:   2018-05-18T14:47:17+08:00
 * @Last modified by:   fjk
 * @Last modified time: 2018-05-18T19:49:01+08:00
 */
#include "../include/GetTuple.h"
#include <linux/netlink.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

struct ReadGetTupleData_t {
  int fd;
  int (*saveGetTupleData)(struct TupleMessage_t *, size_t);
  struct nlmsghdr *nlhdr;
};

void *ReadGetTupleDataThread(void *arg) {
  struct ReadGetTupleData_t *rgtd = (struct ReadGetTupleData_t *)arg;
  struct sockaddr_nl daddr = {0};
  struct msghdr msg = {0};
  struct iovec iov = {0};
  struct TupleMessage_t *data = NULL;
  int ret = 0;
  while (1) {
    memset(rgtd->nlhdr, 0, NLMSG_SPACE(sizeof(struct TupleMessage_t)));
    iov.iov_base = (void *)rgtd->nlhdr;
    iov.iov_len = NLMSG_SPACE(sizeof(struct TupleMessage_t));
    msg.msg_name = (void *)&daddr;
    msg.msg_namelen = sizeof(daddr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    ret = recvmsg(rgtd->fd, &msg, 0);
    if (ret > 0) {
      data = (struct TupleMessage_t *)NLMSG_DATA(rgtd->nlhdr);
      printf("protocol:%#x,src[%#x:%#x],dst[%#x:%#x]\n", data->protocol,
             data->saddr, data->sport, data->daddr, data->dport);
    }
  }

  return arg;
};

int OpenGetTuple(void) {
  int sd;
  int ret = -1;
  struct sockaddr_nl saddr;
  sd = socket(AF_NETLINK, SOCK_RAW, NETLINK_USERSOCK);
  if (sd < 0)
    return -1;
  saddr.nl_family = AF_NETLINK;
  saddr.nl_pid = getpid();
  saddr.nl_groups = NETLINK_GET_TUPLE_GROUP;
  ret = bind(sd, (struct sockaddr *)&saddr, sizeof(saddr));
  if (ret < 0)
    return ret;
  return sd;
}
void CloseGetTuple(int fd) { close(fd); }

pthread_t ReadGetTupleData(int fd,
                           int (*saveGetTupleData)(struct TupleMessage_t *,
                                                   size_t)) {
  int ret = 0;
  pthread_t pt;
  struct ReadGetTupleData_t *r;
  r = (struct ReadGetTupleData_t *)malloc(sizeof(*r));
  if (r == NULL) {
    ret = -1;
    goto Read_Get_Tuple_Data_malloc_err;
  }
  r->fd = fd;
  r->saveGetTupleData = saveGetTupleData;
  r->nlhdr =
      (struct nlmsghdr *)malloc(NLMSG_SPACE(sizeof(struct TupleMessage_t)));
  if (r->nlhdr == NULL) {
    ret = -2;
    goto nlmsghdr_malloc_err;
  }
  ret = pthread_create(&pt, NULL, ReadGetTupleDataThread, r);
  if (ret < 0) {
    ret = -3;
    goto pthread_create_err;
  }
  return pt;
pthread_create_err:
  free(r->nlhdr);
nlmsghdr_malloc_err:
  free(r);
Read_Get_Tuple_Data_malloc_err:
  return ret;
}
