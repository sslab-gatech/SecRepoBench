ATTRIBUTE_NO_SANITIZE_INTEGER
static const xmlDictEntry *
xmlDictLookupInternal(xmlDictPtr dict, const xmlChar *pref,
                      const xmlChar *name, int maybeLen, int update) {
    xmlDictEntry *entry = NULL;
    const xmlChar *ret;
    unsigned hashValue;
    size_t maxLen, len, plen, klen;
    int found = 0;

    if ((dict == NULL) || (name == NULL))
	return(NULL);

    maxLen = (maybeLen < 0) ? SIZE_MAX : (size_t) maybeLen;

    if (pref == NULL) {
        hashValue = xmlDictHashName(dict->seed, name, maxLen, &len);
        if (len > INT_MAX)
            return(NULL);
        klen = len;
    } else {
        hashValue = xmlDictHashQName(dict->seed, pref, name, &plen, &len);
        if ((len > INT_MAX) || (plen >= INT_MAX - len))
            return(NULL);
        klen = plen + 1 + len;
    }

    if ((dict->limit > 0) && (klen >= dict->limit))
        return(NULL);

    /*
     * Check for an existing entry
     */
    if (dict->size > 0)
        entry = xmlDictFindEntry(dict, pref, name, klen, hashValue, &found);
    // <MASK>
}