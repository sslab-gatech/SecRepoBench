void map_coords (int *coords, unsigned int coords_length) const
  {
    unsigned int count = hb_min (coords_length, axisCount);

    const SegmentMaps *map = &firstAxisSegmentMaps;
    for (unsigned int i = 0; i < count; i++)
    {
      coords[i] = map->map (coords[i]);
      map = &StructAfter<SegmentMaps> (*map);
    }

#ifndef HB_NO_VARIATIONS2
    if (version.major < 2)
      return;

    // <MASK>
  }