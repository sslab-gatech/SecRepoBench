static plist_t parse_primitive(const char* js, jsmntok_info_t* tokeninfo, int* index)
{
    if (tokeninfo->tokens[*index].type != JSMN_PRIMITIVE) {
        PLIST_JSON_ERR("%s: token type != JSMN_PRIMITIVE\n", __func__);
        return NULL;
    }
    plist_t val = NULL;
    const char* str_val = js + tokeninfo->tokens[*index].start;
    const char* str_end = js + tokeninfo->tokens[*index].end;
    size_t str_len = tokeninfo->tokens[*index].end - tokeninfo->tokens[*index].start;
    if (!strncmp("false", str_val, str_len)) {
        val = plist_new_bool(0);
    } else if (!strncmp("true", str_val, str_len)) {
        val = plist_new_bool(1);
    } else if (!strncmp("null", str_val, str_len)) // <MASK>
    (*index)++;
    return val;
}