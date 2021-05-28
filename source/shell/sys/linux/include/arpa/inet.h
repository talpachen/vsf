#ifndef __VSF_LINUX_INET_H__
#define __VSF_LINUX_INET_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../netinet/in.h"
#else
#   include <netinet/in.h>
#endif
#include <inttypes.h>

// for endian APIs
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_SOCKET_CFG_FAKE_API == ENABLED
#define inet_addr           __vsf_linux_inet_addr
#define inet_lnaof          __vsf_linux_inet_lnaof
#define inet_makeaddr       __vsf_linux_inet_makeaddr
#define inet_netof          __vsf_linux_inet_netof
#define inet_network        __vsf_linux_inet_network
#define inet_aton           __vsf_linux_inet_aton
#define inet_ntoa           __vsf_linux_inet_ntoa
#define inet_ntop           __vsf_linux_inet_ntop
#endif

#define htonl               cpu_to_be32
#define htons               cpu_to_be16
#define ntohl               be32_to_cpu
#define ntohs               be16_to_cpu

in_addr_t inet_addr(const char *cp);
in_addr_t inet_lnaof(struct in_addr in);
struct in_addr inet_makeaddr(in_addr_t net, in_addr_t lna);
in_addr_t inet_netof(struct in_addr in);
in_addr_t inet_network(const char *cp);
int inet_aton(const char *cp, struct in_addr *addr);
char * inet_ntoa(struct in_addr in);
const char * inet_ntop(int af, const void *src, char *dst, socklen_t size);

#ifdef __cplusplus
}
#endif

#endif
