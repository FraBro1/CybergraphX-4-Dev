
/*
**  This C include file was generated automatically
**  from an assembly include file
**  using I2H written by Henning Friedl.
*/

/* --------------------------------------------------------------------------- */
/*    IFND UTILITY_TAGITEM_I */
/*                 include   utility/tagitem.i */
/*    ENDC */
/* --------------------------------------------------------------------------- */
#define RETINA_LIB_VERSION	(11)	/* the current version of retina.library */
/* --------------------------------------------------------------------------- */
#define SEGMENTSIZE	(64*1024)	/* Retina-Z2 only */
/* --------------------------------------------------------------------------- */
/*  New in v11.7: */
/* --------------------------------------------------------------------------- */
/*  Tags for Retina_AllocBrush(): */

#define BT_SrcScreen	(TAG_USER+401)	/* struct RetinaScreen * */
#define BT_DestScreen	(TAG_USER+402)	/* struct RetinaScreen * */
#define BT_LeftEdge	(TAG_USER+403)	/* Default: 0 */
#define BT_TopEdge	(TAG_USER+404)	/* Default: 0 */
#define BT_Width	(TAG_USER+405)	/* Default: 16 */
#define BT_Height	(TAG_USER+406)	/* Default: 16 */
#define BT_Flags	(TAG_USER+407)	/* see below */

/*  values for BT_Flags */

#define BHB_COPYSOURCE	0
#define BHF_COPYSOURCE	0x00000001	/* copy source brush */
#define BHB_RESTOREBG	1
#define BHF_RESTOREBG	0x00000002	/* restore background */
/* --------------------------------------------------------------------------- */
/*  Tags for Retina_DisplayControl() (v10): */
#define DCT_DPMS	(TAG_USER+300)	/* Display Power Management System */

/*  values for DCT_DPMS: */
#define DPMS_ON	(0)	/* display on, monitor on */
#define DPMS_BLANK	(1)	/* display off, monitor on */
#define DPMS_STANDBY	(2)	/* display off, monitor standby */
#define DPMS_OFF	(3)	/* display off, monitor off */
/* --------------------------------------------------------------------------- */
/*  Tags for Retina_FilterMonitor() (v8): */

#define FMT_Flags	(TAG_USER+200)
#define FMT_MinWidth	(TAG_USER+201)
#define FMT_MaxWidth	(TAG_USER+202)
#define FMT_MinHeight	(TAG_USER+203)
#define FMT_MaxHeight	(TAG_USER+204)
#define FMT_MinHFrequency	(TAG_USER+205)
#define FMT_MaxHFrequency	(TAG_USER+206)
#define FMT_MinVFrequency	(TAG_USER+207)
#define FMT_MaxVFrequency	(TAG_USER+208)

/* Flags for FMT_Flags */
#define FMTB_DEPTH08	1
#define FMTF_DEPTH08	0x00000002
#define FMTB_DEPTH16	2
#define FMTF_DEPTH16	0x00000004
#define FMTB_DEPTH24	3
#define FMTF_DEPTH24	0x00000008
#define FMTB_GROUPS	4
#define FMTF_GROUPS	0x00000010
#define FMTB_NONGROUPS	5
#define FMTF_NONGROUPS	0x00000020
/* --------------------------------------------------------------------------- */
/*  ALL entries in struct RetinaScreenMode are READ ONLY! */
/*  The entries in this structure are only valid after a successful call */
/*  to Retina_Request()! The window dimensions _rsm_Reqxxx are valid even if */
/*  the call failed. */

struct RetinaScreenMode {
	ULONG	rsm_UserData;	/* the value provided by RSM_UserData */

	ULONG	rsm_DisplayID;	/* the selected display id */
	UWORD	rsm_Depth;	/* the selected depth (8, 16 or 24) */
	UWORD	rsm_Width;	/* the selected width */
	UWORD	rsm_Height;	/* the selected height */
	UBYTE	rsm_Virtual;	/* BOOL, set if virtual screen enabled */
	UBYTE	rsm_pad;
	UWORD	rsm_VisibleWidth;	/* the selected visible size, only valid */
	UWORD	rsm_VisibleHeight;	/*   if _rsm_Virtual is true */

