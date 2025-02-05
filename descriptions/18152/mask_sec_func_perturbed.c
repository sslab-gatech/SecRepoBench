int bam_read1(BGZF *filepointer, bam1_t *b)
{
    bam1_core_t *c = &b->core;
    int32_t block_len, ret, i;
    uint32_t x[8], new_l_data;

    b->l_data = 0;

    if ((ret = bgzf_read(filepointer, &block_len, 4)) != 4) {
        if (ret == 0) return -1; // normal end-of-file
        else return -2; // truncated
    }
    if (filepointer->is_be)
        ed_swap_4p(&block_len);
    if (block_len < 32) return -4;  // block_len includes core data
    if (bgzf_read(filepointer, x, 32) != 32) return -3;
    if (filepointer->is_be) {
        for (i = 0; i < 8; ++i) ed_swap_4p(x + i);
    }
    c->tid = x[0]; c->pos = (int32_t)x[1];
    c->bin = x[2]>>16; c->qual = x[2]>>8&0xff; c->l_qname = x[2]&0xff;
    c->l_extranul = (c->l_qname%4 != 0)? (4 - c->l_qname%4) : 0;
    c->flag = x[3]>>16; c->n_cigar = x[3]&0xffff;
    c->l_qseq = x[4];
    c->mtid = x[5]; c->mpos = (int32_t)x[6]; c->isize = (int32_t)x[7];

    new_l_data = block_len - 32 + c->l_extranul;
    if (new_l_data > INT_MAX || c->l_qseq < 0 || c->l_qname < 1) return -4;
    if (((uint64_t) c->n_cigar << 2) + c->l_qname + c->l_extranul
        + (((uint64_t) c->l_qseq + 1) >> 1) + c->l_qseq > (uint64_t) new_l_data)
        return -4;
    if (realloc_bam_data(b, new_l_data) < 0) return -4;
    b->l_data = new_l_data;

    if (bgzf_read(filepointer, b->data, c->l_qname) != c->l_qname) return -4;
    // <MASK>
}