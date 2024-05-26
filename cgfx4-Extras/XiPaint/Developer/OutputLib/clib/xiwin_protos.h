/* Last Change: Herbert / 21. August 1995  19:20:46 */

/****  Verwaltung  ****/

VOID XW_SetLibParams(struct WinSet *NewSet);
ULONG XW_SetReg(ULONG Tag, ULONG Data);


/****  Window  ****/

struct Display *XW_OpenDisplay(struct xo_init *Hardware, BOOL BackPaint);
VOID XW_CloseDisplay(struct Display *Display);

struct Box *XW_InitBox(SHORT X, SHORT Y, SHORT Width, SHORT Height, SHORT Depth, unsigned char *Title, LONG BoxID);
SHORT XW_OpenBox(struct Display *Display, struct Box *NewBox);
void XW_CloseBox(struct Box *Box);
SHORT XW_BoxToFront(struct Box *Box);
SHORT XW_BoxToBack(struct Box *Box);
SHORT XW_ChangeBox(struct Box *Box, SHORT NewX, SHORT NewY, SHORT NewWidth, SHORT NewHeight);
void XW_ActivateBox(struct Box *Box);

void XW_ClearBox(struct Box *Box);
SHORT XW_RotateBox(struct Box *Box);

BOOL XW_AnyBoxOpen(struct Display *Display);
BOOL XW_IsBoxOpen(struct Display *Display, struct Box *Box);

void XW_ActivateButton(struct Box *Box, struct Button *Button);


/****  Button  ****/

struct Button *XW_AddNormal(struct Box *Box, struct NewButton *NewButton, SHORT ButtonMode);
struct Button *XW_AddImage(struct Box *Box, struct NewButton *NewButton, SHORT ButtonMode);
struct Button *XW_AddString(struct Box *Box, struct NewButton *NewButton);
struct Button *XW_AddNumeric(struct Box *Box, struct NewButton *NewButton);
struct Button *XW_AddDNumeric(struct Box *Box, struct NewButton *NewButton);
struct Button *XW_AddScroll(struct Box *Box, struct NewButton *NewButton, SHORT ButtonMode);
struct Button *XW_AddListView(struct Box *Box, struct NewButton *NewButton);
struct Button *XW_AddCycle(struct Box *Box, struct NewButton *NewButton);
struct Button *XW_AddCheck(struct Box *Box, struct NewButton *NewButton);

SHORT XW_AddKeep(struct Box *Box, struct Button *Button);

struct DropArea *XW_AddDropArea(struct Box *Box, LONG AreaID, SHORT X, SHORT Y, SHORT Width, SHORT Height);
VOID XW_RemoveDropArea(struct Box *Box, struct DropArea *DropArea);

void XW_RemoveButton(struct Box *Box, struct Button *Button);
void XW_FreeButton(struct Button *Button);

char *XW_GetLVResult(struct Button *ListView);

short XW_AddLVEntry(struct Box *Box, struct Button *Button, char *Text, UBYTE Color, UBYTE Pri);
void XW_RemoveLVEntry(struct Box *Box, struct Button *Button, char *Text);
void XW_RemoveAllLVEntry(struct Box *Box, struct Button *Button);
void XW_SortLV(struct Box *Box, struct Button *Button);

void XW_SetNormal(struct Box *Box, struct Button *Button, BOOL Down);
void XW_SetString(struct Box *Box, struct Button *Button, char *NewText);
void XW_SetNumeric(struct Box *Box, struct Button *Button, LONG NewValue);
void XW_SetDNumeric(struct Box *Box, struct Button *Button, double *NewValue);
void XW_SetScroll(struct Box *Box, struct Button *Button, LONG NewValue);
void XW_SetListView(struct Box *Box, struct Button *Button, SHORT NewPos, SHORT Mode);
void XW_SetCycle(struct Box *Box, struct Button *Button, SHORT NewNum);
void XW_SetCheck(struct Box *Box, struct Button *Button, BOOL Checked);

void XW_RefreshButtons(struct Box *Box);


/****  Menu  ****/

struct BMenu *XW_AddMenu(struct Box *Box, char *MenuTitle);
struct BItem *XW_AddItem(struct BMenu *Menu, char *ItemText, SHORT Mode, LONG ItemID, UBYTE ShortKey, USHORT ShortQual);
struct BSubItem *XW_AddSubItem(struct BItem *Item, char *SubText, SHORT Mode, LONG SubID, UBYTE ShortKey, USHORT ShortQual);
void XW_RemoveMenu(struct BMenu *Menu);
void XW_RemoveItem(struct BItem *Item);
void XW_RemoveSubItem(struct BSubItem *SubItem);

void XW_SetMenu(struct Box *Box);
void XW_SetOtherMenu(struct Box *Box, struct Box *OtherBox);
void XW_ClearMenu(struct Box *Box);

void XW_DisableAllMenus(struct Box *Box);
void XW_DisableMenu(struct BMenu *Menu);
void XW_DisableItem(struct BItem *Item);
void XW_DisableSubItem(struct BSubItem *SubItem);
void XW_EnableAllMenus(struct Box *Box);
void XW_EnableMenu(struct BMenu *Menu);
void XW_EnableItem(struct BItem *Item);
void XW_EnableSubItem(struct BSubItem *SubItem);

void XW_CheckItem(struct BItem *Item, BOOL State);
void XW_CheckSubItem(struct BSubItem *SubItem, BOOL State);


/****  Update  ****/

void XW_UpdateBox(struct Box *Box, SHORT X, SHORT Y, SHORT Width, SHORT Height);
void XW_UpdateFrame(struct Display *Display, SHORT X, SHORT Y, SHORT Width, SHORT Height);

void XW_CopyAreaBox(struct Box *Box, SHORT SrcX, SHORT SrcY, SHORT Width, SHORT Height, SHORT DestX, SHORT DestY);

SHORT XW_InitUpdateHook(struct Box *Box);
VOID XW_DeleteUpdateHook(struct Box *Box);
VOID XW_SetUpdateHook(SHORT HookNr, __far void (* __asm HookPtr)(register __a0 struct Box *, register __d0 SHORT, register __d1 SHORT, register __d2 SHORT, register __d3 SHORT));


/****  Rubber  ****/

VOID XW_MoveMouse(struct Display *Display);

VOID XW_RefreshMouse(struct Display *Display);

VOID XW_DeleteMouse(struct Display *Display);
VOID XW_DrawMouse(struct Display *Display);

VOID XW_OnMouse(struct Display *Display);
VOID XW_OffMouse(struct Display *Display);

VOID XW_SetMouseBox(struct Box *Box, long MouseType, SHORT x1, SHORT y1, SHORT x2, SHORT y2);
VOID XW_SetMouseDisplay(struct Display *Display, long MouseType, SHORT x1, SHORT y1, SHORT x2, SHORT y2);
VOID XW_SetStdMouse(struct Box *Box, BOOL Mode);

VOID XW_WorkPointer(struct Display *Display);
VOID XW_WaitPointer(struct Display *Display);

VOID XW_RectOn2(struct Display *Display, SHORT x1, SHORT y1, SHORT x2, SHORT y2);


/****  Mouse  ****/

VOID XW_SetTicks(SHORT TicksPerSec);

VOID XW_GetEvent(struct Display *Display, struct Event *Event, UWORD ActionFlags);
VOID XW_PeekEvent(struct Display *Display, struct Event *Event, UWORD ActionFlags);
VOID XW_FlushEvents(struct Display *Display);

VOID XW_SetBoxCoord(struct Box *Box, SHORT MouseX, SHORT MouseY);
