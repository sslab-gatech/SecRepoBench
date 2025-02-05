int flb_gzip_compress(void *in_data, size_t in_len,
                      void **out_data, size_t *out_len)
{
    int flush;
    int status;
    int footer_start;
    uint8_t *pb;
    size_t out_size;
    void *out_buf;
    z_stream strm;
    mz_ulong crc;

    // <MASK>
}