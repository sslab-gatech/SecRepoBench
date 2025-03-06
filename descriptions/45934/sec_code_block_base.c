if (LocaleCompare(keyword,"id") == 0)
                  {
                    if (*id != '\0')
                      ThrowMIFFException(CorruptImageError,
                        "ImproperImageHeader");
                    (void) CopyMagickString(id,options,MagickPathExtent);
                    break;
                  }
                if (LocaleCompare(keyword,"iterations") == 0)
                  {
                    image->iterations=StringToUnsignedLong(options);
                    break;
                  }
                (void) SetImageProperty(image,keyword,options,exception);
                break;