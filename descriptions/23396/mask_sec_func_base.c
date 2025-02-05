void ndpi_parse_packet_line_info(struct ndpi_detection_module_struct *ndpi_str, struct ndpi_flow_struct *flow) {
  u_int32_t a;
  struct ndpi_packet_struct *packet = &flow->packet;

  if((packet->payload_packet_len < 3) || (packet->payload == NULL))
    return;

  if(packet->packet_lines_parsed_complete != 0)
    return;

  packet->packet_lines_parsed_complete = 1;
  ndpi_reset_packet_line_info(packet);

  packet->line[packet->parsed_lines].ptr = packet->payload;
  packet->line[packet->parsed_lines].len = 0;

  for (a = 0; ((a+1) < packet->payload_packet_len) && (packet->parsed_lines < NDPI_MAX_PARSE_LINES_PER_PACKET); a++) {
    if((packet->payload[a] == 0x0d) && (packet->payload[a+1] == 0x0a)) {
      // <MASK>
    }
  }

  if(packet->parsed_lines >= 1) {
    packet->line[packet->parsed_lines].len =
      (u_int16_t)(((unsigned long) &packet->payload[packet->payload_packet_len]) -
		  ((unsigned long) packet->line[packet->parsed_lines].ptr));
    packet->parsed_lines++;
  }
}