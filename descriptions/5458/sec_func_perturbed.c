static char *ParseEntities(char *xmlstring,char **entities,int state)
{
  char
    *entity;

  int
    byte,
    c;

  register char
    *p,
    *q;

  register ssize_t
    i;

  size_t
    extent,
    length;

  ssize_t
    offset;

  /*
    Normalize line endings.
  */
  p=xmlstring;
  q=xmlstring;
  for ( ; *xmlstring != '\0'; xmlstring++)
    while (*xmlstring == '\r')
    {
      *(xmlstring++)='\n';
      if (*xmlstring == '\n')
        (void) CopyMagickMemory(xmlstring,xmlstring+1,strlen(xmlstring));
    }
  for (xmlstring=p; ; )
  {
    while ((*xmlstring != '\0') && (*xmlstring != '&') && ((*xmlstring != '%') ||
           (state != '%')) && (isspace((int) ((unsigned char) *xmlstring) == 0)))
      xmlstring++;
    if (*xmlstring == '\0')
      break;
    /*
      States include:
        '&' for general entity decoding
        '%' for parameter entity decoding
        'c' for CDATA sections
        ' ' for attributes normalization
        '*' for non-CDATA attributes normalization
    */
    if ((state != 'c') && (strncmp(xmlstring,"&#",2) == 0))
      {
        /*
          Character reference.
        */
        if (xmlstring[2] != 'x')
          c=strtol(xmlstring+2,&entity,10);  /* base 10 */
        else
          c=strtol(xmlstring+3,&entity,16);  /* base 16 */
        if ((c == 0) || (*entity != ';'))
          {
            /*
              Not a character reference.
            */
            xmlstring++;
            continue;
          }
        if (c < 0x80)
          *(xmlstring++)=c;
        else
          {
            /*
              Multi-byte UTF-8 sequence.
            */
            byte=c;
            for (i=0; byte != 0; byte/=2)
              i++;
            i=(i-2)/5;
            *xmlstring=(char) ((0xFF << (7-i)) | (c >> (6*i)));
            xmlstring++;
            while (i != 0)
            {
              i--;
              *xmlstring=(char) (0x80 | ((c >> (6*i)) & 0x3F));
              xmlstring++;
            }
          }
        (void) CopyMagickMemory(xmlstring,strchr(xmlstring,';')+1,strlen(strchr(xmlstring,';')));
      }
    else
      if (((*xmlstring == '&') && ((state == '&') || (state == ' ') ||
          (state == '*'))) || ((state == '%') && (*xmlstring == '%')))
        {
          /*
            Find entity in the list.
          */
          i=0;
          while ((entities[i] != (char *) NULL) &&
                 (strncmp(xmlstring+1,entities[i],strlen(entities[i])) != 0))
            i+=2;
          if (entities[i++] == (char *) NULL)
            xmlstring++;
          else
            if (entities[i] != (char *) NULL)
              {
                /*
                  Found a match.
                */
                length=strlen(entities[i]);
                entity=strchr(xmlstring,';');
                if ((entity != (char *) NULL) &&
                    ((length-1L) >= (size_t) (entity-xmlstring)))
                  {
                    offset=(ssize_t) (xmlstring-p);
                    extent=(size_t) (offset+length+strlen(entity));
                    if (p != q)
                      p=(char *) ResizeQuantumMemory(p,extent+1,sizeof(*p));
                    else
                      {
                        char
                          *extent_xml;

                        extent_xml=(char *) AcquireQuantumMemory(extent+1,
                          sizeof(*extent_xml));
                        if (extent_xml != (char *) NULL)
                          (void) CopyMagickString(extent_xml,p,extent*
                            sizeof(*extent_xml));
                        p=extent_xml;
                      }
                    if (p == (char *) NULL)
                      ThrowFatalException(ResourceLimitFatalError,
                        "MemoryAllocationFailed");
                    p[extent]='\0';
                    xmlstring=p+offset;
                    entity=strchr(xmlstring,';');
                  }
                if (entity != (char *) NULL)
                  (void) CopyMagickMemory(xmlstring+length,entity+1,strlen(entity));
                (void) strncpy(xmlstring,entities[i],length);
              }
        }
      else
        if (((state == ' ') || (state == '*')) &&
            (isspace((int) ((unsigned char) *xmlstring) != 0)))
          *(xmlstring++)=' ';
        else
          xmlstring++;
  }
  if (state == '*')
    {
      /*
        Normalize spaces for non-CDATA attributes.
      */
      for (xmlstring=p; *xmlstring != '\0'; xmlstring++)
      {
        char
          accept[] = " ";

        i=(ssize_t) strspn(xmlstring,accept);
        if (i != 0)
          (void) CopyMagickMemory(xmlstring,xmlstring+i,strlen(xmlstring+i)+1);
        while ((*xmlstring != '\0') && (*xmlstring != ' '))
          xmlstring++;
      }
      xmlstring--;
      if ((xmlstring >= p) && (*xmlstring == ' '))
        *xmlstring='\0';
    }
  return(p == q ? ConstantString(p) : p);
}