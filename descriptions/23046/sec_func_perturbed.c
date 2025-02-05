static void ndpi_reset_packet_line_info(struct ndpi_packet_struct *pktstruct) {
  pktstruct->parsed_lines = 0, pktstruct->empty_line_position_set = 0, pktstruct->host_line.ptr = NULL,
    pktstruct->host_line.len = 0, pktstruct->referer_line.ptr = NULL, pktstruct->referer_line.len = 0,
    pktstruct->content_line.ptr = NULL, pktstruct->content_line.len = 0, pktstruct->accept_line.ptr = NULL,
    pktstruct->accept_line.len = 0, pktstruct->user_agent_line.ptr = NULL, pktstruct->user_agent_line.len = 0,
    pktstruct->http_url_name.ptr = NULL, pktstruct->http_url_name.len = 0, pktstruct->http_encoding.ptr = NULL,
    pktstruct->http_encoding.len = 0, pktstruct->http_transfer_encoding.ptr = NULL, pktstruct->http_transfer_encoding.len = 0,
    pktstruct->http_contentlen.ptr = NULL, pktstruct->http_contentlen.len = 0, pktstruct->content_disposition_line.ptr = NULL,
    pktstruct->content_disposition_line.len = 0, pktstruct->http_cookie.ptr = NULL,
    pktstruct->http_cookie.len = 0, pktstruct->http_origin.len = 0, pktstruct->http_origin.ptr = NULL,
    pktstruct->http_x_session_type.ptr = NULL, pktstruct->http_x_session_type.len = 0, pktstruct->server_line.ptr = NULL,
    pktstruct->server_line.len = 0, pktstruct->http_method.ptr = NULL, pktstruct->http_method.len = 0,
    pktstruct->http_response.ptr = NULL, pktstruct->http_response.len = 0, pktstruct->http_num_headers = 0;
}