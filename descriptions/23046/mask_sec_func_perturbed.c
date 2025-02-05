static void ndpi_reset_packet_line_info(struct ndpi_packet_struct *pktstruct) {
  pktstruct->parsed_lines = 0, pktstruct->empty_line_position_set = 0, pktstruct->host_line.ptr = NULL,
    pktstruct->host_line.len = 0, pktstruct->referer_line.ptr = NULL, pktstruct->referer_line.len = 0,
    pktstruct->content_line.ptr = NULL, pktstruct->content_line.len = 0, pktstruct->accept_line.ptr = NULL,
    pktstruct->accept_line.len = 0, pktstruct->user_agent_line.ptr = NULL, pktstruct->user_agent_line.len = 0,
    pktstruct->http_url_name.ptr = NULL, pktstruct->http_url_name.len = 0, pktstruct->http_encoding.ptr = NULL,
    pktstruct->http_encoding.len = 0, pktstruct->http_transfer_encoding.ptr = NULL, // <MASK>;
}