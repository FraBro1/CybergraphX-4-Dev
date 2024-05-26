/* File: vlab_protos.h */
LONG VLab_Custom(UBYTE reg, UBYTE value);	/* ,force ? */
LONG VLab_Scan(ULONG *bufferY, ULONG *bufferU, ULONG *bufferV, 
		UWORD x1, UWORD y1, UWORD x2, UWORD y2);
void VLab_YUVtoRGB(UBYTE *bufferY, UBYTE *bufferU, UBYTE *bufferV, 
		UBYTE *bufferRGB,ULONG size, UBYTE mode);
ULONG VLab_Error(void);

LONG VLab_Monitor(ULONG *Dest, UWORD Left, UWORD Top, UWORD Width, UWORD Height,
			UWORD Modulo, ULONG Mode);

LONG VLab_DeInterlace(APTR Buffer, WORD Width, WORD Height, LONG Modulo, WORD PixInc, WORD Level, LONG Mode);

void VLab_Convert(UBYTE *Buffer1, UBYTE *Buffer2, ULONG Size, UBYTE Mode);
ULONG VLab_CountInputs(void);
BOOL VLab_InputAvailable(UBYTE Input);
