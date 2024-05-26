
/*
**  This C include file was generated automatically
**  from an assembly include file
**  using I2H written by Henning Friedl.
*/

/* --------------------------------------------------------------------------- */
/*  (v2) - this feature has been added in version 2 of multipic.library */
/*  (v3) - this feature has been added in version 3 of multipic.library */
/* --------------------------------------------------------------------------- */
#define MULTIPIC_LIB_VERSION	(3)	/* the current version of multipic.library */
/* --------------------------------------------------------------------------- */
#define MP_TagBase	(TAG_USER+0x800)	/* Begin counting tags */
/* --------------------------------------------------------------------------- */
/* flags for MP_Open(): */

#define MPMODE_LOAD	(0)
#define MPMODE_SAVE	(1)
/* --------------------------------------------------------------------------- */
/*  values for _pi_Type: */
#define PFT_UNKNOWN	(0)	/* UNKNOWN image type, unable to load */
#define PFT_ILBM	(1)	/* IFF ILBM */
#define PFT_DEEP	(2)	/* IFF DEEP */
#define PFT_BMP	(3)	/* BMP */
#define PFT_RGB8	(4)	/* IFF RGB8 */
#define PFT_RGBN	(5)	/* IFF RGBN */
#define PFT_YUVN	(6)	/* IFF YUVN */
#define PFT_VLAB	(7)	/* IFF VLAB, private VLab format */
#define PFT_PGM	(8)	/* PGM */
#define PFT_PPM	(9)	/* PPM */
#define PFT_QRT	(10)	/* QRT */
#define PFT_SUNRASTER	(11)	/* SUNRASTER */
#define PFT_XIPAINT	(12)	/* XIPAINT, private VDPaint format */
#define PFT_DATATYPE	(13)	/* see _pi_TypeName for current ascii identifier (v2) */
#define PFT_PBM	(14)	/* IFF PBM (v2) */
#define PFT_JPEG	(15)	/* JPEG (v2) */

/* values for _pi_Flags: */
#define PIB_IFF	0
#define PIF_IFF	0x00000001	/* image is stored in an IFF file format */
#define PIB_PALETTE	1
#define PIF_PALETTE	0x00000002	/* if set, a palette had been stored together */
/*   with the image */
#define PIB_TRUECOLOR	2
#define PIF_TRUECOLOR	0x00000004	/* set if the image is true color */
/* if at least one of PIF_PALETTE or */
/*   PIF_TRUECOLOR is set, the image seems */
/*   to be a color image. */

struct PicInfo {
	LONG	pi_Flags;	/* see definitions above */
	APTR	pi_FileName;	/* name of the file (provided to Open() ) */
	APTR	pi_TypeName;	/* file type, ascii identifier */
	WORD	pi_Type;	/* see PFT_xxx above */
/* if this is PFT_UNKNOWN, multipic.library */
/*   can not load the image (if it is one) */
	WORD	pi_Width;	/* The width of the image. */
	WORD	pi_Height;	/* The height of the image. */
	WORD	pi_PageWidth;	/* The page width of the image. */
	WORD	pi_PageHeight;	/* The page height of the image. */

	BYTE	pi_AspectX;	/* The X aspect of the image (0, if unknown) */
	BYTE	pi_AspectY;	/* The Y aspect of the image (0, if unknown) */

	UWORD	pi_CMapEntries;	/* number of color values */
/* 2..256 with 8 bit images */
/* 0 or 256 with 24 bit true color images */
	UWORD	pi_CMapSize_RGB;	/* memory size of the PT_RGB palette */
	UWORD	pi_CMapSize_RRGGBB;	/* memory size of the PT_RRGGBB palette */
	UWORD	pi_CMapSize_RGB4;	/* memory size of the PT_RGB4 palette */
	UWORD	pi_CMapSize_RGB32;	/* memory size of the PT_RGB32 palette */
	UWORD	pi_CMapSize_Res1;	/* currently always 0 */
	UWORD	pi_CMapSize_Res2;	/* currently always 0 */

	UBYTE	pi_Depth;	/* 1..8,12,24,32 */
	UBYTE	pi_BytesPerPixel;	/* 1, 3, 4 */

