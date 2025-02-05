static void
xmlTextReaderFreeProp(xmlTextReaderPtr reader, xmlAttrPtr cur) {
    xmlDictPtr dict;

    if ((reader != NULL) && (reader->ctxt != NULL))
	dict = reader->ctxt->dict;
    else
        dict = NULL;
    if (cur == NULL) return;

    if ((__xmlRegisterCallbacks) && (xmlDeregisterNodeDefaultValue))
	xmlDeregisterNodeDefaultValue((xmlNodePtr) cur);

    /* Check for ID removal -> leading to invalid references ! */
    if ((cur->parent != NULL) && (cur->parent->doc != NULL) &&
	((cur->parent->doc->intSubset != NULL) ||
	 (cur->parent->doc->extSubset != NULL))) {
        if (xmlIsID(cur->parent->doc, cur->parent, cur))
	    xmlTextReaderRemoveID(cur->parent->doc, cur);
        if (xmlIsRef(cur->parent->doc, cur->parent, cur))
            xmlTextReaderRemoveRef(cur->parent->doc, cur);
    }
    if (cur->children != NULL)
        xmlTextReaderFreeNodeList(reader, cur->children);

    DICT_FREE(cur->name);
    if ((reader != NULL) && (reader->ctxt != NULL) &&
        (reader->ctxt->freeAttrsNr < 100)) {
        cur->next = reader->ctxt->freeAttrs;
	reader->ctxt->freeAttrs = cur;
	reader->ctxt->freeAttrsNr++;
    } else {
	xmlFree(cur);
    }
}