	WORD	rsm_ReqLeftEdge;	/* left edge of requesting window */
	WORD	rsm_ReqTopEdge;	/* top edge of requesting window */
	WORD	rsm_ReqWidth;	/* width of requesting window */
	WORD	rsm_ReqHeight;	/* height of requesting window */

};

/*  Do NOT refer to _rsm_PUBSIZE, this structure may grow in the future! */
/* --------------------------------------------------------------------------- */
/*  Tagitems for Retina_AllocRequest() or Retina_Request(): */

#define RSMR_Window	(TAG_USER+100)	/* open on this windows screen */
#define RSMR_PubScreenName	(TAG_USER+101)	/* open on this public screen */
#define RSMR_Screen	(TAG_USER+102)	/* open on this screen */
#define RSMR_UserData	(TAG_USER+103)
#define RSMR_Flags	(TAG_USER+104)	/* see below */
#define RSMR_ReqLeftEdge	(TAG_USER+105)	/* the hor. position to open the requester */
#define RSMR_ReqTopEdge	(TAG_USER+106)	/* the vert. position to open the requester */
#define RSMR_DisplayID	(TAG_USER+107)	/* (v10) the initial display ID */
#define RSMR_Depth	(TAG_USER+108)	/* (v10) the initial depth */
#define RSMR_Width	(TAG_USER+109)	/* (v10) the initial width */
#define RSMR_Height	(TAG_USER+110)	/* (v10) the initial height */
#define RSMR_VisibleWidth	(TAG_USER+111)	/* (v10) the initial visible width */
#define RSMR_VisibleHeight	(TAG_USER+112)	/* (v10) the initial visible height */
/* --------------------------------------------------------------------------- */
#define RSMB_DEPTH08	0
#define RSMF_DEPTH08	0x00000001	/*  enable 8 bit modes */
#define RSMB_DEPTH16	1
#define RSMF_DEPTH16	0x00000002	/*  enable 16 bit modes */
#define RSMB_DEPTH24	2
#define RSMF_DEPTH24	0x00000004	/*  enable 24 bit modes */

#define RSMB_SIZE	3
#define RSMF_SIZE	0x00000008	/*  enable the Width and Height gadget */
#define RSMB_VIRTUAL	4
#define RSMF_VIRTUAL	0x00000010	/*  enable the Virtual gadget */
#define RSMB_VIRTUALON	5
#define RSMF_VIRTUALON	0x00000020	/*  select the Virtual gadget */

#define RSMB_GROUPS	6
#define RSMF_GROUPS	0x00000040	/*  display groups */
#define RSMB_NONGROUPS	7
#define RSMF_NONGROUPS	0x00000080	/*  display single display modes */
/* --------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------- */
/* ---- Alerts: */

/*  an alert number like 35xx01xx identifies an alert caused by retina.library */

#define ALERT_NOHARD	(0x0100)	/* no Retina hardware present */

#define ALERT_NOMEM	(0x0110|AG_NoMemory)	/* Retina does not have any memory at all */
#define ALERT_NOMEMTOFREE	(0x0111)	/* tried to free memory although no memory is in use */
#define ALERT_MEMTOFREENOTFOUND	(0x0112)	/* tried to free memory which has not been allocated */
#define ALERT_NOMEMNODEMEM	(0x0113|AG_NoMemory)	/* no Amiga memory for memory listnode */
#define ALERT_MEMNOTFREED	(0x0114)	/* still some memory in use at library expunge */
#define ALERT_MUNGWALLLOW	(0x0115)	/* internal */
#define ALERT_MUNGWALLHIGH	(0x0116)	/* internal */
#define ALERT_MEMCLK	(0x0117)	/* internal */

#define ALERT_CLOSE_LIB_MONITOR	(0x0121)	/* tried to remove a library internal monitor */
#define ALERT_MONITOR_NOT_CLOSED	(0x0122)	/* LIB_Expunge and not all monitors removed */
#define ALERT_MONITOR_ID_INUSE	(0x0123)	/* internal */
/* --------------------------------------------------------------------------- */
/*  modes for Retina_SetDrMd() */

