for (x=0; x < (ssize_t) (number_pixels-2); x+=3)
          {
            p=PushLongPixel(quantum_info->endian,p,&pixel);
            SetPixelGray(image,ScaleAnyToQuantum((pixel >> 2) & 0x3ff,range),
              q);
            q+=GetPixelChannels(image);
            SetPixelGray(image,ScaleAnyToQuantum((pixel >> 12) & 0x3ff,range),
              q);
            q+=GetPixelChannels(image);
            SetPixelGray(image,ScaleAnyToQuantum((pixel >> 22) & 0x3ff,range),
              q);
            p+=quantum_info->pad;
            q+=GetPixelChannels(image);
          }
          if (x++ < (ssize_t) (number_pixels-1))
            {
              p=PushLongPixel(quantum_info->endian,p,&pixel);
              SetPixelGray(image,ScaleAnyToQuantum((pixel >> 2) & 0x3ff,
                range),q);
              q+=GetPixelChannels(image);
            }
          if (x++ < (ssize_t) number_pixels)
            {
              SetPixelGray(image,ScaleAnyToQuantum((pixel >> 12) & 0x3ff,
                range),q);
              q+=GetPixelChannels(image);
            }
          break;