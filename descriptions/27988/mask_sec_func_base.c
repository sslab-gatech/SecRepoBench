void ndpi_free_flow(struct ndpi_flow_struct *flow) {
  if(flow) {
    // <MASK>

    if(flow->l4_proto == IPPROTO_TCP) {
      if(flow->l4.tcp.tls.message.buffer)
	ndpi_free(flow->l4.tcp.tls.message.buffer);
    }

    ndpi_free(flow);
  }
}