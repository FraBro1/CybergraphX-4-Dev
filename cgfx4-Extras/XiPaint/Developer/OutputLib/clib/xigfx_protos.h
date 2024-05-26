/* Last Change from Thomas: Sun Jul 24 11:14:05 1994 */
/* File: xigfx_prots.h */

VOID XG_DrawBoxBorder(struct Box *Box, SHORT mode);
VOID XG_DrawButton(struct Box *Box, struct Button *Button);
VOID XG_Point(struct Box *Box, SHORT x, SHORT y, UBYTE reg);
UBYTE XG_Get (struct Box *Box, SHORT x, SHORT y);
VOID XG_Line(struct Box *Box, SHORT x0, SHORT y0, SHORT x1, SHORT y1, UBYTE reg);
VOID XG_Box(struct Box *Box, SHORT x0, SHORT y0, SHORT x1, SHORT y1, UBYTE reg);
VOID XG_FBox(struct Box *Box, SHORT x0, SHORT y0, SHORT x1, SHORT y1, UBYTE reg);
VOID XG_FBox_True(struct Box *Box, SHORT x0, SHORT y0, SHORT x1, SHORT y1, union color *rgb);
VOID XG_Brush(struct Box *Box, SHORT x0, SHORT y0, SHORT width, SHORT height, UBYTE *memory);
VOID XG_SetPalette (LONG *c24, UBYTE *c8, LONG mode);
LONG XG_SetRegister(LONG entry, LONG data);
SHORT XG_Char(struct Box *Box, unsigned char Letter, SHORT x, SHORT y, UBYTE reg);
SHORT XG_Text(struct Box *Box, unsigned char *string, SHORT x, SHORT y, UBYTE reg);
SHORT XG_TextLen(struct Box *Box, unsigned char *string, SHORT x, SHORT y, UBYTE reg, SHORT w);
VOID XG_Underline(struct Box *Box, char z, char *text, WORD x0, WORD y0, UBYTE pen);
SHORT XG_OpenFont(VOID);
VOID  XG_CloseFont(VOID);
SHORT XG_GetSizingFactor(VOID);
SHORT XG_SetFont(char *name, SHORT size);
VOID XG_Sym (struct Box *Box, SHORT Symbol, SHORT x, SHORT y);
VOID XG_BorderSize(struct Box *Box);
SHORT XG_CharSize(unsigned char Letter);
SHORT XG_TextSize(unsigned char *string);
