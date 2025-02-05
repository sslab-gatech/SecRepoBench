static struct ndpi_flow_info *get_ndpi_flow_info6(struct ndpi_workflow * workflow,
						  u_int16_t vlan_id,
						  ndpi_packet_tunnel tunnel_type,
						  const struct ndpi_ipv6hdr *iph6,
						  u_int16_t ip_offset,
						  u_int16_t ipsize,
						  struct ndpi_tcphdr **tcph,
						  struct ndpi_udphdr **udph,
						  u_int16_t *sport, u_int16_t *dport,
						  u_int8_t *proto,
						  u_int8_t **payload,
						  u_int16_t *payload_len,
						  u_int8_t *src_to_dst_direction,
                                                  pkt_timeval when) {
  struct ndpi_iphdr iph;

  if(ipsize < 40)
    return(NULL);
  memset(&iph, 0, sizeof(iph));
  iph.version = IPVERSION;
  iph.saddr = iph6->ip6_src.u6_addr.u6_addr32[2] + iph6->ip6_src.u6_addr.u6_addr32[3];
  iph.daddr = iph6->ip6_dst.u6_addr.u6_addr32[2] + iph6->ip6_dst.u6_addr.u6_addr32[3];
  u_int8_t l4proto = iph6->ip6_hdr.ip6_un1_nxt;
  u_int16_t ip_len = ntohs(iph6->ip6_hdr.ip6_un1_plen);
  const u_int8_t *l4ptr = (((const u_int8_t *) iph6) + sizeof(struct ndpi_ipv6hdr));
  // <MASK>
}