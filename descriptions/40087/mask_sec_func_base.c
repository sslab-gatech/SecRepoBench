static uint8_t *decrypt_initial_packet(struct ndpi_detection_module_struct *ndpi_struct,
				       struct ndpi_flow_struct *flow,
				       const uint8_t *dest_conn_id, uint8_t dest_conn_id_len,
				       uint8_t source_conn_id_len, uint32_t version,
				       uint32_t *clear_payload_len)
{
  uint64_t token_length, payload_length, packet_number;
  struct ndpi_packet_struct *packet = &ndpi_struct->packet;
  uint8_t first_byte;
  uint32_t pkn32, pn_offset, pkn_len, offset;
  quic_ciphers ciphers; /* Client initial ciphers */
  quic_decrypt_result_t decryption = { 0, 0};
  uint8_t client_secret[HASH_SHA2_256_LENGTH];

  memset(&ciphers, '\0', sizeof(ciphers));
  if(quic_derive_initial_secrets(version, dest_conn_id, dest_conn_id_len,
				 client_secret) != 0) {
    NDPI_LOG_DBG(ndpi_struct, "Error quic_derive_initial_secrets\n");
    return NULL;
  }

  /* Packet numbers are protected with AES128-CTR,
     Initial packets are protected with AEAD_AES_128_GCM. */
  if(!quic_ciphers_prepare(&ciphers, GCRY_MD_SHA256,
                           GCRY_CIPHER_AES128, GCRY_CIPHER_MODE_GCM,
                           client_secret, version)) {
    NDPI_LOG_DBG(ndpi_struct, "Error quic_cipher_prepare\n");
    return NULL;
  }

  /* Type(1) + version(4) + DCIL + DCID + SCIL + SCID */
  pn_offset = 1 + 4 + 1 + dest_conn_id_len + 1 + source_conn_id_len;
  pn_offset += quic_len(&packet->payload[pn_offset], &token_length);
  pn_offset += token_length;
  // <MASK>
}