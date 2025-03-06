if (ret == NULL) {
	xsltTransformError(tctxt, NULL, tctxt->inst,
	    "exsltCryptoRc4EncryptFunction: Failed to allocate result\n");
	tctxt->state = XSLT_STATE_STOPPED;
	xmlXPathReturnEmptyString (ctxt);
	goto done;
    }
    PLATFORM_RC4_DECRYPT (ctxt, padkey, bin, ret_len, ret, ret_len);
    ret[ret_len] = 0;

    xmlXPathReturnString (ctxt, ret);

done:
    if (key != NULL)
	xmlFree (key);