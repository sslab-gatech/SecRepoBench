static int
json_parse_array(const unsigned char **ucp, const unsigned char *ue,
	size_t *st, size_t lvl)
{
	const unsigned char *uc = *ucp;

	DPRINTF("Parse array: ", uc, *ucp);
	while (uc < ue) {
		uc = json_skip_space(uc, ue);
		// <MASK>
	}
out:
	DPRINTF("Bad array: ", uc,  *ucp);
	*ucp = uc;
	return 0;
}