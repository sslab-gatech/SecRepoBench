bool serialize (hb_serialize_context_t *c, const VarRegionList *src, const hb_bimap_t &region_map)
  {
    TRACE_SERIALIZE (this);
    VarRegionList *out = c->allocate_min<VarRegionList> ();
    if (unlikely (!out)) return_trace (false);
    axisCount = src->axisCount;
    regionCount = region_map.get_population ();
    if (unlikely (!c->allocate_size<VarRegionList> (get_size () - min_size))) return_trace (false);
    // <MASK>
  }