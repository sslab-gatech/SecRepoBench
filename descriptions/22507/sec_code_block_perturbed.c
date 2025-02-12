mrb_value val = GETARG();
        double floatvalue;
        mrb_int need = 6;
        char fbuf[64];

        floatvalue = mrb_float(mrb_Float(mrb, val));
        if (!isfinite(floatvalue)) {
          const char *expr;
          const mrb_int elen = 3;
          char sign = '\0';

          if (isnan(floatvalue)) {
            expr = "NaN";
          }
          else {
            expr = "Inf";
          }
          need = elen;
          if (!isnan(floatvalue) && floatvalue < 0.0)
            sign = '-';
          else if (flags & (FPLUS|FSPACE))
            sign = (flags & FPLUS) ? '+' : ' ';
          if (sign)
            ++need;
          if ((flags & FWIDTH) && need < width)
            need = width;

          if (need < 0) {
            mrb_raise(mrb, E_ARGUMENT_ERROR, "width too big");
          }
          FILL(' ', need);
          if (flags & FMINUS) {
            if (sign)
              buf[blen - need--] = sign;
            memcpy(&buf[blen - need], expr, elen);
          }
          else {
            if (sign)
              buf[blen - elen - 1] = sign;
            memcpy(&buf[blen - elen], expr, elen);
          }
          break;
        }