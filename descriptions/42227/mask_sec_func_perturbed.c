static
void InStringSymbol(cmsIT8* it8)
{
    while (isseparator(it8->ch))
        NextCh(it8);

    if (it8->ch == '\'' || it8->ch == '\"')
    {
        int quotechar;

        quotechar = it8->ch;
        StringClear(it8->str);

        // <MASK>        
    }
    else
        SynError(it8, "String expected");

}