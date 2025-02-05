bool serialize (hb_serialize_context_t *c,
                  Iterator it)
  {
    TRACE_SERIALIZE (this);
    if (unlikely (!c->extend_min (*this))) return_trace (false);

    if (unlikely (!it))
    {
      startGlyph = 0;
      classValue.len = 0;
      return_trace (true);
    }

    hb_codepoint_t glyph_min = (*it).first;
    hb_codepoint_t glyph_max = + it
                               | hb_map (hb_first)
                               | hb_reduce (hb_max, 0u);
    // <MASK>
    return_trace (true);
  }