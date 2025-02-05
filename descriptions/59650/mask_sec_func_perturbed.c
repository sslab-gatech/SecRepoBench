TLVRecordType getNextTLVRecord(TLVRecordType& record, const uint8_t* tlvBufferPtr, size_t tlvDataLen) const
		{
			TLVRecordType resRec(NULL); // for NRVO optimization

			if (record.isNull())
				return resRec;

			// <MASK>

			return resRec;
		}