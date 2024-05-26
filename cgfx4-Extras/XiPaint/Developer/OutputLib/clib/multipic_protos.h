/*
** This C prototypes file was generated automatically
** from a document file
** using doc2protos written by Claus Bönnhoff.
*/

struct FormatInfo * MP_NextFormatInfo(struct FormatInfo *,ULONG,ULONG);
void MP_SetImageAttrs(struct PicHandle *, struct TagItem *);
void MP_Fault(ULONG, APTR, ULONG);
ULONG MP_IoErr(void);
ULONG MP_GetIoErr(struct PicHandle *);
struct PicInfo * MP_Info(struct PicHandle *);
struct PicHandle * MP_Open(UBYTE *, ULONG);
void MP_Close(struct PicHandle *);
BOOL MP_SetBufferAttrs(struct PicHandle *,struct TagItem *);
BOOL MP_Read(struct PicHandle *,APTR,APTR,APTR,APTR,UWORD);
BOOL MP_Write(struct PicHandle *,APTR,APTR,APTR,APTR,UWORD);
APTR MP_ReadPalette(struct PicHandle *,APTR,ULONG);
BOOL MP_WritePalette(struct PicHandle *,APTR,ULONG);
BOOL MP_SetProp(struct PicHandle *, ULONG);
BOOL MP_WriteProp(struct PicHandle *, APTR, ULONG, ULONG, ULONG);
struct StoredProperty * MP_FindProp(struct PicHandle *, ULONG);
