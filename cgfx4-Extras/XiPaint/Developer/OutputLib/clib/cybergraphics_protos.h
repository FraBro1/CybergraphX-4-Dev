#ifndef CLIB_CYBERGRAPHICS_H
#define CLIB_CYBERGRAPHICS_H

#ifndef UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif

struct List *AllocCModeListTagList(struct TagItem *);
struct List *AllocCModeListTags(Tag, ...);
ULONG        BestCModeIDTagList(struct TagItem *);
ULONG        BestCModeIDTags(Tag, ...);
ULONG        CModeRequestTagList(APTR, struct TagItem *);
ULONG        CModeRequestTags(APTR, Tag, ...);
void         DoCDrawMethodTagList(struct Hook *, struct RastPort *, struct TagItem *);
void         DoCDrawMethodTags(struct Hook *, struct RastPort *, Tag, ...);
ULONG        FillPixelArray(struct RastPort *, UWORD, UWORD, UWORD, UWORD, ULONG);
void         FreeCModeList(struct List *);
ULONG        GetCyberIDAttr(ULONG, ULONG);
ULONG        GetCyberMapAttr(struct BitMap *, ULONG);
ULONG        InvertPixelArray(struct RastPort *, UWORD, UWORD, UWORD, UWORD);
BOOL         IsCyberModeID(ULONG);
ULONG        MovePixelArray(UWORD, UWORD, struct RastPort *, UWORD, UWORD, UWORD,
                            UWORD);
ULONG        ReadPixelArray(APTR, UWORD, UWORD, UWORD, struct RastPort *, UWORD,
                            UWORD, UWORD, UWORD, UBYTE);
ULONG        SwapPixelArray(APTR, UWORD, UWORD, UWORD, struct RastPort *, UWORD,
                            UWORD, UWORD, UWORD, UBYTE);
ULONG        ReadRGBPixel(struct RastPort *, UWORD, UWORD);
ULONG        CVideoCtrlTagList(struct ViewPort *, struct TagItem *);
ULONG        CVideoCtrlTags(struct ViewPort *, Tag tag1, ...);
ULONG        WritePixelArray(APTR, UWORD, UWORD, UWORD, struct RastPort *, UWORD,
                             UWORD, UWORD, UWORD, UBYTE);
LONG         WriteRGBPixel(struct RastPort *, UWORD, UWORD, ULONG);
#endif
