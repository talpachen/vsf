#ifndef __VSF_LINUX_NETDB_H__
#define __VSF_LINUX_NETDB_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/socket.h"
#else
#   include <sys/socket.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct addrinfo {
    int              ai_flags;
    int              ai_family;
    int              ai_socktype;
    int              ai_protocol;
    socklen_t        ai_addrlen;
    struct sockaddr *ai_addr;
    char            *ai_canonname;
    struct addrinfo *ai_next;
};

struct hostent {
    char            *h_name;
    char            **h_aliases;
    int             h_addrtype;
    int             h_length;
    char            **h_addr_list;
};

struct netent {
    char            *n_name;
    char            **n_aliases;
    int             n_addrtype;
    uint32_t        n_net;
};

struct protoent {
    char            *p_name;
    char            **p_aliases;
    int             p_proto;
};

struct servent {
    char            *s_name;
    char            **s_aliases;
    int             s_port;
    char            *s_proto;
};

#if VSF_LINUX_SOCKET_CFG_FAKE_API == ENABLED
#define getnameinfo         __vsf_linux_getnameinfo
#define gethostbyaddr       __vsf_linux_gethostbyaddr
#define gai_strerror        __vsf_linux_gai_strerror
#define gethostbyname       __vsf_linux_gethostbyname
#define getaddrinfo         __vsf_linux_getaddrinfo
#define freeaddrinfo        __vsf_linux_freeaddrinfo
#endif

struct hostent * gethostbyaddr(const void *addr, size_t len, int type);
struct hostent * gethostbyname(const char *name);

const char * gai_strerror(int errcode);

// flags for getnameinfo
#define NI_NAMEREQD         (1 << 0)
#define NI_DGRAM            (1 << 1)
#define NI_NOFQDN           (1 << 2)
#define NI_NUMERICHOST      (1 << 3)
#define NI_NUMERICSERV      (1 << 4)
int getnameinfo(const struct sockaddr *addr, socklen_t addrlen,
                        char *host, socklen_t hostlen,
                        char *serv, socklen_t servlen, int flags);
int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints,
                        struct addrinfo **res);
void freeaddrinfo(struct addrinfo *res);

#ifdef __cplusplus
}
#endif
#endif
