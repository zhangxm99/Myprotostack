        
                          +--------------------------------+                                          
                          |           IPManager            |                                          
                          |+--------+           +--------+ |                                          
                          || TCPQ   |      -----|  UDPQ  | |                                          
                          |+--------+   if UDP  +--------+ |                                          
                          |       ^      /   +-----------+ |                                          
                          |        \    /  --|ICMPManager| |                                          
                          |   if TCP\  / -/  +-----------+ |      NetworkLayer                                    
                          |          \/-/if ICMP      |    |                                          
                          |           ^      |<-------|    |                                          
                          |-----------|------|-------------|                                          
                          |    receive|      |transmit     |                                          
                          |           |      |             |                                          
                          |           |      v             |                                          
                          |           IPDevice |           |                                          
                          +--------------------|-----------+                                          
                            |                  |                                                      
                     ip_read|                  |ip_write                                              
                          +-|------------------v-----------+                                          
                          | |      EthernetManager         |                                          
                          |                                |                                          
                          |+--------+         +-----------+|                                          
                          ||  ipQ   |    ---->|ArpManager ||                                          
                          |+--------+   /     +-----------+|                                          
                          |     ^      /        /          |                                             
                          |      \    /if arp  /           |      LinkLayer                                    
                          | if IP \  /        /            |                                          
                          |        \/        v             |                                          
                          |         ^       |              |                                          
                          |---------|-------|--------------|                                          
                          | receive |       v transmit     |                                          
                          |     ^    etdevice       |      |                                          
                          +--------------------------------+                                          
                                |                   |                                                 
                     tuntap_read|                   |tuntap_write                                     
                          +-----|-------------------|------+                                          
                          |     |                   v      |                                          
                          |                                |                                          
                          | Tuntap                         |      PhysicalLayer                                   
                          |                                |                                          
                          |                                |                                          
                          +--------------------------------+                                          