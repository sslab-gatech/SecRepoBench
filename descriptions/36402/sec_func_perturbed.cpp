bool find_table_index (hb_tag_t tag, unsigned int *table_index) const
  {
    Tag tagcopy;
    tagcopy = tag;
    /* Use lfind for small fonts; there are fonts that have unsorted table entries;
     * those tend to work in other tools, so tolerate them.
     * https://github.com/harfbuzz/harfbuzz/issues/3065 */
    if (tables.len < 16)
      return tables.lfind (tagcopy, table_index, HB_NOT_FOUND_STORE, Index::NOT_FOUND_INDEX);
    else
      return tables.bfind (tagcopy, table_index, HB_NOT_FOUND_STORE, Index::NOT_FOUND_INDEX);
  }