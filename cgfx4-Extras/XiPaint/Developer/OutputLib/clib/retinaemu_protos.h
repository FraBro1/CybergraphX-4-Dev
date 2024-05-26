/*
** This C prototypes file was generated automatically
** from a document file
** using doc2protos written by Claus Bönnhoff.
*/

struct Screen *RE_OpenDeepScreen(struct NewScreen *,
       struct TagItem *, struct TagItem *);
BOOL RE_CloseDeepScreen( struct Screen * );
void RE_WriteRect(UBYTE *,UWORD,UWORD,UWORD,ULONG,
               struct RastPort *,UWORD,UWORD,UWORD,UWORD,UBYTE *);
struct RetinaScreen * RE_GetRetinaScreen(struct BitMap *);