#define RDM_JAM1	(0)
#define RDM_COMPLEMENT	(2)
#define RDMB_PENCLR	4
#define RDMF_PENCLR	0x00000010	/*  (v11) PRIVATE */
#define RDMB_PENSET	5
#define RDMF_PENSET	0x00000020	/*  (v11) PRIVATE */
#define RDMB_PENINVERT	6
#define RDMF_PENINVERT	0x00000040	/*  (v11) PRIVATE */
/* --------------------------------------------------------------------------- */
/*  modes for Retina_WriteBitMap() */

#define WBMMODE_STD	(0)	/* normal 2 to 256 color images and EHB to 8 bit */
#define WBMMODE_HAM	(1)	/* 4096 color HAM to 16 or 24 bit (16 Bit: (v4)) */
#define WBMMODE_HAM8	(2)	/* 256k color AA HAM to 24 bit */

#define WBMMODEB_EXTENDED	16
#define WBMMODEF_EXTENDED	0x00010000	/* see retina.doc/Retina_WriteBitMap() (v3) */
/* --------------------------------------------------------------------------- */
/*  modes for Retina_WriteRect(), ... */

#define RECTMODE_RGB	(0)	/* one byte each for red, green, blue */
#define RECTMODE_RGBA	(1)	/* one byte each for red, green, blue and alpha */
#define RECTMODE_BGR	(2)	/* same as RGB, but reverse pixel order */
#define RECTMODE_GB_R	(3)	/* PRIVATE, do not use */
#define RECTMODE_256	(4)	/* one byte for each pixel */
#define RECTMODE_GB_R0	(5)	/* PRIVATE, do not use */
#define RECTMODE_RAW	(6)	/* raw data, either 8, 16 or 24 bit (v4) */
#define RECTMODE_RGBA_0	(7)	/* PRIVATE, do not use (v5) */
#define RECTMODE_CLUT	(8)	/* Palette: RGBA --- PRIVATE, do not use (v6) */

/*  you may OR the following flags to Mode when calling Retina_WriteRect(), ... */

#define RECTMODEB_BYTEMEMWIDTH	16
#define RECTMODEF_BYTEMEMWIDTH	0x00010000	/* if set, MemWidth counts in bytes */
/*   instead of pixels (v7) */
/* --------------------------------------------------------------------------- */
/*  Flags for Retina_CopyRect(): */

#define CRB_SRCDBUF	0
#define CRF_SRCDBUF	0x00000001	/* use the double buffer bitmap as source */
#define CRB_DESTDBUF	1
#define CRF_DESTDBUF	0x00000002	/* use the double buffer bitmap as destination */
#define CRB_XOR	2
#define CRF_XOR	0x00000004	/* XOR Source with Destination (v11) PRIVATE */
#define CRB_NOCLIP	3
#define CRF_NOCLIP	0x00000008	/* avoid clipping (v11) PRIVATE */
/* --------------------------------------------------------------------------- */
/*  Tagitems for Retina_SetSprite() or Retina_SpriteFunction(): */

#define RSP_On	(TAG_USER+1)
#define RSP_Color0	(TAG_USER+2)
#define RSP_Color1	(TAG_USER+3)
#define RSP_ZoomX	(TAG_USER+4)
#define RSP_ZoomY	(TAG_USER+5)	/* ignored by Retina_SpriteFunction() */
/* --------------------------------------------------------------------------- */
#define MONID_LIB	(0x00000000)	/*  IDs used by retina.library. Do NOT use! */
#define MONID_WBEMU	(0x10000000)	/*  IDs used by RetinaEmu. Do NOT use! */
#define MONID_CUSTOM	(0x20000000)	/*  Software which adds custom monitor */
/*  definitions must set the highest */
/*  nibble of the monitor ID to 2! */
#define MONID_AUTO	(0x70000000)	/*  Auto-IDs created by Retina_AddMonitor(). */
/*  Do NOT use! */
#define MONID_ADDAUTO	(0xffffffff)	/*  Used by Retina_AddMonitor() to create */
/*  auto IDs. */

