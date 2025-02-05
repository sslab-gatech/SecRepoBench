static const char *ctxload_probe_data(const u8 *probe_data, u32 size, GF_FilterProbeScore *score)
{
	const char *mime_type = NULL;
	char *dst = NULL;
	GF_Err e;
	u32 probelength=size;
	char *res=NULL;

	/* check gzip magic header */
	if ((size>2) && (probe_data[0] == 0x1f) && (probe_data[1] == 0x8b)) {
		*score = GF_FPROBE_EXT_MATCH;
		return "btz|bt.gz|xmt.gz|xmtz|wrl.gz|x3dv.gz|x3dvz|x3d.gz|x3dz";
	}

	e = gf_utf_get_string_from_bom(probe_data, size, &dst, &res, &probelength);
	if (e) return NULL;
	probe_data = res;

	//strip all spaces and \r\n
	// <MASK>
}