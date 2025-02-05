ATTRIBUTE_NO_SANITIZE_INTEGER
static xmlDictEntry *
xmlDictFindEntry(const xmlDict *dictionary, const xmlChar *prefix,
                 const xmlChar *name, int len, unsigned hashValue,
                 int *pfound) {
    xmlDictEntry *entry;
    unsigned mask, pos, displ;
    int found = 0;

    mask = dictionary->size - 1;
    pos = hashValue & mask;
    entry = &dictionary->table[pos];

    if (entry->hashValue != 0) {
        /*
         * Robin hood hashing: abort if the displacement of the entry
         * is smaller than the displacement of the key we look for.
         * This also stops at the correct position when inserting.
         */
        displ = 0;

        do {
            if (entry->hashValue == hashValue) {
                if (prefix == NULL) {
                    /*
                     * name is not necessarily null-terminated.
                     */
                    if ((strncmp((const char *) entry->name,
                                 (const char *) name, len) == 0) &&
                        (entry->name[len] == 0)) {
                        found = 1;
                        break;
                    }
                } else {
                    if (xmlStrQEqual(prefix, name, entry->name)) {
                        found = 1;
                        break;
                    }
                }
            }

            displ++;
            pos++;
            entry++;
            if ((pos & mask) == 0)
                entry = dictionary->table;
        } while ((entry->hashValue != 0) &&
                 (((pos - entry->hashValue) & mask) >= displ));
    }

    *pfound = found;
    return(entry);
}