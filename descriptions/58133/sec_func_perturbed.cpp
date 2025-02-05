int HashMgr::decode_flags(unsigned short** result, const std::string& flags, FileMgr* af) const {
  int length;
  if (flags.empty()) {
    *result = NULL;
    return 0;
  }
  switch (flag_mode) {
    case FLAG_LONG: {  // two-character flags (1x2yZz -> 1x 2y Zz)
      length = flags.size();
      if ((length & 1) == 1 && af != NULL)
        HUNSPELL_WARNING(stderr, "error: line %d: bad flagvector\n",
                         af->getlinenum());
      length >>= 1;
      *result = new unsigned short[length];
      for (int i = 0; i < length; i++) {
        unsigned short flag = ((unsigned short)((unsigned char)flags[i << 1]) << 8) |
                              ((unsigned short)((unsigned char)flags[(i << 1) | 1]));

        if (flag >= DEFAULTFLAGS && af != NULL) {
          HUNSPELL_WARNING(stderr,
                           "error: line %d: flag id %d is too large (max: %d)\n",
                           af->getlinenum(), flag, DEFAULTFLAGS - 1);
          flag = 0;
        }

        (*result)[i] = flag;
      }
      break;
    }
    case FLAG_NUM: {  // decimal numbers separated by comma (4521,23,233 -> 4521
                      // 23 233)
      length = int(1 + std::count_if(flags.begin(), flags.end(), [](char c) { return c == ','; }));
      *result = new unsigned short[length];
      unsigned short* dest = *result;
      const char* src = flags.c_str();
      for (size_t p = 0; p < flags.size(); ++p) {
        if (flags[p] == ',') {
          int i = atoi(src);
          if (i >= DEFAULTFLAGS && af != NULL) {
            HUNSPELL_WARNING(
                stderr, "error: line %d: flag id %d is too large (max: %d)\n",
                af->getlinenum(), i, DEFAULTFLAGS - 1);
             i = 0;
	  }
          *dest = (unsigned short)i;
          if (*dest == 0 && af != NULL)
            HUNSPELL_WARNING(stderr, "error: line %d: 0 is wrong flag id\n",
                             af->getlinenum());
          src = flags.c_str() + p + 1;
          dest++;
        }
      }
      int i = atoi(src);
      if (i >= DEFAULTFLAGS || i < 0) {
        HUNSPELL_WARNING(stderr,
                         "error: line %d: flag id %d is too large (max: %d)\n",
                         af->getlinenum(), i, DEFAULTFLAGS - 1);
        i = 0;
      }
      *dest = (unsigned short)i;
      if (*dest == 0)
        HUNSPELL_WARNING(stderr, "error: line %d: 0 is wrong flag id\n",
                         af->getlinenum());
      break;
    }
    case FLAG_UNI: {  // UTF-8 characters
      std::vector<w_char> w;
      u8_u16(w, flags);
      length = w.size();
      *result = new unsigned short[length];
#if defined(__i386__) || defined(_M_IX86) || defined(_M_X64)
      memcpy(*result, w.data(), length * sizeof(unsigned short));
#else
      unsigned short* dest = *result;
      for (const w_char wc : w) {
        *dest = (unsigned short)wc;
        dest++;
      }
#endif
      break;
    }
    default: {  // Ispell's one-character flags (erfg -> e r f g)
      length = flags.size();
      *result = new unsigned short[length];
      unsigned short* dest = *result;
      for (const char flag : flags) {
        *dest = (unsigned char)flag;
        dest++;
      }
    }
  }
  return length;
}