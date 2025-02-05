static int threedostr_probe(const AVProbeData *p)
{
    for (int i = 0; i < p->buf_size;) {
        unsigned chunk = AV_RL32(p->buf + i);
        unsigned length  = AV_RB32(p->buf + i + 4);

        // <MASK>
    }

    return 0;
}