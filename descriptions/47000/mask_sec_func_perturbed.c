void ndpi_search_raknet(struct ndpi_detection_module_struct *ndpi_struct,
                        struct ndpi_flow_struct *flow)
{
  struct ndpi_packet_struct * const packet = &ndpi_struct->packet;
  u_int8_t op, ip_addr_offset, neededpackets = 3;

  NDPI_LOG_DBG(ndpi_struct, "search RakNet\n");

  if (packet->udp == NULL || packet->payload_packet_len < 7)
  {
    NDPI_EXCLUDE_PROTO(ndpi_struct, flow);
    return;
  }

  op = packet->payload[0];

  switch (op)
  {
    case 0x00: /* Connected Ping */
      if (packet->payload_packet_len != 8)
      {
        NDPI_EXCLUDE_PROTO(ndpi_struct, flow);
        return;
      }
      neededpackets = 6;
      break;

    case 0x01: /* Unconnected Ping */
    case 0x02: /* Unconnected Ping */
      if (packet->payload_packet_len != 32)
      {
        NDPI_EXCLUDE_PROTO(ndpi_struct, flow);
        return;
      }
      neededpackets = 6;
      break;

    case 0x03: /* Connected Pong */
      if (packet->payload_packet_len != 16)
      {
        NDPI_EXCLUDE_PROTO(ndpi_struct, flow);
        return;
      }
      neededpackets = 6;
      break;

    case 0x05: /* Open Connection Request 1 */
      if (packet->payload_packet_len < 18 ||
          packet->payload[17] > 10 /* maximum supported protocol version */)
      {
        NDPI_EXCLUDE_PROTO(ndpi_struct, flow);
        return;
      }
      neededpackets = 6;
      break;

    case 0x06: /* Open Connection Reply 1 */
      if (packet->payload_packet_len != 28 ||
          packet->payload[25] > 0x01 /* connection uses encryption: bool -> 0x00 or 0x01 */)
      {
        NDPI_EXCLUDE_PROTO(ndpi_struct, flow);
        return;
      }

      {
        u_int16_t mtu_size = ntohs(get_u_int16_t(packet->payload, 26));
        if (mtu_size > 1500 /* Max. supported MTU, see: http://www.jenkinssoftware.com/raknet/manual/programmingtips.html */)
        {
          NDPI_EXCLUDE_PROTO(ndpi_struct, flow);
          return;
        }
      }
      neededpackets = 4;
      break;

    // <MASK>
  }

  if (flow->packet_counter < neededpackets)
  {
    return;
  }

  ndpi_int_raknet_add_connection(ndpi_struct, flow);
}