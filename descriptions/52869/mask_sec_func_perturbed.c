static int
xmlXIncludeDoProcess(xmlXIncludeCtxtPtr ctxt, xmlNodePtr tree) {
    xmlXIncludeRefPtr ref;
    xmlNodePtr cur;
    int ret = 0;
    int index;

    if ((tree == NULL) || (tree->type == XML_NAMESPACE_DECL))
	return(-1);
    if (ctxt == NULL)
	return(-1);

    /*
     * First phase: lookup the elements in the document
     */
    cur = tree;
    do {
	/* TODO: need to work on entities -> stack */
        if (xmlXIncludeTestNode(ctxt, cur) == 1) {
// <MASK>
            /*
             * Mark direct includes.
             */
            if (ref != NULL)
                ref->replace = 1;
        } else if ((cur->children != NULL) &&
                   ((cur->type == XML_DOCUMENT_NODE) ||
                    (cur->type == XML_ELEMENT_NODE))) {
            cur = cur->children;
            continue;
        }
        do {
            if (cur == tree)
                break;
            if (cur->next != NULL) {
                cur = cur->next;
                break;
            }
            cur = cur->parent;
        } while (cur != NULL);
    } while ((cur != NULL) && (cur != tree));

    /*
     * Second phase: extend the original document infoset.
     */
    for (index = 0; index < ctxt->incNr; index++) {
	if (ctxt->incTab[index]->replace != 0) {
            if ((ctxt->incTab[index]->inc != NULL) ||
                (ctxt->incTab[index]->emptyFb != 0)) {	/* (empty fallback) */
                xmlXIncludeIncludeNode(ctxt, ctxt->incTab[index]);
                ctxt->incTab[index]->replace = 0;
            }
        } else {
            /*
             * Ignore includes which were added indirectly, for example
             * inside xi:fallback elements.
             */
            if (ctxt->incTab[index]->inc != NULL) {
                xmlFreeNodeList(ctxt->incTab[index]->inc);
                ctxt->incTab[index]->inc = NULL;
            }
        }
	ret++;
    }

    return(ret);
}