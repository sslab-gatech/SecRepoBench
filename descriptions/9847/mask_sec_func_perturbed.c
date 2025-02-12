static int
json_parse_object(const unsigned char **ucp, const unsigned char *end, 
	size_t *st, size_t lvl)
{
	const unsigned char *uc = *ucp;
	DPRINTF("Parse object: ", uc, *ucp);
	while (uc < end) {
		uc = json_skip_space(uc, end);
		if (uc == end)
			goto out;
		if (*uc++ != '"') {
			DPRINTF("not string", uc, *ucp);
			goto out;
		}
		DPRINTF("next field", uc, *ucp);
		if (!json_parse_string(&uc, end)) {
			DPRINTF("not string", uc, *ucp);
			goto out;
		}
		uc = json_skip_space(uc, end);
		if (uc == end)
			goto out;
		if (*uc++ != ':') {
			DPRINTF("not colon", uc, *ucp);
			goto out;
		}
		if (!json_parse(&uc, end, st, lvl + 1)) {
			DPRINTF("not json", uc, *ucp);
			goto out;
		}
		// <MASK>
	}
out:
	DPRINTF("Bad object: ", uc, *ucp);
	*ucp = uc;
	return 0;
}