/*  flag definitions for _nrm_Flags and _rm_Flags: */

#define MDB_DBL	0
#define MDF_DBL	0x00000001	/* 1 = Double-Scan */
#define MDB_LACE	1
#define MDF_LACE	0x00000002	/* 1 = Interlace */
#define MDB_CLKDIV2	2
#define MDF_CLKDIV2	0x00000004	/* 1 = use 1/2 pixel clock */
#define MDB_PLANAR	3
#define MDF_PLANAR	0x00000008	/* 1 = screen has 16 colors on 4 planes          (v2) */
#define MDB_HIDE	4
#define MDF_HIDE	0x00000010	/* 1 = entry should not appear in public lists   (v2) */
#define MDB_GROUP	5
#define MDF_GROUP	0x00000020	/* 1 = this definition is a group-header         (v2) */
#define MDB_POSHSYNC	6
#define MDF_POSHSYNC	0x00000040	/* 1 = HSync wird positiv                        (v4) */
#define MDB_POSVSYNC	7
#define MDF_POSVSYNC	0x00000080	/* 1 = VSync wird positiv                        (v4) */

#define ATNT_MODE_8	(0)	/*  8 bit with color lookup */
#define ATNT_MODE_15	(1)	/* 15 bit true color (not supported) */
#define ATNT_MODE_24	(2)	/* 24 bit true color */
#define ATNT_MODE_16	(3)	/* 16 bit true color */

struct NewRetinaMonitor {
	LONG	nrm_MonitorID;	/* a unique ID to describe this monitor */
/*  0 is NOT a valid ID */
/* -1 forces Retina_Addmonitor() to */
/*    generate a unique ID */
	LONG	nrm_PixelClock;	/* the desired pixelclock, must be one */
/* out of: */
/* 25175000,28322000,36000000,44900000 */
/* 50000000,56644000,63000000,65000000 */
/* 72000000,75000000,80000000,90000000 */
	BYTE	nrm_AtntMode;	/* see definitions above */
	BYTE	nrm_Flags;	/* see definitions above */

 /* LABEL _nrm_NewGroupIDs */	/* PRIVATE (v5) */

	WORD	nrm_StdWidth;	/* standard width of the monitor */
	WORD	nrm_StdHeight;	/* standard height of the monitor */
	WORD	nrm_MaxWidth;	/* maximum (overscen) width of the monitor */
	WORD	nrm_MaxHeight;	/* maximum (overscen) height of the monitor */

 /* LABEL _nrm_GroupIDs */	/* a ULONG *, pointing to a NULL terminated */
/*   array of monitor IDs. (v2) */
	WORD	nrm_HBStart;
	WORD	nrm_HSStart;
	WORD	nrm_HSEnd;
	WORD	nrm_HBEnd;
	WORD	nrm_HTotal;

	WORD	nrm_VBStart;
	WORD	nrm_VSStart;
	WORD	nrm_VSEnd;
	WORD	nrm_VBEnd;
	WORD	nrm_VTotal;

};

/*  Do NOT refer to _nrm_SIZEOF, this structure may grow in the future! */
/* --------------------------------------------------------------------------- */
struct RetinaMonitor {
	struct	Node _rm_Node;	/* LN_NAME points to a human readable string */
/* which identifies the monitor definition. */
	LONG	rm_MonitorID;

	LONG	rm_HFrequency;
	LONG	rm_VFrequency;

	WORD	rm_BytesPerPixel;

	WORD	rm_StdWidth;
	WORD	rm_StdHeight;
	WORD	rm_MaxWidth;
	WORD	rm_MaxHeight;

	BYTE	rm_PrivateFlags;	/* PRIVATE, do not use! */
	BYTE	rm_Reserved1;	/* PRIVATE, do not use! */
	BYTE	rm_AtntMode;	/* a copy of _nrm_AtntMode */
	BYTE	rm_Flags;	/* a copy of _nrm_Flags */

