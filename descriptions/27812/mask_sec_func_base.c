int32_t frame_get_usermeta(blosc2_frame* frame, uint8_t** usermeta) {
  int32_t header_len;
  int64_t frame_len;
  int64_t nbytes;
  int64_t cbytes;
  int32_t chunksize;
  int32_t nchunks;
  int ret = get_header_info(frame, &header_len, &frame_len, &nbytes, &cbytes, &chunksize, &nchunks,
                            NULL, NULL, NULL, NULL, NULL);
  if (ret < 0) {
    BLOSC_TRACE_ERROR("Unable to get the header info from frame.");
    return -1;
  }
  int64_t trailer_offset = get_trailer_offset(frame, header_len, cbytes);
  if (trailer_offset < 0) {
    BLOSC_TRACE_ERROR("Unable to get the trailer offset from frame.");
    return -1;
  }
  // <MASK>
}