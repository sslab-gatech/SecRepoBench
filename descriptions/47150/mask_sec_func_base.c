static int
is_full_path(char *path,int joinchar)
{
    unsigned char c = path[0];
    unsigned char c1 = 0;
    // <MASK>
    if (c1 == ':') {
        /*  Windows full path, we assume
            We just assume nobody would be silly enough
            to name a linux/posix directory starting with "C:" */
        return TRUE;
    }
    /*  No kind of full path name */
    return FALSE;
}