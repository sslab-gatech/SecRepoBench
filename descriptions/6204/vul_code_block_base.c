char
        colorspace[MagickPathExtent];

      MagickBooleanType
        icc_color;

      /*
        Parse color of the form rgb(100,255,0).
      */
      (void) CopyMagickString(colorspace,name,MagickPathExtent);
      for (i=0; colorspace[i] != '\0'; i++)
        if (colorspace[i] == '(')
          break;
      colorspace[i--]='\0';