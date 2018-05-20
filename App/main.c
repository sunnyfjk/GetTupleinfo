/**
 * @Author: fjk
 * @Date:   2018-05-18T14:46:46+08:00
 * @Last modified by:   fjk
 * @Last modified time: 2018-05-20T16:25:01+08:00
 */
#include "include/GetTuple.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
struct NetLinkSocket_t ns = {0};
int SaveNetLinkReacvData(const char *name, struct TupleMessage_t *data,
                         int count) {
#if defined(__DEBUG__)
  int i = 0;
  char src[16] = {0}, dst[16] = {0};
#endif
  int fd = 0;
  int ret = 0;
  size_t len = 0;
  size_t all_len = sizeof(struct TupleMessage_t) * count;

  if (data == NULL || name == NULL)
    return -1;
  /*写入文件 开始*/
  fd = open(name, O_APPEND | O_CREAT | O_RDWR, 0660);
  if (fd < 0) {
    ret = fd;
    PERR("open %s file err\n", name);
    goto open_file_err;
  }
  while (len < all_len) {

    ret = write(fd, (((const char *)data) + len), all_len - len);
    if (ret < 0)
      continue;
    len += ret;
  }
  close(fd);
/*写入文件 结束*/
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
open_file_err:
  return ret;
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
