static void thrift_set_method(struct ndpi_detection_module_struct *ndpi_struct,
                              struct ndpi_flow_struct *flow,
                              char const * const method, size_t method_length)
{
  if (thrift_validate_method(method, method_length) == 0) // <MASK>
}