void
xmlSnprintfElementContent(char *buf, int capacity, xmlElementContentPtr content, int englob) {
    int len;

    if (content == NULL) return;
    len = strlen(buf);
    if (capacity - len < 50) {
	if ((capacity - len > 4) && (buf[len - 1] != '.'))
	    strcat(buf, " ...");
	return;
    }
    if (englob) strcat(buf, "(");
    // <MASK>
}