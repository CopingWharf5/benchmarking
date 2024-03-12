#include "bpf_sockmap.h"
#define MAX_PACKET_OFF 0xffff

char LICENSE[] SEC("license") = "Dual BSD/GPL";

SEC("sk_msg")
int bpf_redir(struct sk_msg_md *msg)
{
    if(msg->remote_port == 38001) {
        // Initialize map
        u32 key = 0;
        char value[8] = "test123\n";
        bpf_map_update_elem(&input_map, &key, &value, BPF_ANY);
    } else if(msg->remote_port == 38002) {

        struct sock_key socket_key = {
            .sip = msg->remote_ip4,
            .dip = msg->local_ip4,
            .dport = bpf_htonl(msg->local_port), /* convert to network byte order */
            .sport = msg->remote_port,
            .family = msg->family,
        };
        struct sock_md_tmp new_socket;
        new_socket.family = msg->family;
        new_socket.remote_ip4 = msg->remote_ip4;
        new_socket.local_ip4 = msg->local_ip4;
        //new_socket.remote_ip6[4] = msg->remote_ip6[4];
        //new_socket.local_ip6[4] = msg->local_ip6[4];
        new_socket.remote_port = msg->remote_port;
        new_socket.local_port = msg->local_port;
        new_socket.size = msg->size;
        new_socket.sk = msg->sk;
        int err;
        int *msg_id_ptr;
        long len = (long)msg->data_end - (long)msg->data;
        if (len > 0) {
            bpf_printk("Message length: %ld\n", len);
        } else {
            bpf_printk("ERR: Message length is negative\n");
            return SK_PASS;
        }

        char *target_string;
        __u32 key = 0;
        target_string = bpf_map_lookup_elem(&input_map, &key);

        if (!target_string) {
                return 0;
        }

        new_socket.data = target_string;
        new_socket.data_end = target_string + 8;
        bpf_printk("rediret port %d --> %d\n", new_socket.local_port,
            	    bpf_ntohl(new_socket.remote_port));
        struct sk_msg_md *new_msg = &new_socket;
        return bpf_msg_redirect_hash(new_msg, &sock_ops_map, &socket_key, 0);
        //return bpf_msg_redirect_hash((struct sk_msg_md *)(&new_socket), &sock_ops_map, &socket_key, 0);
    }

//return SK_PASS;

    //err = bpf_probe_read_kernel(msg_str, 4, msg->data);
    //if (err < 0) {
    //  bpf_printk("ERR: bpf_probe_read_kernel failed\n");
    //  return SK_PASS;
    //}
    //bpf_printk("Socket Message: %s\n", msg_str);
    //}

    return SK_PASS;
}

//        // Try to print the message
//        char msg_str[4];
//        if (len < 4) {
//            bpf_printk("Message length is less than 300\n");
//        } else {
//            err = bpf_msg_pull_data(msg, 0, 4, 0);
//            if (err < 0) {
//                bpf_printk("ERR: bpf_msg_pull_data failed\n");
//                return SK_PASS;
//            }
//            void *data_end = (void*)(long)msg->data_end;
//            void *data = (void*)(long)msg->data;
//            char *body = data + 1;
//            if(body >= data_end) return SK_PASS;
//            char val = *body;
//            bpf_printk("The value is: %c\n", val);
//            //unsigned off = 0;
//            //char *data =(char*)(long)msg->data ;
//            //if(off + msg->data + 1 <= data_end) {
//            //	data[off] = 'a';
//        }

//char custom_data[] = "abcd"; // Example custom data
//long custom_data_len = sizeof(custom_data) - 1; // Exclude null terminator
//bpf_printk("Hey!\n");
//if(msg->remote_port == 38001) {
//           long data_len = (long)msg->data_end - (long)msg->data;
//	    if(data_len > 0) {
//       	    bpf_printk("Length of data is: %l\n", data_len);
//		    } else {
//       	    bpf_printk("ERR: Message length is negative\n", data_len);
//		    return SK_PASS;
//		    }
//bpf_msg_pull_data(msg, 0, data_len, 0);
//     data_len = (long)msg->data_end - (long)msg->data;
// 	    bpf_printk("Length of data after pull is: %l\n", data_len);
//    if ((char *)(long)msg->data + custom_data_len > (char *)(long)msg->data_end) {
//	            return SK_DROP; // Drop the packet if there isn't enough space
//    }
//if((char*)(long)msg->data_end >(char*)(long)msg->data) {
//	     char *data =(char*)(long)msg->data ;
//        *data = 'X';
//}
//char c = data[0];
//bpf_printk("Data is: %c\n", c);
//    if (msg->data + 1 > msg->data_end) {
//	    return SK_DROP;
//    } else {
//	    bpf_printk("%c\n", data[0]);

//}
//
//if(msg->data + 4 < msg->data_end)
//bpf_probe_read_kernel(msg->data, 4, custom_data);
//bpf_probe_write_user(msg->data, custom_data, custom_data_len);
//	msg->data = (void*)custom_data;


//for (long i = 0; i < custom_data_len; i++) {
//	   if (data + i < (char *)(long)msg->data_end &&  i < custom_data_len && i < MAX_PACKET_OFF ) {
// 	    	data[i] = custom_data[i];
//	    }
// 	}
//}

//struct sock_key key = {
//    .sip = msg->remote_ip4,
//    .dip = msg->local_ip4,
//    .dport = bpf_htonl(msg->local_port), /* convert to network byte order */
//    .sport = msg->remote_port,
//    .family = msg->family,
//};

//bpf_printk("rediret port %d --> %d\n", msg->local_port,
//    	    bpf_ntohl(msg->remote_port));
//return bpf_msg_redirect_hash(msg, &sock_ops_map, &key, 0);
//return SK_PASS;
//}