	UBYTE	pi_RedBits;	/* 1..8  \ */
	UBYTE	pi_GreenBits;	/* 1..8   \ for TRUE-COLOR only */
	UBYTE	pi_BlueBits;	/* 1..8   /   otherwise 0 */
	UBYTE	pi_AlphaBits;	/* 1..8  / */
	UBYTE	pi_Reserved1Bits;	/* currently always 0 */
	UBYTE	pi_Reserved2Bits;	/* currently always 0 */

	UWORD	pi_LeftEdge;	/* (v3) */
	UWORD	pi_TopEdge;	/* (v3) */

};
/* --------------------------------------------------------------------------- */
struct PicHandle {
	ULONG	ph_Private;
};
/* --------------------------------------------------------------------------- */
/*  values for MP_ReadPalette: */

#define PT_RGB	(0)
#define PT_RRGGBB	(1)	/* not yet */
#define PT_RGB4	(2)	/* not yet */
#define PT_RGB32	(3)	/* not yet */
/* --------------------------------------------------------------------------- */
/*  Flags for BAT_Flags: */
#define BAB_MERGEPALETTE	0
#define BAF_MERGEPALETTE	0x00000001	/* recalculate 24 bit true color data */
/*   if a palette is present */
/*   currently not yet implemented */

/* --------------------------------------------------------------------------- */
/* Tags for MP_SetBufferAttrs(): */

#define BAT_Inc	(MP_TagBase+1)
#define BAT_RedInc	(MP_TagBase+2)
#define BAT_GreenInc	(MP_TagBase+3)
#define BAT_BlueInc	(MP_TagBase+4)
#define BAT_AlphaInc	(MP_TagBase+5)
#define BAT_DefaultAlphaValue	(MP_TagBase+6)
#define BAT_Flags	(MP_TagBase+7)
#define BAT_LeftEdge	(MP_TagBase+8)
#define BAT_TopEdge	(MP_TagBase+9)
#define BAT_Width	(MP_TagBase+10)
#define BAT_Mod	(MP_TagBase+11)
#define BAT_RedMod	(MP_TagBase+12)
#define BAT_GreenMod	(MP_TagBase+13)
#define BAT_BlueMod	(MP_TagBase+14)
#define BAT_AlphaMod	(MP_TagBase+15)
#define BAT_MergePalette	(MP_TagBase+16)	/* (default: off) (v3) */

/*  values for BAT_MergePalette: */
#define MERGEPALETTEB_CMAP	0
#define MERGEPALETTEF_CMAP	0x00000001	/* merge image data with CMAP */
#define MERGEPALETTEB_CLUT	1
#define MERGEPALETTEF_CLUT	0x00000002	/* merge image data with CLUT */

#define MERGEOFF	(0)	/* OBSOLETE, do not use */
#define MERGECMAP	(MERGEPALETTEF_CMAP)	/* OBSOLETE, do not use */
#define MERGECLUT	(MERGEPALETTEF_CLUT)	/* OBSOLETE, do not use */
/* --------------------------------------------------------------------------- */
/*  Tags for MP_SetImageAttrs(): */

#define IAT_Type	(MP_TagBase+51)	/* the desired file format (see PFT_xxx above) */
/*   (overrides IAT_Key) */
#define IAT_Width	(MP_TagBase+52)
#define IAT_Height	(MP_TagBase+53)
#define IAT_PageWidth	(MP_TagBase+54)	/* Default: IAT_Width */
#define IAT_PageHeight	(MP_TagBase+55)	/* Default: IAT_Height */
#define IAT_Depth	(MP_TagBase+56)

#define IAT_Crunch	(MP_TagBase+57)	/* BOOL: crunch mode, Default: TRUE (v2) */
#define IAT_Key	(MP_TagBase+58)	/* pointer to ascii key of the desired */
/*   file format (v2) */
#define IAT_AspectX	(MP_TagBase+59)	/* Default: 1 (v2) */
#define IAT_AspectY	(MP_TagBase+60)	/* Default: 1 (v2) */

