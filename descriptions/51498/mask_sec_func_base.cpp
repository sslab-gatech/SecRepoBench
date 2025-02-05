struct hentry* AffixMgr::compound_check(const std::string& word,
                                        short wordnum,
                                        short numsyllable,
                                        short maxwordnum,
                                        short wnum,
                                        hentry** words = NULL,
                                        hentry** rwords = NULL,
                                        char hu_mov_rule = 0,
                                        char is_sug = 0,
                                        int* info = NULL) {
  short oldnumsyllable, oldnumsyllable2, oldwordnum, oldwordnum2;
  struct hentry* rv = NULL;
  struct hentry* rv_first;
  std::string st;
  char ch = '\0';
  int cmin;
  int cmax;
  int striple = 0;
  size_t scpd = 0;
  int soldi = 0;
  int oldcmin = 0;
  int oldcmax = 0;
  int oldlen = 0;
  int checkedstriple = 0;
  char affixed = 0;
  hentry** oldwords = words;
  size_t len = word.size();

  int checked_prefix;

  // add a time limit to handle possible
  // combinatorical explosion of the overlapping words

  HUNSPELL_THREAD_LOCAL clock_t timelimit;

  if (wordnum == 0) {
      // get the start time, seeing as we're reusing this set to 0
      // to flag timeout, use clock() + 1 to avoid start clock()
      // of 0 as being a timeout
      timelimit = clock() + 1;
  }
  else if (timelimit != 0 && (clock() > timelimit + TIMELIMIT)) {
      timelimit = 0;
  }

  setcminmax(&cmin, &cmax, word.c_str(), len);

  st.assign(word);

  for (int i = cmin; i < cmax; ++i) {
    // go to end of the UTF-8 character
    if (utf8) {
      for (; (st[i] & 0xc0) == 0x80; i++)
        ;
      if (i >= cmax)
        return NULL;
    }

    words = oldwords;
    int onlycpdrule = (words) ? 1 : 0;

    do {  // onlycpdrule loop

      oldnumsyllable = numsyllable;
      oldwordnum = wordnum;
      checked_prefix = 0;

      do {  // simplified checkcompoundpattern loop

        if (timelimit == 0)
          return 0;

        if (scpd > 0) {
          for (; scpd <= checkcpdtable.size() &&
                 (checkcpdtable[scpd - 1].pattern3.empty() ||
                  word.compare(i, checkcpdtable[scpd - 1].pattern3.size(), checkcpdtable[scpd - 1].pattern3) != 0);
               scpd++)
            ;

          if (scpd > checkcpdtable.size())
            break;  // break simplified checkcompoundpattern loop
          st.replace(i, std::string::npos, checkcpdtable[scpd - 1].pattern);
          soldi = i;
          i += checkcpdtable[scpd - 1].pattern.size();
          st.replace(i, std::string::npos, checkcpdtable[scpd - 1].pattern2);
          st.replace(i + checkcpdtable[scpd - 1].pattern2.size(), std::string::npos,
                 word.substr(soldi + checkcpdtable[scpd - 1].pattern3.size()));

          oldlen = len;
          len += checkcpdtable[scpd - 1].pattern.size() +
                 checkcpdtable[scpd - 1].pattern2.size() -
                 checkcpdtable[scpd - 1].pattern3.size();
          oldcmin = cmin;
          oldcmax = cmax;
          setcminmax(&cmin, &cmax, st.c_str(), len);

          cmax = len - cpdmin + 1;
        }

	// <MASK>

      } while (!onlycpdrule && simplifiedcpd &&
               scpd <= checkcpdtable.size());  // end of simplifiedcpd loop

      scpd = 0;
      wordnum = oldwordnum;
      numsyllable = oldnumsyllable;

      if (soldi != 0) {
        i = soldi;
        st.assign(word);  // XXX add more optim.
        soldi = 0;
      } else
        st[i] = ch;

    } while (!defcpdtable.empty() && oldwordnum == 0 &&
             onlycpdrule++ < 1);  // end of onlycpd loop
  }

  return NULL;
}