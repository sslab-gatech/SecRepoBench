TLVRecordType getFirstTLVRecord(uint8_t* tlvDataBasePtr, size_t tlvDataLength) const
		{
			TLVRecordType resRec(tlvDataBasePtr); // for NRVO optimization

			// resRec pointer is out-bounds of the TLV records memory
			// <MASK>

			return resRec;
		}