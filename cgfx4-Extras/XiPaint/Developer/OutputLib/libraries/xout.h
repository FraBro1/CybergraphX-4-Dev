/* Last Change: Mon Jan 16 11:51:58 1995 */
/* Und noch die Übergaberoutinen
*/

struct xo_init{
	LONG kennung;			/* Nummer der Library */
	char lib_name[50];		/* Library-Name, Name der Unterstützten Lib */
	char hw_name[50];		/* Retina, Picasso etc. */
	char version[50];		/* Versionsstring im Klartext */
	LONG prozessor;			/* 68000, 020, 030, 040, 881 etc. */
	LONG res_request;		/* Ist ein Resolution-Requester in der ext. Lib? */

	SHORT width, height, depth;	/* Größe u. Tiefe des Displays */
	LONG display_id;		/* ID des Displays */

	ULONG max_undo_mem;		/* Größe des max. Undo-Speichers */
	SHORT dummy;			/* temp. Variable zum Benutzen in XPrefs u. a. */
	LONG oscanmode;			/* Overscan-Modus für AA-Version */
	SHORT autoscroll;		/* AutoScroll-Feature von AA */
	SHORT versionnumber;		/* Mit der 1.sten Release ist das > 1 */
	SHORT mousemax;			/* Anzahl der Maustreiber in Library, >1 wenn Tabletts */
	SHORT mouseact;			/* aktuell gewählter Maustreiber, 0 wenn Amiga */
	char mode_name[256];		/* ev. Display-Mode-Name (EGS) */
	UBYTE reserved[512];		/* für spätere Erweiterungen */
};


/****  Defines für Tag von XO_SetReg()-Funktion   ****/

#define  XO_SR_GETWINLIB_STD      1	/* Name der Standard-Window-Library */
#define  XO_SR_GETGFXLIB          2	/* Name der passenden XiGfx-Library */
#define  XO_GET_ERROR            99
#define  XO_SET_CLUT             98	/* Wegen Hintergrundfarbe */
#define  XO_SR_GETWINLIBS       100	/* + Nummer des Treibers (0 = Amiga) -> Name der akt. Window-Library */
#define  XO_SR_GETWINLIBS2      101	/* + Nummer des Treibers (0 = Amiga) -> Name der akt. Window-Library */
#define  XO_SR_GETMOUSENAMES    200	/* + Nummer des Treibers (0 = Amiga) -> Name des Maustreibers */
#define  XO_SR_GETMOUSENAMES2   201	/* + Nummer des Treibers (0 = Amiga) -> Name des Maustreibers */

#define  XO_SR_GETCOLTRANS_FW     3	/* (nur AA) Zeiger auf AA-Paletten-Übersetzungstabelle */
#define  XO_SR_GETCOLTRANS_BK     4	/* (nur AA) Zeiger auf umgekehrte AA-Paletten-Übersetzungstabelle */

#define  XO_SR_SETSCRFONTNAME     5	/* (nur AA) Setzen des Font-Namens für Screen */
#define  XO_SR_SETSCRFONTSIZE     6	/* (nur AA) Setzen der Font-Größe für Screen */


/****  Namen der Grafikkarten im Kennungs-Feld der xo_init-Struktur  ****/

#define HW_RETINA24    1	/* R xigfx_u.library, xiwin_u.library */
#define HW_RETINAEMU   2 	/* W xigfx_w.library, xiwin_w.library */
#define HW_PICASSO24   3	/* P xigfx_u.library, xiwin_u.library */
#define HW_GRAFFITY    4	/* G xigfx_u.library, xiwin_u.library */
#define HW_EGSPICASSO  5	/* ? xigfx_u.library, xiwin_u.library */
#define HW_VD2001      6	/* V xigfx_u.library, xiwin_u.library */
#define HW_PICASSO16   7	/* Q xigfx_u.library, xiwin_u.library */
#define HW_FRAMEMASTER 9	/* F xigfx_u.library, xiwin_u.library */
#define HW_HAM8       10	/* A xigfx_a.library, xiwin_a.library */
#define HW_HAM6       11	/* H xigfx_h.library, xiwin_a.library */
#define HW_AA256      12 	/* D xigfx_d.library, xiwin_a.library */
#define HW_EGS        13	/* E xigfx_u.library, xiwin_u.library */
#define HW_OMNIBUS    14	/* O xigfx_u.library, xiwin_u.library */
#define HW_RETINA8    15	/* B xigfx_u.library, xiwin_u.library */
#define HW_PICASSO8   16	/* ? xigfx_u.library, xiwin_u.library */
#define HW_CYBER24    17	/* Y xigfx_u.library, xiwin_u.library */
#define HW_MERLIN24   18	/* M xigfx_u.library, xiwin_u.library */
#define HW_CYBERWB    19	/* C xigfx_a.library, xiwin_a.library */

/* 3 für 3.1 und höher, -69 für Demoversion */
/* 4 für enhanced Errorhandling und Farbeinstellung */
#define VERSIONNUMBER 4
