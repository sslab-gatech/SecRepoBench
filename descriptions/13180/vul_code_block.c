uint32
    count,
    length,
    type;

  unsigned long
    *tietz;

  if ((TIFFGetField(tiff,TIFFTAG_ARTIST,&text) == 1) &&
      (text != (char *) NULL))
    (void) SetImageProperty(image,"tiff:artist",text,exception);
  if ((TIFFGetField(tiff,TIFFTAG_COPYRIGHT,&text) == 1) &&
      (text != (char *) NULL))
    (void) SetImageProperty(image,"tiff:copyright",text,exception);