#define IAT_LeftEdge	(MP_TagBase+61)	/* Default: 0 (v2) */
#define IAT_TopEdge	(MP_TagBase+62)	/* Default: 0 (v2) */

#define IAT_RedBits	(MP_TagBase+63)	/* Default: 8 (v2) */
#define IAT_GreenBits	(MP_TagBase+64)	/* Default: 8 (v2) */
#define IAT_BlueBits	(MP_TagBase+65)	/* Default: 8 (v2) */
#define IAT_AlphaBits	(MP_TagBase+66)	/* Default: 8 (v2) */

/* --------------------------------------------------------------------------- */
/* values for _fi_Flags: */

#define FIB_LOAD	0
#define FIF_LOAD	0x00000001	/* can load images (crunched, uncrunched or both) */
#define FIB_LOADCRUNCHED	1
#define FIF_LOADCRUNCHED	0x00000002	/* can load crunched images */
#define FIB_LOADUNCRUNCHED	2
#define FIF_LOADUNCRUNCHED	0x00000004	/* can load uncrunched images */
#define FIB_SAVE	3
#define FIF_SAVE	0x00000008	/* can save images (crunched, uncrunched or both) */
#define FIB_SAVECRUNCHED	4
#define FIF_SAVECRUNCHED	0x00000010	/* can save crunched images */
#define FIB_SAVEUNCRUNCHED	5
#define FIF_SAVEUNCRUNCHED	0x00000020	/* can save uncrunched images */
#define FIB_LOSSY	6
#define FIF_LOSSY	0x00000040	/* crunched images loose information (JPEG, ...) */
#define FIB_TRUECOLOR	7
#define FIF_TRUECOLOR	0x00000080	/* can load/save true color images */
#define FIB_ALPHA	8
#define FIF_ALPHA	0x00000100	/* supports alpha channel */
#define FIB_PROPERTY	9
#define FIF_PROPERTY	0x00000200	/* supports property chunks (v3) */

struct FormatInfo {
	struct	Node Node;
/* LN_NAME points to 'ILBM', 'BMP', ... */
	BYTE	fi_Type;	/* see: PFT_xxx */
	BYTE	fi_CrunchModes;	/* (v3) */
	ULONG	fi_Flags;	/* see definitions above */
	APTR	fi_TypeName;	/* ascii string (v3) */

};
/* --------------------------------------------------------------------------- */
/*  possible error codes: */

#define EC_OK	(0)
#define EC_INTERNAL	(1)
#define EC_DOS	(2)

#define EC_OUT_OF_MEMORY	(3)
#define EC_COULD_NOT_GET_INFORMATION	(4)
#define EC_NO_IFFPARSE_LIB	(5)
#define EC_END_OF_FILE	(6)
#define EC_PLANENUMBER_NOT_SUPPORTED	(7)
#define EC_COMPRESSION_NOT_SUPPORTED	(8)
#define EC_COMPRESSIONMODE_NOT_SUPPORTED	(9)
#define EC_COMPRESSIONMODE_UNKNOWN	(10)
#define EC_ALLOCIFF_FAILED	(11)
#define EC_IFFERR_EOC	(12)
#define EC_IFFERR_NO_SCOPE	(13)
#define EC_IFFERR_READ	(14)
#define EC_IFFERR_WRITE	(15)
#define EC_IFFERR_SEEK	(16)
#define EC_BAD_DATA	(17)
#define EC_IFFERR_SYNTAX	(18)
#define EC_IFFERR_NOT_IFF	(19)
#define EC_IFFERR_NO_HOOK	(20)
#define EC_SAVE_NOT_SUPPORTED	(21)
#define EC_DATATYPE_FUNCTION_FAILED	(22)
#define EC_NO_VLAB_LIB	(23)
#define EC_BITMAPHEADER_NOT_FOUND	(24)
#define EC_UNKNOWN_FILETYPE	(25)
#define EC_BAD_PALETTE	(26)
#define EC_BAD_IMAGEATTRS	(27)
/* --------------------------------------------------------------------------- */
/*  You may use this macro to call the functions in multipic.library. To avoid */
/*  conflicts with existing functions, they have a slightly unusual name (MP_xxx). */
/* --------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------- */