	APTR	rm_GroupIDs;	/* points to a null terminated array of IDs */
/* if this is a group, otherwise NULL (v5) */
	LONG	rm_PixelClock;	/* (v5) */

};
/* --------------------------------------------------------------------------- */
/* definitions for _dsi_Flags */
#define DSIB_USEWIDTH	0
#define DSIF_USEWIDTH	0x00000001	/* If set, use the value in _dsi_Width. If not set, */
/*   pass the original value to Retina_OpenScreen(). */
#define DSIB_USEHEIGHT	1
#define DSIF_USEHEIGHT	0x00000002	/* If set, use the value in _dsi_Height. If not set, */
/*   pass the original value to Retina_OpenScreen(). */

struct DefaultScreenInfo {
	LONG	dsi_ID;	/* the current ScreenMode-ID */
	WORD	dsi_Width;	/* the current width */
/* (only valid if DSIB_USEWIDTH is set) */
	WORD	dsi_Height;	/* the current height */
/* (only valid if DSIB_USEHEIGHT is set) */
	BYTE	dsi_Flags;	/* flag definitions see above */
	UBYTE	_dsi_Reserved[7];	/* reserved, do not use */
};
/* --------------------------------------------------------------------------- */
/*  special values vor Retina_OpenScreen() Width and Height */

#define RSCR_MINWIDTH	(13)	/* minimum width of a Retina screen */
#define RSCR_MINHEIGHT	(1)	/* minimum height of a Retina screen */

#define RSCR_STDWIDTH	(-1)	/* open the screen at its standard width */
#define RSCR_STDHEIGHT	(-1)	/* open the screen at its standard height */
#define RSCR_MAXWIDTH	(-2)	/* open the screen at its maximum width */
#define RSCR_MAXHEIGHT	(-2)	/* open the screen at its maximum height */

/*  TagItems for Retina_OpenScreen(): */

#define RSA_BitMap	(TAG_USER+1)	/* use this BitMap */
#define RSA_DBufBitMap	(TAG_USER+2)	/* not supported yet */
#define RSA_ErrorCode	(TAG_USER+3)	/* return an error code */
#define RSA_Name	(TAG_USER+4)	/* the screens name (v3) */
#define RSA_Palette	(TAG_USER+5)	/* the screens initial palette (v3) */
#define RSA_PaletteRange	(TAG_USER+6)	/* to select less then 256 colors (v3) */
/* high word of ti_Data: First */
/* low word of ti_Data: Count */
/* see: retina.doc/Retina_LoadPalette() */
#define RSA_FreeID	(TAG_USER+7)	/* optional ID for 'free screens' (v4) */
#define RSA_TopEdge	(TAG_USER+8)	/* initial top edge of the screen (v5) */
#define RSA_Parent	(TAG_USER+9)	/* parent screen used by the panel screen (v5) */
#define RSA_Depth	(TAG_USER+10)	/* PRIVATE, do not use! */
#define RSA_LeftEdge	(TAG_USER+11)	/* initial left edge of the screen (v7) */
#define RSA_VisibleWidth	(TAG_USER+12)	/* visible width of the screen (v7) */
#define RSA_VisibleHeight	(TAG_USER+13)	/* visible height of the screen (v7) */

/*  values for RSA_ErrorCode: */

#define ROSERR_NONE	(0)	/*  everything ok if Retina_OpenScreen() returned NON-NULL */
/*    or unknown error if it returned NULL */
#define ROSERR_ID	(1)	/*  unknown monitor-ID */
#define ROSERR_DEFAULTID	(2)	/*  unknown default monitor-ID */
#define ROSERR_TOOBIG	(3)	/*  screen size is too big */
#define ROSERR_NOMEM	(4)	/*  couldn't get normal memory */
#define ROSERR_NORETINAMEM	(5)	/*  couldn't get Retina graphics memory */
#define ROSERR_TOOSMALL	(6)	/*  screen size is too small (< 13*1) (v5) */
#define ROSERR_CLOCK	(7)	/*  pixelclock is too high (v5) */

/*  Flags for _rs_Flags: */

