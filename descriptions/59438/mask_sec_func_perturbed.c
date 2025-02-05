static int
simh_parse(const unsigned char *uc, const unsigned char *ue)
{
	uint32_t bytecount, cbytes;
	const unsigned char *orig_uc = uc;
	// <MASK>
	if (nt * sizeof(uint32_t) == CAST(size_t, uc - orig_uc))
		return 0;	/* All examined data was tapemarks (0) */
	if (nr == 0 && nt == 0)
		return 0;	/* No records and no tapemarks */
	return 1;
}