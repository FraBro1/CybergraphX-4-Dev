/* Last Change: Herbert / 14. August 1995  10:07:17 */

/****  Header für XiWindows-Library  ****/

struct WinSet {
        short SystemFontHeight;                 /* Größe des System-Fonts */
        SHORT TicksPerSec;                      /* 0 = keine Ticks, sonst Anzahl Ticks / Sekunde */
        SHORT MouseKeep;                        /* jedes x-te Mouse-Move behalten */
        BOOL NoActKlick;                        /* wenn = TRUE wird der Aktiv-Klick einer Paint-Box geschluckt */
        BOOL ClickToFront;                      /* wenn = TRUE, kommt Fenster mit Doppelklick nach vorne */
        LONG opaque_pixel;                      /* Grenzwert, ab dem Brush nur mehr als Rubber */
        WORD pox,poy;                           /* Pointer-Offset in x/y */

        struct MsgPort *RexxPort;               /* Exec-Struktur von Rexx-Port */
        struct MsgPort *UserPort;               /* Port für Messages von außen und von Prozessen */
        struct brush *brush;                    /* Brush zum Zeichnen, Font oder ausgeschnitten */
        ULONG *Clut24;                          /* Palette für 24-Bit */

        struct XoutBase *XoutBase;              /* unsere Out-Library */
        struct XigfxBase *XigfxBase;            /* unsere Grafik-Routinen */

        char *SystemFontName;                   /* Name des System-Fonts */
     };


/****  Strukturen für Events  ****/

struct Event {
        struct Message ExecMess;        /* Exec-Message-Struktur */
        UWORD Event;                    /* Art des eingetretenen Events */
        UWORD KeyCode;                  /* ASCII-Code bei EV_VANILLA od. RAWKEY / Area-Nummer von Drop-Area */
        UWORD Qualifier;                /* Qualifier-Taste bei EV_RAWKEY od. bei Maus-Buttons */
        SHORT EvX, EvY;                 /* Koordinaten eines Events (Mausklick...) */
        struct Box *EvBox;              /* Zeiger auf Box, in der Event war (bei manchen Events) */
        LONG EvID;                      /* Nummer des Input-Elements (Gadget, etc. oder 0) */
        LONG EvID2;                     /* Hilfs-ID-Feld für manche Events (Drag & Drop) */
        void *EvAddress;                /* Zeiger auf Input-Objekt (Button, Menü, etc.) */
        void *EvAddress2;               /* Hilfs-Zeiger für manche Events (Drag & Drop) */
        SHORT ButtonPressed;            /* MouseButton gedrückt == TRUE */
        SHORT EvPressure;               /* Druck bei Grafiktablett */
        ULONG UserCode;                 /* ev. ID-Code für die User-Message */
        void *UserMessage;              /* Zeiger auf Message vom User-Port */
 };


#define  EV_NOBUTTON      1             /* Events */
#define  EV_LBUTTONDOWN   2
#define  EV_LBUTTONUP     3
#define  EV_RBUTTONDOWN   4
#define  EV_RBUTTONUP     5
#define  EV_VANILLAKEY    6
#define  EV_RAWKEY        7
#define  EV_GADGET        8
#define  EV_GADGET_R      9
#define  EV_GADGET_DBL   10
#define  EV_MENUPICK     11
#define  EV_CLOSEWIN     12
#define  EV_ACTIVEWIN    13
#define  EV_INACTIVEWIN  14
#define  EV_REXXCALL     15
#define  EV_DOACTION     16
#define  EV_HELP         19
#define  EV_USER         20
#define  EV_INTUI        21
#define  EV_DRAGDROP     22

#define  DO_ACT_BUTTON   50             /* Kommandos an den Event-Prozess */

#define  ASK_GADGET   1<<0              /* gewünschte Events bei der Abfrage */
#define  ASK_MOUSE    1<<1
#define  ASK_KEYS     1<<2
#define  ASK_WINDOW   1<<3
#define  ASK_REXX     1<<4
#define  ASK_MENU     1<<5
#define  ASK_HELP     1<<7
#define  ASK_USER     1<<8
#define  ASK_DRAGDROP 1<<9
#define  ASK_INTUI    1<<11

#define  ASK_NOWAIT   1<<10


/****  Defines für Tag von XW_SetReg()-Funktion   ****/

#define  XW_SR_SETSYSFONTH        1     /* Größe des Systemfonts nachträglich setzen */
#define  XW_SR_GETUPDLOCK         2     /* Zeiger auf die Semaphore für den Update-Lock */
#define  XW_SR_BEGINUPDATE        3     /* ev. störende Maus vor Zeichenoperation entfernen, Data zeigt auf SHORT-Array mit X, Y, Width, Height */
#define  XW_SR_ENDUPDATE          4     /* Ende der Zeichenoperation, Wiederherstellen der Maus */
