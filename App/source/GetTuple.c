/**
 * @Author: fjk
 * @Date:   2018-05-18T14:47:17+08:00
 * @Last modified by:   fjk
 * @Last modified time: 2018-05-20T16:07:31+08:00
 */
#include "../include/GetTuple.h"
#include <errno.h>
#include <linux/netlink.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
struct PthreadControl_t {
  const char *name;
  struct TupleMessage_t td[TUPLE_MESSAGE_DATA];
  int pos;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  pthread_t pd;
  int state;
  int (*SaveNetLinkReacvData)(const char *, struct TupleMessage_t *, int);
};
void *SaveWork(void *arg) {
  struct PthreadControl_t *pc = (struct PthreadControl_t *)arg;
  if (pc == NULL)
    return arg;

  while (1) {
#if defined(__DEBUG__)
    PERR(":run\n");
#endif
    pthread_mutex_lock(&pc->mutex);
    while (pc->state == STATE_RUN && pc->pos == 0)
      pthread_cond_wait(&pc->cond, &pc->mutex);
    if (pc->pos > 0 && pc->SaveNetLinkReacvData != NULL)
      pc->SaveNetLinkReacvData(pc->name, pc->td, pc->pos);
    pc->pos = 0;
    pthread_cond_signal(&(pc->cond));
    if (pc->state == STATE_CLOSE) {
      pthread_mutex_unlock(&pc->mutex);
      break;
    }
    pthread_mutex_unlock(&pc->mutex);
  }
#if defined(__DEBUG__)
  PERR(":close\n");
#endif
  return arg;
};
struct PthreadControl_t *PthreadControlCreate(
    const char *name,
    int (*SaveNetLinkReacvData)(const char *, struct TupleMessage_t *, int)) {
  int ret = 0;
  struct PthreadControl_t *pc;
  pc = (struct PthreadControl_t *)malloc(sizeof(*pc));
  if (pc == NULL) {
    ret = -1;
    goto parameter_error;
  }
  pc->name = name;
  pc->state = STATE_RUN;
  pc->pos = 0;
  pc->SaveNetLinkReacvData = SaveNetLinkReacvData;
  ret = pthread_cond_init(&pc->cond, NULL);
  if (ret < 0) {
    goto pthread_cond_init_err;
  }
  ret = pthread_mutex_init(&pc->mutex, NULL);
  if (ret < 0) {
    goto pthread_mutex_init_err;
  }
  ret = pthread_create(&pc->pd, NULL, SaveWork, pc);
  if (ret < 0) {
    goto pthread_create_err;
  }
  return pc;
pthread_create_err:
  pthread_mutex_destroy(&pc->mutex);
pthread_mutex_init_err:
  pthread_cond_destroy(&pc->cond);
pthread_cond_init_err:
  pc->state = STATE_CLOSE;
  pc->pos = 0;
  pc->SaveNetLinkReacvData = NULL;
  free(pc);
parameter_error:
  return NULL;
}
void PthreadControlDelete(struct PthreadControl_t *pc) {
  pc->state = STATE_CLOSE;
  pthread_cond_broadcast(&pc->cond);
  pthread_join(pc->pd, NULL);
  pthread_mutex_destroy(&pc->mutex);
  pthread_cond_destroy(&pc->cond);
  free(pc);
}

int CreateNetLinkSocket(struct NetLinkSocket_t *ns) {
  int ret = 0;
  int i = 0, j = 0;
  struct sockaddr_nl addr = {0};
  char name[100];
  if (ns == NULL) {
    ret = -1;
    goto parameter_error;
  }
  ns->state = 1;
  ns->pos = 0;
  ns->fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_USERSOCK);
  if (ns->fd < 0) {
    ret = -2;
    goto netlink_socket_err;
  }
  addr.nl_family = AF_NETLINK;
  addr.nl_pid = getpid();
  addr.nl_groups = NETLINK_GET_TUPLE_GROUP;
  ret = bind(ns->fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_nl));
  if (ret < 0) {
    goto netlink_bind_err;
  }
  if (ns->name == NULL)
    ns->name = "./Defaule";
  for (i = 0; i < PTHREAD_COND; i++) {
    memset(name, 0, sizeof(name));
    sprintf(name, "Tuple_%s_%d", ns->name, i);
    ns->cond[i] = PthreadControlCreate(name, ns->SaveNetLinkReacvData);
    if (ns->cond[i] == NULL)
      goto PthreadControlCreate_err;
  }
  return 0;
PthreadControlCreate_err:
  for (j = 0; j < i; j++)
    PthreadControlDelete(ns->cond[i]);
netlink_bind_err:
  close(ns->fd);
netlink_socket_err:
parameter_error:
  return ret;
}
int ReacvNetLinkMessage(struct NetLinkSocket_t *ns) {
  int ret = 0;
  struct nlmsghdr *nlh = NULL;
  struct sockaddr_nl src_addr = {0};
  socklen_t addrlen = sizeof(struct sockaddr_nl);
  struct PthreadControl_t **cond = (struct PthreadControl_t **)ns->cond;
  if (ns == NULL || ns->cond == NULL) {
    ret = -1;
    goto parameter_error;
  }
  nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(sizeof(struct TupleMessage_t)));
  if (nlh == NULL) {
    ret = -2;
    goto malloc_nlmsghdr_err;
  }
  while (ns->state == STATE_RUN) {
    memset(&src_addr, 0, sizeof(struct sockaddr_nl));
    memset(nlh, 0, NLMSG_SPACE(sizeof(struct TupleMessage_t)));
    ret = recvfrom(ns->fd, nlh, NLMSG_SPACE(sizeof(struct TupleMessage_t)), 0,
                   (struct sockaddr *)&src_addr, (socklen_t *)&addrlen);
    if (ret < 0 ||
        ((nlh->nlmsg_len - NLMSG_SPACE(0)) != sizeof(struct TupleMessage_t))) {
      continue;
    }
    pthread_mutex_lock(&(cond[ns->pos]->mutex));
    while (cond[ns->pos]->pos >= TUPLE_MESSAGE_DATA)
      pthread_cond_wait(&(cond[ns->pos]->cond), &(cond[ns->pos]->mutex));
    memcpy((unsigned char *)(&(cond[ns->pos]->td[cond[ns->pos]->pos])),
           NLMSG_DATA(nlh), nlh->nlmsg_len - NLMSG_SPACE(0));
    ret = 0;
    cond[ns->pos]->pos++;
    if (!(cond[ns->pos]->pos < TUPLE_MESSAGE_DATA)) {
      ret = 1;
      pthread_cond_signal(&(cond[ns->pos]->cond));
    }
    pthread_mutex_unlock(&(cond[ns->pos]->mutex));
    if (ret == 1) {
      ns->pos++;
      if (!(ns->pos < PTHREAD_COND))
        ns->pos = 0;
    }
  }
  free(nlh);
  return 0;
malloc_nlmsghdr_err:
parameter_error:
  return ret;
}
void DeleteNetLinkSocket(struct NetLinkSocket_t *ns) {
  int i = 0;
  if (ns == NULL)
    return;
  ns->state = STATE_CLOSE;
  close(ns->fd);
  for (i = 0; i < PTHREAD_COND; i++)
    PthreadControlDelete(ns->cond[i]);
}
