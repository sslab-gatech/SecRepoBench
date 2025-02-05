static const char* str_table_entry(
    const char* str_table_base,
    const char* str_table_limit,
    int index)
{
  // <MASK>

  if (str_entry >= str_table_limit)
    return NULL;

  len = strnlen(str_entry, str_table_limit - str_entry);

  // Entry is clamped by extent of string table, not null-terminated.
  if (str_entry + len == str_table_limit)
    return NULL;

  return str_entry;
}