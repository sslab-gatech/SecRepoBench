MagickExport void GetNextToken(const char *begin,const char **end,
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

  assert(begin != (const char *) NULL);
  assert(token != (char *) NULL);
  i=0;
  length=strlen(begin);
  p=begin;
  while ((isspace((int) ((unsigned char) *p)) != 0) && (*p != '\0'))
    p++;
  // <MASK>
  while (isspace((int) ((unsigned char) *p)) != 0)
    p++;
  if (end != (const char **) NULL)
    *end=(const char *) p;
}