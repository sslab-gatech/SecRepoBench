static void ndpi_check_http_server(struct ndpi_detection_module_struct *ndpi_struct,
				   struct ndpi_flow_struct *flow,
				   const char *server, u_int server_len) {
  if(server_len > 7) {
    u_int off;
  
    if(strncmp((const char *)server, "ntopng ", 7) == 0) {
      ndpi_set_detected_protocol(ndpi_struct, flow, NDPI_PROTOCOL_NTOP, NDPI_PROTOCOL_HTTP, NDPI_CONFIDENCE_DPI);
      NDPI_CLR_BIT(flow->risk, NDPI_KNOWN_PROTOCOL_ON_NON_STANDARD_PORT);
    } else if((strncasecmp(server, "Apache/", off = 7) == 0) /* X.X.X */
	      || (strncasecmp(server, "nginx/", off = 6) == 0) /* X.X.X */) {
      u_int i, j, a, b, c;
      char buf[16] = { '\0' };

      // <MASK>
    }
  }
}