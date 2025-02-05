static void ndpi_search_bittorrent(struct ndpi_detection_module_struct *detectionmodule,
				   struct ndpi_flow_struct *flow) {
  struct ndpi_packet_struct *packet = &detectionmodule->packet;
  char *bt_proto = NULL;

  NDPI_LOG_DBG(detectionmodule, "Search bittorrent\n");

  /* This is broadcast */
  if(packet->iph) {
    if((packet->iph->saddr == 0xFFFFFFFF) || (packet->iph->daddr == 0xFFFFFFFF))
      goto exclude_bt;

    if(packet->udp) {
      u_int16_t sport = ntohs(packet->udp->source), dport = ntohs(packet->udp->dest);

      if(is_port(sport, dport, 3544) /* teredo */
	 || is_port(sport, dport, 5246) || is_port(sport, dport, 5247) /* CAPWAP */) {
      exclude_bt:
	NDPI_EXCLUDE_PROTO(detectionmodule, flow);
	return;
      }
    }
  }

  if(flow->detected_protocol_stack[0] != NDPI_PROTOCOL_BITTORRENT) {
    if(packet->tcp != NULL) {
      ndpi_int_search_bittorrent_tcp(detectionmodule, flow);
    } else if(packet->udp != NULL) {
      /* UDP */
      const char *bt_search  = "BT-SEARCH * HTTP/1.1\r\n";
      const char *bt_search1 = "d1:ad2:id20:";

      if((ntohs(packet->udp->source) < 1024)
	 || (ntohs(packet->udp->dest) < 1024) /* High ports only */) {
	ndpi_skip_bittorrent(detectionmodule, flow, packet);
	return;
      }

      /*
	Check for uTP http://www.bittorrent.org/beps/bep_0029.html

	wireshark/epan/dissectors/packet-bt-utp.c
      */

	// <MASK>
    }
  }

  if(flow->packet_counter > 8) {
    ndpi_skip_bittorrent(detectionmodule, flow, packet);
  }
}