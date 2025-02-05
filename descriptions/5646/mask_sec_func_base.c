MagickExport void GetNextToken(const char *start,const char **end,
  const size_t extent,char *token)
{
  double
    value;

  register const char
    *p;

  register ssize_t
    i;

  size_t
    length;

  assert(start != (const char *) NULL);
  assert(token != (char *) NULL);
  i=0;
  length=strlen(start);
  p=start;
  while ((isspace((int) ((unsigned char) *p)) != 0) && (*p != '\0'))
    p++;
  // <MASK>
  while (isspace((int) ((unsigned char) *p)) != 0)
    p++;
  if (end != (const char **) NULL)
    *end=(const char *) p;
}