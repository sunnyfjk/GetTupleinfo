/**
 * @Author: fjk
 * @Date:   2018-05-18T10:50:57+08:00
 * @Last modified by:   fjk
 * @Last modified time: 2018-05-18T15:22:11+08:00
 */
#ifndef __GET_TUPLE_H__
#define __GET_TUPLE_H__
#include <linux/init.h>
#include <linux/ip.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <net/net_namespace.h>

#define DRIVER_NAME "GetTuple"

#define PDEBUG(fmt, args...)                                                   \
  do {                                                                         \
    printk(KERN_DEBUG "[%s(%s:%d)]" fmt, DRIVER_NAME, __FUNCTION__, __LINE__,  \
           ##args);                                                            \
  } while (0)
#define PERR(fmt, args...)                                                     \
  do {                                                                         \
    printk(KERN_ERR "[%s(%s:%d)]" fmt, DRIVER_NAME, __FUNCTION__, __LINE__,    \
           ##args);                                                            \
  } while (0)
#define PINFO(fmt, args...)                                                    \
  do {                                                                         \
    printk(KERN_INFO "[%s(%s:%d)]" fmt, DRIVER_NAME, __FUNCTION__, __LINE__,   \
           ##args);                                                            \
  } while (0)

#define NETLINK_GET_TUPLE_GROUP (2)

#pragma pack(push, 1)
struct TupleMessage_t {
  uint8_t protocol;
  uint32_t saddr, daddr;
  uint16_t sport, dport;
};
#pragma pack(pop)
struct Tuple_t {
  struct sock *ssk;
};
#endif
