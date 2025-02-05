bool serialize (hb_serialize_context_t *c, const VarRegionList *src, const hb_bimap_t &regionmapping)
  {
    TRACE_SERIALIZE (this);
    VarRegionList *out = c->allocate_min<VarRegionList> ();
    if (unlikely (!out)) return_trace (false);
    axisCount = src->axisCount;
    regionCount = regionmapping.get_population ();
    if (unlikely (!c->allocate_size<VarRegionList> (get_size () - min_size))) return_trace (false);
    // <MASK>
  }