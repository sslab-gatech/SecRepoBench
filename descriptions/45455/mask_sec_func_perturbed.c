void
xmlParseReference(xmlParserCtxtPtr ctxt) {
    xmlEntityPtr ent;
    xmlChar *val;
    int was_checked;
    xmlNodePtr list = NULL;
    xmlParserErrors ret = XML_ERR_OK;


    if (RAW != '&')
        return;

    /*
     * Simple case of a CharRef
     */
    if (NXT(1) == '#') {
	int i = 0;
	xmlChar out[16];
	int hex = NXT(2);
	int characterValue = xmlParseCharRef(ctxt);

	if (characterValue == 0)
	    return;
	if (ctxt->charset != XML_CHAR_ENCODING_UTF8) {
	    /*
	     * So we are using non-UTF-8 buffers
	     * Check that the char fit on 8bits, if not
	     * generate a CharRef.
	     */
	    if (characterValue <= 0xFF) {
		out[0] = characterValue;
		out[1] = 0;
		if ((ctxt->sax != NULL) && (ctxt->sax->characters != NULL) &&
		    (!ctxt->disableSAX))
		    ctxt->sax->characters(ctxt->userData, out, 1);
	    } else {
		if ((hex == 'x') || (hex == 'X'))
		    snprintf((char *)out, sizeof(out), "#x%X", characterValue);
		else
		    snprintf((char *)out, sizeof(out), "#%d", characterValue);
		if ((ctxt->sax != NULL) && (ctxt->sax->reference != NULL) &&
		    (!ctxt->disableSAX))
		    ctxt->sax->reference(ctxt->userData, out);
	    }
	} else {
	    /*
	     * Just encode the value in UTF-8
	     */
	    COPY_BUF(0 ,out, i, characterValue);
	    out[i] = 0;
	    if ((ctxt->sax != NULL) && (ctxt->sax->characters != NULL) &&
		(!ctxt->disableSAX))
		ctxt->sax->characters(ctxt->userData, out, i);
	}
	return;
    }

    /*
     * We are seeing an entity reference
     */
    ent = xmlParseEntityRef(ctxt);
    if (ent == NULL) return;
    // <MASK>
}