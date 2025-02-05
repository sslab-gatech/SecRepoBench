static int
is_full_path(char *path,int joinchar)
{
    unsigned char c = path[0];
    unsigned char secondchar = 0;
    if (!c) {
        /* empty string. */
        return FALSE;
    }
    if (c == joinchar) {
        return TRUE;
    }
    secondchar = path[1];
    if (!secondchar) {
        /* No second character */
        return FALSE;
    }
    if (secondchar == ':') {
        /*  Windows full path, we assume
            We just assume nobody would be silly enough
            to name a linux/posix directory starting with "C:" */
        return TRUE;
    }
    /*  No kind of full path name */
    return FALSE;
}