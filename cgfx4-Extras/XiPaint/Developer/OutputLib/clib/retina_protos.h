/*
** This C prototypes file was generated automatically
** from a document file
** using doc2protos written by Claus Bönnhoff.
*/

struct RetinaMonitor * Retina_FilterMonitor(struct RetinaMonitor *,
                                                    struct TagItem *);
struct RetinaScreenMode * Retina_AllocRequest(struct TagItem *);
BOOL Retina_Request(struct RetinaScreenMode *,struct TagItem *);
void Retina_FreeRequest(struct RetinaScreenMode *);
struct RetinaScreen * Retina_AllocScreen(UWORD,UWORD,ULONG,ULONG,APTR);
void Retina_FreeScreen(struct RetinaScreen *);
void Retina_CopyRect(struct RetinaScreen *,UWORD,UWORD,UWORD,UWORD,
                     struct RetinaScreen *,UWORD,UWORD,ULONG);
void Retina_AddVBIntServer(struct Interrupt *);
void Retina_RemVBIntServer(struct Interrupt *);
void Retina_DrawTestImage(struct RetinaScreen *);
void Retina_DisplayAlert(UBYTE *, ULONG);
void Retina_DisplayOff( void );
void Retina_DisplayOn( void );
BOOL Retina_WriteBitMap(struct RetinaScreen *,struct BitMap *,UBYTE *,
                        UWORD,UWORD,UWORD,ULONG,UWORD,UWORD,UWORD);
void Retina_SetFont(struct RetinaScreen *,struct TextFont *);
void Retina_Text(struct RetinaScreen *,UBYTE *,UWORD,UWORD);
void Retina_OwnRetina( void );
void Retina_DisownRetina( void );
void Retina_WriteRect(UBYTE *,UWORD,UWORD,UWORD,ULONG,
               struct RetinaScreen *,UWORD,UWORD,UWORD,UWORD,UBYTE *);
void Retina_ReadRect(UBYTE *,UWORD,UWORD,UWORD,ULONG,
                     struct RetinaScreen *,UWORD,UWORD,UWORD,UWORD,UBYTE *);
void Retina_SwapRect(UBYTE *,UWORD,UWORD,UWORD,ULONG,
                     struct RetinaScreen *,UWORD,UWORD,UWORD,UWORD);
void Retina_SwapBitMap(struct RetinaScreen *);
ULONG Retina_SetSprite(struct RetinaScreen *, ULONG *,UWORD,APTR);
void Retina_MoveSprite(struct RetinaScreen *,UWORD,UWORD);
void Retina_SpriteFunction(struct RetinaScreen *,struct TagItem *);
ULONG Retina_AvailMem(ULONG);
UBYTE * Retina_AllocVec(ULONG,ULONG);
void Retina_FreeVec(UBYTE *);
UBYTE * Retina_AllocBitMap(UWORD,UWORD,UWORD,ULONG);
void Retina_FreeBitMap(UBYTE *);
void Retina_Circle(struct RetinaScreen *,UWORD,UWORD,UWORD);
void Retina_CircleFill(struct RetinaScreen *,UWORD,UWORD,UWORD);
void Retina_WritePixel(struct RetinaScreen *,UWORD,UWORD);
void Retina_SetPixel(struct RetinaScreen *,UWORD,UWORD,ULONG);
ULONG Retina_ReadPixel(struct RetinaScreen *,UWORD,UWORD);
void Retina_Line(struct RetinaScreen *,UWORD,UWORD,UWORD,UWORD);
void Retina_RectFill(struct RetinaScreen *,UWORD,UWORD,UWORD,UWORD);
void Retina_EraseRect(struct RetinaScreen *,UWORD,UWORD,UWORD,UWORD);
void Retina_SetAPen(struct RetinaScreen *,ULONG);
void Retina_SetDrMd(struct RetinaScreen *,ULONG);
ULONG Retina_GetAPen(struct RetinaScreen *);
ULONG Retina_GetDefaultScreenInfo(ULONG,struct _DefaultScreenInfo *);
void Retina_ScreenToBack(struct RetinaScreen *);
void Retina_ScreenToFront(struct RetinaScreen *);
void Retina_MoveScreen(struct RetinaScreen *,UWORD,UWORD);
UBYTE * Retina_SetSegmentPtr(UBYTE *);
void Retina_WaitTOF(void);
void Retina_WaitBLT(void);
struct Retina_RetinaMonitor * Retina_AddMonitor(struct NewRetinaMonitor *);
BOOL Retina_RemMonitor(struct RetinaMonitor *);
struct RetinaMonitor * Retina_FindMonitor(ULONG);
struct RetinaMonitor * Retina_NextMonitor(struct RetinaMonitor *);
void Retina_SetPalette(struct RetinaScreen *,UWORD,UBYTE,UBYTE,UBYTE);
void Retina_LoadPalette(struct RetinaScreen *,UWORD,UWORD,UBYTE*);
struct RetinaScreen * Retina_OpenScreen(UWORD,UWORD,ULONG,ULONG,APTR);
void Retina_CloseScreen(struct RetinaScreen *);
void Retina_ClearScreen(struct RetinaScreen *);
UWORD Retina_GetCRC(void);
void Retina_DisplayControl(struct TagItem *);
APTR Retina_AllocBrush(struct TagItem *);
void Retina_FreeBrush(APTR);
void Retina_MoveBrush(APTR,UWORD,UWORD);
