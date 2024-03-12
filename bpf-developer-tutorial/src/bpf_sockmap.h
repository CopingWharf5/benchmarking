#include "vmlinux.h"
#include <bpf/bpf_endian.h>
#include <bpf/bpf_helpers.h>

#define LOCALHOST_IPV4 16777343

struct sock_key {
    __u32 sip;
    __u32 dip;
    __u32 sport;
    __u32 dport;
    __u32 family;
};

struct {
	__uint(type, BPF_MAP_TYPE_SOCKHASH);
	__uint(max_entries, 65535);
	__type(key, struct sock_key);
	__type(value, int);
} sock_ops_map SEC(".maps");

#define __bpf_md_ptr(type, name) \
    union {                     \
        type name;                \
        __u64: 64;              \
    } __attribute__((aligned(8)))

struct sock_md_tmp {
    __bpf_md_ptr(void *, data);
    __bpf_md_ptr(void *, data_end);
    __u32 family;
    __u32 remote_ip4;
    __u32 local_ip4;
    __u32 remote_ip6[4];
    __u32 local_ip6[4];
    __u32 remote_port;
    __u32 local_port;
    __u32 size;
    __bpf_md_ptr(struct bpf_sock *, sk);
};

struct {
   __uint(type, BPF_MAP_TYPE_ARRAY);
   __uint(max_entries, 1);
   __type(key, u32);
   __type(value, char[8]);
} input_map SEC(".maps");
