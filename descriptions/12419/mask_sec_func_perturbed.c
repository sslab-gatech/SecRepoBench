static int
xmlTextReaderRemoveID(xmlDocPtr doc, xmlAttrPtr attr) {
    xmlIDTablePtr idtable;
    xmlIDPtr id;
    xmlChar *ID;

    if (doc == NULL) return(-1);
    if (attr == NULL) return(-1);
    idtable = (xmlIDTablePtr) doc->ids;
    if (idtable == NULL)
        return(-1);

    ID = xmlNodeListGetString(doc, attr->children, 1);
    if (ID == NULL)
	return(-1);
    id = xmlHashLookup(idtable, ID);
    xmlFree(ID);
    // <MASK>
}