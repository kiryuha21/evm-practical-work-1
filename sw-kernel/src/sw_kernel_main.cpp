#include "lnh64.h"
#include "gpc_io_swk.h"
#include "gpc_handlers.h"

#define SW_VERSION 0x28102023
#define __fast_recall__

extern lnh lnh_core;
volatile unsigned int event_source;

int main(void) {
    lnh_init();
    for (;;) {
        event_source = wait_event();
        switch(event_source) {
            case __event__(receive_ips) : receive_ips(); break;
            case __event__(get_port_by_ip) : get_port_by_ip(); break;
        }
        set_gpc_state(READY);
    }
}
 
void receive_ips() {
    for (unsigned long long port = 0; port < 254; ++port){
        unsigned long long ip = mq_receive();
        lnh_ins_async(3, ip, port % 128);
    }
    sync();
}

void get_port_by_ip() {
    unsigned long long ip = mq_receive();
    lnh_search(3, ip);
    mq_send(lnh_core.result.value);    
}
