static const char *txtin_probe_data(const u8 *data, u32 data_size, GF_FilterProbeScore *score)
{
	char *dst = NULL;
	char *res=NULL;
	u32 resultlength=0;

	GF_Err e = gf_utf_get_string_from_bom((char *)data, data_size, &dst, &res, &resultlength);
	if (e) return NULL;

	data = res;
	//strip all spaces and \r\n\t
	// <MASK>
}