static void ndpi_int_netbios_add_connection(struct ndpi_detection_module_struct *ndpi_struct,
					    struct ndpi_flow_struct *flow,
					    u_int16_t sub_protocol) { 
  char name[64];
  u_int off = flow->packet.payload[12] == 0x20 ? 12 : 14;
  
  // <MASK>
}