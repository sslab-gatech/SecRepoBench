int ndpi_serialize_string_int32(ndpi_serializer *_serializer,
				const char *key, int32_t value) {
#ifdef OPTIMIZE_CSV_SERIALIZATION
  ndpi_private_serializer *serializer = (ndpi_private_serializer*)_serializer;

  if(serializer->fmt == ndpi_serialization_format_csv) {
    /* Key is ignored */
    u_int32_t buff_diff = serializer->buffer.size - serializer->status.buffer.size_used;
    u_int16_t needed = 12 /* 10 (billion) + CVS separator + \0 */;
    int rc;

    if(buff_diff < needed) {
      if(ndpi_extend_serializer_buffer(&serializer->buffer, needed - buff_diff) < 0)
	return(-1);     	
    }

    if(!(serializer->status.flags & NDPI_SERIALIZER_STATUS_HDR_DONE)) {
      if(ndpi_serializer_header_string(serializer, key, strlen(key)) < 0)
	return(-1);
    }

    ndpi_serialize_csv_pre(serializer);
    buff_diff = serializer->buffer.size - serializer->status.buffer.size_used;

    rc = ndpi_snprintf((char*)&serializer->buffer.data[serializer->status.buffer.size_used],
		       buff_diff, "%u", value);

    if(rc < 0 || (u_int)rc >= buff_diff)
      return(-1);
    
    serializer->status.buffer.size_used += rc;
    
    return(0);
  } else
#endif
  return(ndpi_serialize_binary_int32(_serializer, key, strlen(key), value));
}