/**
 * @Author: fjk
 * @Date:   2018-05-18T10:11:21+08:00
 * @Last modified by:   fjk
 * @Last modified time: 2018-05-18T14:43:15+08:00
 */
#include "GetTuple.h"

struct Tuple_t tuple = {0};

int SendTupleMessage(struct TupleMessage_t *tmsg) {
  struct sk_buff *skb = NULL;
  struct nlmsghdr *nlh = NULL;
  int ret = 0;
  skb = nlmsg_new(NLMSG_SPACE(sizeof(*tmsg)), GFP_ATOMIC);
  if (IS_ERR_OR_NULL(skb))
    return -ENOMEM;
  nlh = nlmsg_put(skb, 0, 0, NLMSG_DONE, sizeof(*tmsg), 0);
  memcpy(NLMSG_DATA(nlh), tmsg, sizeof(*tmsg));
  ret =
      netlink_broadcast(tuple.ssk, skb, 0, NETLINK_GET_TUPLE_GROUP, GFP_ATOMIC);
  /* ENOBUFS should be handled in userspace */
  if (ret == -ENOBUFS || ret == -ESRCH)
    ret = 0;
  return ret;
}

void GetTupleNetLinkRecv(struct sk_buff *skb) {
  /*不需要接收功能，所以没有内容*/
}

unsigned int GetTuple_hookfn(void *priv, struct sk_buff *skb,
                             const struct nf_hook_state *state) {
  struct iphdr *iph = NULL;
  struct TupleMessage_t tmsg = {0};
  if (IS_ERR_OR_NULL(skb))
    return NF_ACCEPT;
  iph = ip_hdr(skb);
  if (IS_ERR_OR_NULL(iph))
    return NF_ACCEPT;
  if (!(iph->protocol == IPPROTO_TCP || iph->protocol == IPPROTO_UDP))
    return NF_ACCEPT;
  tmsg.protocol = iph->protocol;
  tmsg.saddr = iph->saddr;
  tmsg.daddr = iph->daddr;
  memcpy(&(tmsg.sport), skb_transport_header(skb), sizeof(uint16_t) * 2);
#if defined(__DEBUG__)
  PDEBUG("protocol=%d,src[%pI4:%d],dst[%pI4:%d]\n", tmsg.protocol, &tmsg.saddr,
         ntohs(tmsg.sport), &tmsg.daddr, ntohs(tmsg.dport));
#endif
  SendTupleMessage(&tmsg);
  return NF_ACCEPT;
}

struct nf_hook_ops GetTupleOps[] = {
    {
        .hook = GetTuple_hookfn,
        .pf = PF_INET,
        .hooknum = NF_INET_LOCAL_OUT,
        .priority = NF_IP_PRI_LAST - 1,
    },
};
struct netlink_kernel_cfg GetTupleCfg = {
    .groups = NETLINK_GET_TUPLE_GROUP, .input = GetTupleNetLinkRecv,
};

/*init_net*/
static __init int GetTuple_init(void) {
  int ret = 0;
  PINFO("INIT\n");
  tuple.ssk = netlink_kernel_create(&init_net, NETLINK_USERSOCK, &GetTupleCfg);
  if (IS_ERR_OR_NULL(tuple.ssk)) {
    ret = -ENOMEM;
    PERR("netlink kernel create err\n");
    goto netlink_kernel_create_err;
  }
  ret = nf_register_net_hooks(&init_net, GetTupleOps, ARRAY_SIZE(GetTupleOps));
  if (ret) {
    PERR("register net hooks Get Tuple err\n");
    goto nf_register_net_hooks_err;
  }
  return 0;
nf_register_net_hooks_err:
  netlink_kernel_release(tuple.ssk);
netlink_kernel_create_err:
  return ret;
}

static __exit void GetTuple_exit(void) {
  PINFO("EXIT\n");
  nf_unregister_net_hooks(&init_net, GetTupleOps, ARRAY_SIZE(GetTupleOps));
  netlink_kernel_release(tuple.ssk);
}

module_init(GetTuple_init);
module_exit(GetTuple_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("FJK");
MODULE_DESCRIPTION("this is a Tencent Test");
