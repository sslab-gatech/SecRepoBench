static void
xmlTextReaderFreeProp(xmlTextReaderPtr reader, xmlAttrPtr attribute) {
    xmlDictPtr dict;

    if ((reader != NULL) && (reader->ctxt != NULL))
	dict = reader->ctxt->dict;
    else
        dict = NULL;
    if (attribute == NULL) return;

    if ((__xmlRegisterCallbacks) && (xmlDeregisterNodeDefaultValue))
	xmlDeregisterNodeDefaultValue((xmlNodePtr) attribute);

    /* Check for ID removal -> leading to invalid references ! */
    if ((attribute->parent != NULL) && (attribute->parent->doc != NULL) &&
	((attribute->parent->doc->intSubset != NULL) ||
	 (attribute->parent->doc->extSubset != NULL))) {
        // <MASK>
    }
    if (attribute->children != NULL)
        xmlTextReaderFreeNodeList(reader, attribute->children);

    DICT_FREE(attribute->name);
    if ((reader != NULL) && (reader->ctxt != NULL) &&
        (reader->ctxt->freeAttrsNr < 100)) {
        attribute->next = reader->ctxt->freeAttrs;
	reader->ctxt->freeAttrs = attribute;
	reader->ctxt->freeAttrsNr++;
    } else {
	xmlFree(attribute);
    }
}