#define RSFB_DONTCLEARONOPEN	0
#define RSFF_DONTCLEARONOPEN	0x00000001	/* don't clear the screens memory when opened */
#define RSFB_DONTCLEARONCLOSE	1
#define RSFF_DONTCLEARONCLOSE	0x00000002	/* don't turn off display after closing screen */
#define RSFB_DOUBLEBUFFER	2
#define RSFF_DOUBLEBUFFER	0x00000004	/* prepare screen for double buffering */
#define RSFB_CUSTOMBITMAP	3
#define RSFF_CUSTOMBITMAP	0x00000008	/* PRIVATE, do not use */
#define RSFB_CUSTOMDBUFBITMAP	4
#define RSFF_CUSTOMDBUFBITMAP	0x00000010	/* PRIVATE, do not use */
#define RSFB_AUTOADJUST	5
#define RSFF_AUTOADJUST	0x00000020	/* adjust the screens size if too big, ... */
#define RSFB_OPENBEHIND	6
#define RSFF_OPENBEHIND	0x00000040	/* open the screen behind all others */
#define RSFB_DBUFPALETTE	7
#define RSFF_DBUFPALETTE	0x00000080	/* use special palette handling (v3) */
#define RSFB_OVERSIZED	8
#define RSFF_OVERSIZED	0x00000100	/* PRIVATE, do not use (v3) */
#define RSFB_SYSTEM	9
#define RSFF_SYSTEM	0x00000200	/* PRIVATE, do not use (v3) */
#define RSFB_DONTCLOSEONCLOSE	10
#define RSFF_DONTCLOSEONCLOSE	0x00000400	/* don't close the screen on Retina_CloseScreen() (v4) */
#define RSFB_FREE	11
#define RSFF_FREE	0x00000800	/* PRIVATE, do not use (v4) */
#define RSFB_ZEROBITMAP	12
#define RSFF_ZEROBITMAP	0x00001000	/* PRIVATE, do not use (v5) */
#define RSFB_PANEL	13
#define RSFF_PANEL	0x00002000	/* screen is a panel screen (v5) */
#define RSFB_LOCKPANEL	14
#define RSFF_LOCKPANEL	0x00004000	/* panel screen is locked to its parent (v5) */
#define RSFB_FAKE	15
#define RSFF_FAKE	0x00008000	/* this is a fake screen allocated */
/*   by Retina_AllocScreen() (v6) */
#define RSFB_HCENTER	16
#define RSFF_HCENTER	0x00010000	/* center the screen horizontally (v7) */
#define RSFB_VCENTER	17
#define RSFF_VCENTER	0x00020000	/* center the screen vertically (v7) */
#define RSFB_DEEP	18
#define RSFF_DEEP	0x00040000	/* PRIVATE, do not use (v7) */

struct RetinaScreen {
	struct	Node Node;	/* ListNode for internal use only */
/* LN_NAME MAY point to the screens name */
/*   it also may be NULL! (v3) */
	WORD	rs_Width;	/* number of pixels in one horizontal line */
	WORD	rs_Height;	/* number of lines */
/* --- */
	APTR	rs_BitMap;	/* the screen's address in Retina memory */
	APTR	rs_DBufBitMap;	/* used for double buffered screens */

	LONG	rs_Flags;	/* flags see above */

 /* LABEL _rs_APen */
	BYTE	rs_APen_reserved;	/* reserved */
	BYTE	rs_APenR;	/* the red component (true color only) */
	BYTE	rs_APenG;	/* the green component (true color only) */
 /* LABEL _rs_APen256 */	/* the number of the palette register */
/*   on a 256 color screen */
	BYTE	rs_APenB;	/*   or the blue component (true color) */
/* --- */
	BYTE	rs_DrawMode;	/* RDM_JAM1, ... */
	BYTE	rs_Reserved;	/* (v3) */
/* --- */
	WORD	rs_BytesPerPixel;	/* number of bytes per pixel (1..3) */
	WORD	rs_Modulo;	/* number of bytes in one line */
	APTR	rs_Monitor;	/* struct RetinaMonitor * */

	APTR	rs_Palette;	/* points to an array of 256 RGB values */
/*   read only! */

