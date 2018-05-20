/**
 * @Author: fjk
 * @Date:   2018-05-18T14:46:46+08:00
 * @Last modified by:   fjk
 * @Last modified time: 2018-05-20T15:39:22+08:00
 */
#include "include/GetTuple.h"
#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
struct NetLinkSocket_t ns = {0};
int SaveNetLinkReacvData(struct TupleMessage_t *data, int count) {
#if defined(__DEBUG__)
  int i = 0;
  char src[16] = {0}, dst[16] = {0};
#endif
  if (data == NULL)
    return -1;
#if defined(__DEBUG__)
  PERR("count=%d\n", count);
  for (i = 0; i < count; i++) {
    memset(src, 0, sizeof(src));
    memset(dst, 0, sizeof(dst));
    inet_ntop(AF_INET, &data[i].saddr, src, sizeof(src));
    inet_ntop(AF_INET, &data[i].daddr, dst, sizeof(dst));
    PERR("protocol=%d,src[ip=%s,port=%d],dst[ip=%s,port=%d]\n",
         data[i].protocol, src, ntohs(data[i].sport), dst,
         ntohs(data[i].dport));
  }
#endif
  return 0;
}
void CTRL_C(int signum) {
  if (signum == SIGINT) {
    DeleteNetLinkSocket(&ns);
  }
}
int main(int argc, char **argv) {
  int ret = 0;
  ns.SaveNetLinkReacvData = SaveNetLinkReacvData;
  ret = CreateNetLinkSocket(&ns);
  signal(SIGINT, CTRL_C);
  if (ret < 0) {
    PERR("CreateNetLinkSocket_err\n");
    goto CreateNetLinkSocket_err;
  }

  ret = ReacvNetLinkMessage(&ns);
  if (ret < 0) {
    PERR("ReacvNetLinkMessage_err\n");
    goto ReacvNetLinkMessage_err;
  }

  return 0;
ReacvNetLinkMessage_err:
  DeleteNetLinkSocket(&ns);
CreateNetLinkSocket_err:
  return ret;
}
