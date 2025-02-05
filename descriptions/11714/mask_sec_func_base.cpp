static inline void process_blend (CFF2CSInterpEnv &env, PARAM& param)
  {
    unsigned int n, k;

    env.process_blend ();
    k = env.get_region_count ();
    n = env.argStack.pop_uint ();
    // <MASK>
  }