	WORD	rs_BitsPerPixel;	/* number of bits per pixel (4,8,16,24) (v3) */

	WORD	rs_LeftEdge;	/* the position of centered */
	WORD	rs_TopEdge;	/*   or oversized screens (v7) */

	WORD	rs_VisibleWidth;	/* the visible size */
	WORD	rs_VisibleHeight;	/*   of oversized screens (v7) */

	APTR	rs_BitMapA;	/* PRIVATE, do not use (v11) */
	APTR	rs_DBufBitMapA;	/* PRIVATE, do not use (v11) */

	ULONG	rs_UserData;	/* for your use - _rs_UserData is */
/*   set to 0 by Retina_OpenScreen() */
/*   or Retina_AllocScreen(). (v11) */

};
/* --------------------------------------------------------------------------- */
/*  These are the currently available Mode-ID's you need for Retina_OpenScreen() */

#define MID_DEFAULT_08	(-1)	/* ask for the default 8 bit screen */
#define MID_DEFAULT_16	(-2)	/* ask for the default 16 bit screen */
#define MID_DEFAULT_24	(-3)	/* ask for the default 24 bit screen */

#define MID_UNKNOWN	(0)
/* --------------------------------------------------------------------------- */
/*  the public-part of retina.library */

struct RetinaBase {
	struct	Library Library;
	APTR	rb_FirstScreen;	/* A pointer to the first screen. This */
/* is the visible screen. If no screen */
/* is open, _rb_FirstScreen is NULL. */
	ULONG	rb_FrameCount;	/* This is incremented each Retina */
/* vertical blank interrupt. You may use */
/* it to check wether your animation got */
/* every frame, but NEVER use it at a */
/* clock! */
	APTR	rb_HardInfo;	/* NULL, if there is no Retina hardware */
/* Non-NULL if there is a hardware. (v2) */
/* Starting with (v5) _rb_HardInfo is a  */
/* pointer to struct HardInfo (PRIVATE)  */
/* or NULL. */
/* Do NOT use _rb_HardInfo as a pointer */
/* in earlier versions! */
	APTR	rb_BoardAddress;	/* the address of the Retina board (v3) */
	APTR	rb_SegmentAddress;	/* the address of the Retina memory */
/* window (v3) */
	UBYTE	_rb_private1[12];	/* PRIVATE, do not use */

	struct	List _rb_ScreenList;	/* all open Retina screens (v3) */

	APTR	rb_SpriteMem;	/* PRIVATE, do not use */
	struct	List _rb_MemList;	/* PRIVATE, do not use */

};

/*  subsequent fields in the library base are RETINA PRIVATE */
/* --------------------------------------------------------------------------- */
struct MemNode {
	struct	MinNode MinNode;	/* PRIVATE, do not use */
	APTR	mn_Memory;	/* PRIVATE, do not use */
	LONG	mn_Size;	/* PRIVATE, do not use */
};
/* --------------------------------------------------------------------------- */
/*  flags for _hi_Flags: */
#define HIB_FREEPIXCLK	0
#define HIF_FREEPIXCLK	0x00000001	/* PRIVATE, do not use */
#define HIB_DPMS	1
#define HIF_DPMS	0x00000002	/* PRIVATE, do not use */
#define HIB_LINEAR	2
#define HIF_LINEAR	0x00000004	/* PRIVATE, do not use */

/*  values for _hi_Board: */
#define HIB_RETINA	(0)	/* PRIVATE, do not use */
#define HIB_RETINABLTZ3	(1)	/* PRIVATE, do not use */

struct HardInfo {	/* PRIVATE, do not use */
	ULONG	hi_Flags;	/* PRIVATE, do not use */
	ULONG	hi_MaxPixClk;	/* PRIVATE, do not use */
	UBYTE	hi_Board;	/* PRIVATE, do not use */
	UBYTE	_hi_Reserved[3];	/* PRIVATE, do not use */
};
/* --------------------------------------------------------------------------- */
/*  You may use this macro to call the functions in retina.library. To avoid */
/*  conflicts with existing functions, they have a slightly unusual name. */

/* --------------------------------------------------------------------------- */
