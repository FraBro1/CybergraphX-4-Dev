/* Last Change: Herbert /  9. September 1995  11:28:50 */

/* libraries/xigfx.h
   Includefile fuer die xigfx.library und deren Anwendung
*/

/* Defines für XG_SetRegister(entry, data) */
#define IVERSION  0     /* Entry  (Returnvalue) */
#define IREVISION 1     /* Entry  (Returnvalue) */
#define XDPI      2     /* Entry */
#define YDPI      3     /* Entry */
#define XOUT      4     /* Adresse der Xout-Library für AA-Version der Xigfx */
#define XIWIN     5     /* Adresse der Xiwin-Library für AA-Version der Xigfx */

/****  Basis-Strukturen  ****/

//+ "struct Clip {"

struct Clip {
        struct Clip *PrevClip;
        struct Clip *NextClip;
        struct Box *ClippingBox;
        SHORT X1, X2;
        SHORT Y1, Y2;
      };
//-

//+ "union color{"

union color{
        LONG c24;
        UBYTE c8[4];
        };
//-

//+ "typedef RGBA                /* Braucht das jemand? */"

typedef                 /* Braucht das jemand? */
struct
{
  UBYTE r;      
  UBYTE g;      
  UBYTE b;      
  UBYTE a;      
}
RGBA;
//-


/******************************* Defines für DrawBoxBorder **************/
/*********** mode kann folgende Werte annehmen: *************************/
#define FULLBORDER 0
#define V_BAR      1
#define H_BAR      2
#define TITLEBAR   3
#define QTITLEBAR  4

/* XiAct übersiedelt nach Xpaint.h */


/****  Defines für Arten von Strukturen (Kind-Feld)  ****/

//+ "STR-Defines"
#define  STR_DISPLAY       0x58495001
#define  STR_BOX           0x58495002
#define  STR_BUTTON        0x58495003
#define  STR_NEWBUTTON     0x58495004
#define  STR_MENU          0x58495005
#define  STR_ITEM          0x58495006
#define  STR_SUBITEM       0x58495007
#define  STR_DROPAREA      0x58495008

#define  STR_CYCLE         0x58495009
#define  STR_LISTVIEW      0x5849500A
#define  STR_CHARLIST      0x5849500B
#define  STR_MOUSEINFO     0x5849500C
//-



/****  Display-Struktur  ****/

//+ "struct Display {"

struct Display {
        LONG Kind;                              /* Kennung = STR_DISPLAY */
        struct Display *NextDisplay;            /* Listen-Links */
        struct Display *PrevDisplay;                    /* Privat !! */

    /**  allgemeine Daten  **/
        short Width, Height;                    /* Größe des Displays */
        short BytesPerRow;                      /* Breite einer Zeile in Bytes */
        short Depth;                            /* Anzahl Bytes/Pixel */
        short MouseX, MouseY;                   /* aktuelle Mauskoordinaten */

    /**  Hardwarespezifische Pointer  **/
        struct RetinaScreen *RetScreen;         /* Zeiger auf Screen der Retina-Version */
        struct Window *InputWindow;             /* Zeiger auf Amiga-Eingabe-Fenster */

    /**  Dummy-Box in Display-Struktur für Menüs, etc.  **/
        struct Box *BackBox;                    /* Zeiger auf Dummy-Box-Struktur */

    /**  Speicher zum Zeichnen (Hintergrund-Display)  **/
        union color *PaintMem;                  /* Zeiger auf Zeichenspeicher des Displays */
        union color *BackMem;                   /* Zeiger auf Hintergrundspeicher des Displays */
        ULONG PaintMemSize;                     /* Größe dieses Speichers */
        BOOL PaintMemOn;                        /* Display hat Zeichenspeicher, wenn = TRUE */

    /**  Fensterverwaltung  **/
        struct Box *FirstBox;                   /* erste Box */
        struct Box *LastBox;                    /* letzte Box */
        struct Box *ActiveBox;                  /* gerade aktive Box */
        struct Box *LastActive;                 /* vorherige aktive Box */
        struct Box *MouseBox;                   /* Box, über der gerade die Maus steht */
        struct Clip *FirstScrClip;              /* von Fenstern und sonstigem verdeckte Bereiche des Screens */
        struct Clip *LastScrClip;

    /**  Zeichenverwaltung des Displays  **/
        struct Box *ActivePaint;                /* Box, in der die letzte Zeichenoperation war */

    /**  Subprozesse für Maus- und Fenster-Handling  **/
        struct Thread *MouseThread;             /* Zeiger auf Maus-Thread (Prozess) */
        struct Thread *EventThread;             /* Zeiger auf WinHandle-Thread (Prozess) */
        struct MsgPort *RawPort;                /* Port für Basis-Input-Events */
        struct MsgPort *EventPort;              /* Port für Input-Events aus WinHandle-Prozess */
        struct MsgPort *RawTickPort;            /* Ports für Timer-Ticker */
        struct MsgPort *EventTickPort;

    /**  Maushandling  **/
        short RMouseX, RMouseY;                 /* von der Maus abgedeckte Fläche */
        short RMouseWidth, RMouseHeight;
        long PointerType;                       /* Typen der Default-Fenster-Maus */
        long PaintType;                         /* Typen der Zeichen-Maus */
        long MouseType;                         /* aktuelle Maus-Typen */
        BOOL MouseAllowed;                      /* Maus darf nur gezeichnet werden, wenn = TRUE */
        BOOL KeepMouse;                         /* aktuelle Maus ändert sich nicht, wenn = TRUE */
        BOOL Busy;                              /* nur mehr Meldungen von System-Buttons werden durchgeschleift (Busy-Zustand) */
        BOOL LockEvLoop;                        /* Keine Gadget- und Menü-Behandlung durchführen, wenn = TRUE */
        struct MouseInfo *MouseInfo;            /* Zeiger auf aktuelle Daten der Maus */
        struct MouseInfo *NewMouse;             /* Zeiger auf gewünschte Größe der Maus */
        short MouseFixX1, MouseFixY1;           /* Fix-Punkte eines Rubberbands, spezifisch für dieses Display (nur Paint-Mouse) */
        short MouseFixX2, MouseFixY2;
        short MouseFixX3, MouseFixY3;
        short MouseFixX4, MouseFixY4;

    /**  Daten des gerade aktiven Stringeingabe-Buttons  **/
        struct Button *ActString;               /* Zeiger auf offenen String-Button oder NULL */
        struct Box *ActStringBox;               /* Zeiger auf dessen Box oder NULL */

    /**  Daten für Grid  **/
        BOOL GridOn;                            /* Grid für Zeichenfenster ist eingeschaltet */
        short GridX, GridY;                     /* Offset des Rasters von der linken oberen Ecke */
        short GridWidth, GridHeight;            /* Breite und Höhe des Rasters */

    /**  Daten für den Standard-Mauszeiger (R_PFEIL)  **/
        short PointerWidth, PointerHeight;      /* Größe des Mauszeigers */
        short PointerOffX, PointerOffY;         /* Hot-Spot-Offset für Mauszeiger */
        union color *PointerMem;                /* Zeiger auf Speicher für Mauspfeil */

    /**  Daten für den Busy-Mauszeiger (R_BUSY)  **/
        short BusyWidth, BusyHeight;            /* Größe des Busyzeigers */
        short BusyOffX, BusyOffY;               /* Hot-Spot-Offset für Busyzeiger */
        union color *BusyMem;                   /* Zeiger auf Speicher für Busy-Symbol */

    /**  Pool für Visible Clips  **/
        UBYTE *VisClips;                        /* Zeiger auf großen Speicher für VisClips für Update */
        ULONG VisClipSize;                      /* Größe dieses Speichers */

    /**  Hooks zum Beseitigen etwaiger fremder Einflüsse auf den Hardware-Screen  **/
        __far void (* __asm DisownScreen)(register __a0 struct Display *);      /* wird aufgerufen, wenn der Hardware-Screen freigegeben werden muß */
        __far void (* __asm OwnScreen)(register __a0 struct Display *);         /* der Screen ist für die Anwendung wieder frei */
      };
//-



/****  Strukturen für die Boxen und die Buttons ****/

#define NORMAL 1                                /* Simplerefresh */
#define SUPER  2                                /* SuperBoxMap */
#define NOBORDER   4                            /* keine Border darstellen */
#define BACKBOX    8                            /* Box im Hintergrund */
                                                        /* Abfrage: if ( Box->Type & SUPER )  -> Box ist SUPER */
                                                        /*      if ( Box->Type == (Box->Type | (SUPER|NOBORDER)) ) */


//+ "struct Cycle {"

struct Cycle {
        LONG Kind;                              /* Kennung = STR_CYCLE */
        struct Cycle *PrevCycle;                /* zeigt auf vorherigen Cycleeintrag */
        struct Cycle *NextCycle;                /* zeigt auf nächsten Cycleeintrag */
        char ButtonText[256];                   /* Text für Normaldarstellung */
        short Symbol;                           /* Symbol für Normaldarstellung */
      };
//-


//+ "struct ListView {"

struct ListView {
        LONG Kind;                              /* Kennung = STR_LISTVIEW */
        struct ListView *NextLV;                /* zeigt auf vorherigen Listeintrag */
        struct ListView *PrevLV;                /* zeigt auf nächsten Listeintrag */
        char ListText[256];                     /* Text für Listeneintrag */
        UBYTE Color;                            /* Farbregister des Textes */
        UBYTE Pri;                              /* Priorität des Eintrags, für Sortieren */
        struct Node AA_LVNode;                  /* zugehöriger AA-Listen-Node */
      };
//-


//+ "struct CharList {"

struct CharList {
        LONG Kind;                              /* Kennung = STR_CHARLIST */
        struct CharList *PrevChar;              /* zeigt auf vorherigen Listeintrag */
        struct CharList *NextChar;              /* zeigt auf nächsten Listeintrag */
        short Width;                            /* Breite des Zeichens */
        UBYTE Letter;                           /* Zeichen */
      };
//-


//+ "struct DropArea {"

struct DropArea {
        LONG Kind;                              /* Kennung = STR_DROPAREA */
        struct DropArea *PrevDrop;              /* zeigt auf vorherigen Listeneintrag */
        struct DropArea *NextDrop;              /* zeigt auf nächsten Listeneintrag */
        LONG AreaID;                            /* Kennung der Area */
        SHORT X1, Y1;                           /* Koordinaten des Area-Feldes (relativ zur Box) */
        SHORT X2, Y2;
      };
//-


//+ "struct NewButton {"

struct NewButton {
        LONG Kind;                              /* Kennung = STR_NEWBUTTON */
        SHORT X, Y;                             /* Felder für alle Buttons, Bedeutung siehe struct Button */
        SHORT Width, Height;
        LONG ButtonID;
        UBYTE ShortKey;
        USHORT ShortQual;

        char *Text;                             /* spez. Felder für normalen Button */
        char *TextHi;

        SHORT Symbol;                           /* spez. Felder für Image-Button */
        SHORT SymbolHi;
        SHORT SymSize;

        char *InitText;                         /* spez. Felder für String-Button */
        SHORT MaxChar;

        LONG InitValue;                         /* spez. Felder für Numeric- u. DNumeric-Button */
        double DInitValue;
        LONG MinValue, MaxValue;
        double DMinValue, DMaxValue;
        SHORT MaxDigit;

        LONG ScrollValue;                       /* spez. Felder für Scroll-Button */
        LONG MinScroll, MaxScroll;
        double ScrollSteps;
        LONG ScrollPage;
        __far void (* __asm UpdateFunc)(register __a0 struct Box *, register __a1 struct Button *);

        char **CycleTextList;                   /* spez. Felder für Cycle-Button */
        SHORT Count;
        void *UserData;                         /* Pointer zur Verfügung für User */
      };
//-


//+ "struct Button {"

struct Button {
        LONG Kind;                              /* Kennung = STR_BUTTON */
        struct Button *PrevButton;              /* Listen-Links */
        struct Button *NextButton;                      /* Privat !!! */
        short X, Y;                             /* Position */
        short Width, Height;                    /* Größe */
        LONG ButtonID;                          /* ID-Nummer für Check */
        unsigned short ButtonMode;              /* Mode des Buttons */
        unsigned short ButtonTextMode;          /* Mode für Textdarstellung (Text, HiText od. Image) */
        unsigned short RMBEnable;               /* if == 1, wird der Button auch von der rechten Maustaste gedrückt */
        char *ButtonText;                       /* Text für Normaldarstellung */
        char *ButtonTextHi;                     /* Text bei gedrücktem Button */
        short Symbol;                           /* Symbol für Normaldarstellung */
        short SymbolHi;                         /* Symbol bei gedrücktem Button */
        short SymSize;                          /* 1 = Normal, 2 = Faktor 2 etc. */
        BOOL ButtonDown;                        /* Button gedrückt = TRUE, sonst FALSE */
        char *TextString;                       /* zum Ausfüllen bei B_STRING und Rückgabe */
        struct Cycle *FirstCycle;               /* Links auf Cyclestruktur für Cyclebutton */
        struct Cycle *LastCycle;
        struct Cycle *ActCycle;                 /* gerade dargestellter Cycle-Text */
        short CyclePos;                         /* Nummer des gerade verwendeten Cycle-Feldes */
        short CycleCount;                       /* Anzahl der Cycle-Felder */
        struct ListView *FirstLV;               /* Links auf Listeneinträge für ListView-Button */
        struct ListView *LastLV;
        struct Button *LVScroll;                /* Links auf Unterbuttons eines ListViews */
        struct Button *LVField;
        struct Button *LVUp;
        struct Button *LVDown;
        struct ListView *TopLV;                 /* erster dargestellter Listeneintrag */
        struct ListView *SelectLV;              /* selektierter Listeneintrag */
        short TopLVPos;                         /* Nummer des ersten dargestellten Listeneintrags */
        short SelectLVPos;                      /* Nummer des selektierten Listeneintrags */
        short EntryHeight;                      /* Höhe eines Eintrages */
        short EntryCount;                       /* Anzahl der dargestellten Einträge */
        LONG MaxChars;                          /* Maximale Länge des Eingabestrings */
        LONG CurrChars;                         /* aktuelle Länge des Eingabestrings */
        struct CharList *FirstChar, *LastChar;  /* Spezialeinträge für String-Buttons (Handle-Links) */
        struct CharList *FirstVisChar, *LastVisChar;
        struct CharList *CursorChar;
        SHORT FirstVisible;                     /* erstes sichtbares Zeichen in längerem String */
        LONG NumValue;                          /* zum Ausfüllen bei B_VALUE und Rückgabe */
        LONG MinValue, MaxValue;                /* Constraints bei Value-Eingabe */
        double DNumValue;                       /* zum Ausfüllen bei B_DVALUE und Rückgabe */
        double DMinValue, DMaxValue;            /* Constraints bei Double-Value-Eingabe */
        LONG MinScroll, MaxScroll;              /* Bereich des Scrollbalkens */
        LONG ScrollVal;                         /* aktueller Wert */
        double ScrollSteps;                     /* Schrittweite des Vals bei Sprung des Scrollers um einen Pixel */
        LONG ScrollPage;                        /* Anzahl der Schritte bei einem Klick in ein leeres Feld */
        short ScrollerWidth;                    /* Breite des Scrollers in Pixeln (intern!) */
        short ScrollerOff;                      /* Offset des Scrollers in Pixeln (intern!) vom linken Buttonrand (<- 2 Pixel breit) */
        __far void (* __asm UpdateFunction)(register __a0 struct Box *, register __a1 struct Button *);
                                                /* Funktion, die aufgerufen wird, solange der Button gedrückt ist */
        BOOL Rendered;                          /* = TRUE, wenn schon dargestellt */
        UBYTE ShortKey;                         /* ASCII-Wert für Gadget-Shortcut (od. 0 für keinen) */
        USHORT ShortQual;                       /* Qualifier zu ShortKey - im Moment nicht verwendet */
        SHORT ShortPos;                         /* Position des ShortKeys im Gadget-Text (-1 für nicht vorhanden) */
        BOOL Dragable;                          /* wenn = TRUE, kann der Button mit der Maus gezogen werden */
        BOOL Keep;                              /* wenn = TRUE, dann soll Button nicht freigegeben werden */
        void *UserData;                         /* Zeiger auf private Daten des Users */
        short ArrLX, ArrRX;                     /* interner Gebrauch */
        short ArrTY, ArrBY;                             /* Privat !!! - Hitbox */
        short SlArrLX, SlArrRX;                 /* interner Gebrauch */
        short SlArrTY, SlArrBY;                         /* Privat !!! - Hitbox des Sliders bei Scroll-Button */
    /**  spezielle Felder für die Amiga-Versionen  **/
        struct Gadget *AA_FirstGadget;          /* Zeiger auf erstes Gadget der Amiga-(GadTools-)Version */
        struct Gadget *AA_LastGadget;           /* letztes AA-Gadget */
        BOOL AA_Set;                            /* Gadget ist im Fenster gesetzt... */
        SHORT AA_GadCount;                      /* Länge der Gadget-Liste */
        struct List AA_LVList;                  /* Zeiger auf Listenkopf für ListView-Gadget */
        char **AA_CycleList;                    /* Zeiger auf Liste mit Texten für das Cycle-Gadget */
        UWORD *AA_BorderDataA;                  /* Zeiger auf Koordinaten der Gadget-Border bei Bool-Gads... */
        UWORD *AA_BorderDataB;
        struct Border *AA_BorderA;              /* Border-Strukturen für die 3D-Border */
        struct Border *AA_BorderB;
        struct Border *AA_BorderAHi;
        struct Border *AA_BorderBHi;
        struct IntuiText *AA_IntuiText1;        /* Struktur für Gadget-Text */
        struct IntuiText *AA_IntuiText2;        /* Struktur für Gadget-Underscore */
        UBYTE Dummy[20];                        /* für spätere Erweiterungen */
      };
//-


//+ "struct Box {"

struct Box {
        LONG Kind;                              /* Kennung = STR_BOX */
        struct Box *PrevBox;                    /* Listen-Links */
        struct Box *NextBox;                            /* Privat !!! */
        short X, Y;                             /* Position */
        short Width, Height;                    /* Größe */
        short Depth;                            /* 8, 32 */
        short Type;                             /* NORMAL, SUPER, NOBORDER, BACKBOX */
        UBYTE *BoxLayer;                        /* Zeiger auf Speicher mit Inhalt der Box */
        short MinWidth, MinHeight;              /* Grenzen beim Sizen */
        short MaxWidth, MaxHeight;
        short AltX, AltY;                       /* zweite Box-Postiton und -Größe */
        short AltWidth, AltHeight;              
        short BMapX, BMapY;                     /* Offset in der übergroßen Boxmap einer SUPER-Box */
        short BMapWidth, BMapHeight;            /* Größe der übergroßen BoxMap bei SUPER */
        short MouseX, MouseY;                   /* Box-relative Mauskoordinaten */
        short BMapMouseX, BMapMouseY;           /* Maus-Koordineten bezüglich der großen Bitmap einer SUPER-Box */      
        unsigned char *Title;                   /* Titelzeiger: NULL = kein Titel */
        long MouseType;                         /* Typen der Maus für dieses Fenster */
        UBYTE Background;                       /* Farbregister des Box-Hintergrundes bei normaler Box */
        LONG BoxID;                             /* ID-Nummer der Box */
        LONG CloseID, DragID;                   /* Gadget-IDs: 0 = kein Gadget */
        LONG DepthID, SizeID, AltSizeID;
        LONG ScrollXID, ScrollYID;              /* Scrollbars bei SUPER-Box (wird von OpenBox() belegt !!) */
        struct Button *CloseButton;             /* Zeiger auf die Fenster-Gadgets */
        struct Button *DragButton;                      /* Privat !!! */
        struct Button *DepthButton;
        struct Button *SizeButton;
        struct Button *AltSizeButton;
        struct Button *ScrollXButton, *ScrollYButton;
        struct Button *FirstButton;             /* erster Button d. Box (privat) */
        struct Button *LastButton;              /* letzter Button d. Box (privat) */
        struct BMenu *FirstMenu;                /* erstes Menü d. Box (privat) */
        struct BMenu *LastMenu;         /* letztes Menü d. Box (privat) */
        struct DropArea *FirstDrop;             /* Zeiger auf Liste mit den Drop-Clients */
        struct DropArea *LastDrop;
        BOOL BoxActive;                         /* Box aktiv für Input */
        UBYTE *BorderMemT, *BorderMemB;         /* Zeiger auf separaten Rahmen-Speicher einer SUPER-Box */
        UBYTE *BorderMemL, *BorderMemR;
        short BorderWidthT, BorderWidthB;       /* Breite der jeweiligen Rahmens */
        short BorderWidthL, BorderWidthR;
        struct Clip *FirstClip;                 /* Liste der verdeckten Clips (privat) */
        struct Clip *LastClip;
        struct Display *BoxDisplay;             /* Zeiger auf das der Box zugeordnete Display */
        short BoxX1, BoxX2;                     /* Eck-Koordinaten der Box für schnellen Zugriff */
        short BoxY1, BoxY2;
    /**  Hilfsdaten für Maus  **/
        short MouseFixX1, MouseFixY1;           /* Fix-Punkte eines Rubberbands, spezifisch für diese Box */
        short MouseFixX2, MouseFixY2;
        short MouseFixX3, MouseFixY3;
        short MouseFixX4, MouseFixY4;
    /**  Button-Refresh, etc.  **/              
        BOOL NoRefresh;                         /* wenn NoRefresh == TRUE, kein Auto-Refresh beim Resizen */
    /**  Daten für Update-Hook  **/
        short UpdateMode;                       /* wenn != 0, bei SUPER-32-Box Update über Hook mit gew. Mode */
        UBYTE *UpdateMem;                       /* Temporärer Speicher für Hook-Update */
        short UpMemWidth, UpMemHeight;          /* Größe dieses Speichers */
    /**  spezifische Systemdaten  **/
        struct Window *AABoxWindow;             /* Zeiger auf Amiga-Fenster und Menü (nur AA) */
        struct Menu *AABoxMenu;
    /**  User-Daten für XiPaint  **/
        struct XiAct *XiAct;                    /* Zeiger auf Struktur mit zu diesem Fenster gehörigen Zeicheneinstellungen */
        struct Box *BackLink;                   /* Zeiger auf Box, die den Hintergrund bildet (Durchzeichnen) */
        ULONG RedoQueue;                        /* Anzahl der im Undo wiederhergestellten Elemente */
        struct Box *ZoomLink;                   /* Zeiger von Paint- auf Zoomfenster */
        struct Box *PaintLink;                  /* Zeiger von Zoom- auf Paintfenster */
        short ZX1, ZY1, ZX2, ZY2;               /* Eckpunkte des Zoom-Bereichs */
        short ZoomFactor;                       /* Vergrößerungsfaktor */
        BOOL PaintLock;                         /* wenn = TRUE, darf in der Box nicht gezeichnet werden */
        BOOL BackPaint;                         /* wenn = TRUE, ist die Box die Hintergrund-Box */
    /**  Spezielle Daten für AA_SUPER_Boxen  **/
        Object *AA_HorizScroll, *AA_VertScroll; /* Scrollbars */
        Object *AA_LeftGadget, *AA_RightGadget; /* Scrollpfeile */
        Object *AA_UpGadget, *AA_DownGadget;
        LONG AA_HTotal, AA_VTotal;              /* Breite / Höhe der BitMap (wie BMapWidth) */
        LONG AA_HVisible, AA_VVisible;          /* gerade sichtbare Größe */
        BOOL AA_Frozen;                         /* Zeichensperre bei Resize des Fensters */
        BOOL AA_MenuSet;                        /* wenn TRUE: Menü am Amiga-Fenster gesetzt */
        BOOL MenuLend;                          /* wenn TRUE: die verwendeten Menüs sind nicht unsere... */
        SHORT AA_MouseType;                     /* Typ der gesetzten Amiga-Maus */
        UBYTE Dummy[20];                        /* Reserve für später... */
      };
//-


//+ "Button-Defines"

#define BA_BUTTON       0       /* Defines, die von Button-Add-Funktionen gebraucht werden (ButtonMode-Feld) */
#define BA_TOGGLE       1                       /* Defines für AddNormal() und AddImage() */
#define BA_HALF_T       2
#define BA_HALF_B       3
#define BA_BUTTON_DR    4                       /* Buttons sind Drag&Drop-fähig */
#define BA_TOGGLE_DR    5
#define BA_HALF_T_DR    6
#define BA_HALF_B_DR    7

#define BA_HIDDEN       8                       /* Defines für den unsichtbaren Button (für AddNormal()) */
#define BA_HIDDEN_DR    9                              /* Drag & Drop - Version */

#define BA_HOR          0                       /* Defines für AddScroll() */
#define BA_VER          1
#define BA_HORCOL       2
#define BA_INTEGER      0                       /* Defines für AddNumeric() */
#define BA_DOUBLE       1

#define B_BUTTON        1       /* Buttonmode (privat - xigfx, Window) */
#define B_TOGGLE        2
#define B_HALF_T        3
#define B_HALF_B        4
#define B_STRING        5
#define B_VALUE         6
#define B_DVALUE        7
#define B_SCROLLHOR     8
#define B_SCROLLVER     9
#define B_SCROLLHORC    10              /* spezieller Scroller für Farbuntergrund */
#define B_LISTVIEW      11
#define B_CYCLE         12
#define B_CHECK         13
#define B_HIDDEN        14

#define S_CLOSE         40      /* Buttonmode (privat - xigfx, Window) */
#define S_DRAG          41
#define S_SIZE          42
#define S_ALTSIZE       43
#define S_DEPTH         44
#define S_SCROLLX       45
#define S_SCROLLY       46

#define T_NONE          0       /* TextMode (in allen Funktionen) */
#define T_TEXT          1
#define T_SYMBOL        2
#define T_CYCLE         3

#define MB_LEFT         1               /* Defines für Maustasten und -Qualifer */
#define MB_RIGHT        2

#define MQ_NONE         0
#define MQ_SHIFT        1
#define MQ_DOUBLE       2

#define SCQ_NONE        0               /* Qualifier für Shortcuts - im Moment nicht unterstützt */
#define SCQ_SHIFT       1
#define SCQ_ALT         2
#define SCQ_SHIFTALT    3
#define SCQ_CTRL        4
#define SCQ_AMIGA       5
#define SCQ_COMMAND     6

#define SC_NONE         0               /* kein Shortcut gewünscht */
//-

//+ "Defines für Funktions-Modes"

#define  SLV_POS     1                  /* Mode-Defines für XW_SetListView() */
#define  SLV_HI      2
#define  SLV_HIPOS   3
//-


/****  Menü-Strukturen  ****/

//+ "Menü-Defines"
#define  M_STD        0                 /* Menü- und Item-Modes */
#define  M_DISABLED   1
#define  M_DUMMY      2
#define  M_CHECKMARK  4
#define  M_CHECKED    8
#define  M_HILIGHT   16

#define  MID_NONE     0                 /* keine Menü-ID */
//-

//+ "struct BMenu {"

struct BMenu {
        LONG Kind;                              /* Kennung = STR_MENU */
        struct BMenu *PrevMenu;                 /* Listen-Links */
        struct BMenu *NextMenu;
        short X, Y;                             /* Größe und Ort des Titel-Eintrags */
        short Width, Height;
        short Mode;                             /* Status des Menüs (DISABLED) */
        LONG MenuID;                            /* ID-Nummer des Menüs (muß eindeutig sein) */
        char *MenuTitle;                        /* Titel-Text */
        BOOL MenuDown;                          /* Menü dargestellt */
        BOOL Hilight;                           /* Maus über Menü */
        struct BItem *FirstItem;                /* Liste der Menü-Items */
        struct BItem *LastItem;
        short MX1, MX2;                         /* Hitbox für Maus */
        short MY1, MY2;
        struct Box *RefBox;                     /* zugehörige normale Box */
        struct Box *MenuBarBox;                 /* Box der Titelzeile */
        struct Box *MenuBox;                    /* Box der Menüitems */
        struct Menu *AAMenu;                    /* Zeiger auf Amiga-Menü, privat */
      };
//-

//+ "struct BItem {"

struct BItem {
        LONG Kind;                              /* Kennung = STR_ITEM */
        struct BItem *PrevItem;                 /* Listen-Links */
        struct BItem *NextItem;
        short X, Y;                             /* Größe und Ort des Titel-Eintrags */
        short Width, Height;
        short Mode;                             /* Status des Items (DUMMY, DISABLED, CHECKMARK, CHECKED) */
        LONG ItemID;                            /* ID-Nummer des Items (muß eindeutig sein) - nicht notw. bei Item mit Submenüs */
        char *ItemText;                         /* Item-Text */
        UBYTE ShortKey;                         /* ASCII-Wert für Item-Shortcut (od. 0 für keinen) */
        USHORT ShortQual;                       /* Qualifier zu ShortKey - im Moment nicht verwendet */
        BOOL ItemDown;                          /* Item dargestellt */
        BOOL Hilight;                           /* Maus über Item */
        struct BSubItem *FirstSub;              /* Liste der Submenü-Items */
        struct BSubItem *LastSub;
        short IX1, IX2;                         /* Hitbox für Maus */
        short IY1, IY2;
        struct BMenu *RefMenu;                  /* zugehöriges Menü */
        struct Box *SubMenuBox;                 /* Box eines etw. Submenüs */
        struct MenuItem *AAItem;                /* Zeiger auf Amiga-MenüItem, privat */
      };
//-

//+ "struct BSubItem {"

struct BSubItem {
        LONG Kind;                              /* Kennung = STR_SUBITEM */
        struct BSubItem *PrevSub;               /* Listen-Links */
        struct BSubItem *NextSub;
        short X, Y;                             /* Größe und Ort des Titel-Eintrags */
        short Width, Height;
        short Mode;                             /* Status des SubItems (DUMMY, DISABLED, CHECKMARK, CHECKED) */
        LONG SubID;                             /* ID-Nummer des SubItems (muß eindeutig sein) */
        char *SubText;                          /* SubItem-Text */
        UBYTE ShortKey;                         /* ASCII-Wert für Item-Shortcut (od. 0 für keinen) */
        USHORT ShortQual;                       /* Qualifier zu ShortKey - im Moment nicht verwendet */
        BOOL SubDown;                           /* SubItem dargestellt */
        BOOL Hilight;                           /* Maus über SubItem */
        short SX1, SX2;                         /* Hitbox für Maus */
        short SY1, SY2;
        struct BItem *RefItem;                  /* zugehöriges Menü-Item */
        struct MenuItem *AASub;                 /* Zeiger auf Amiga-SubItem, privat */
      };
//-



/****  Struktur für Maus-Info  ****/

//+ "struct MouseInfo {"

struct MouseInfo {
        LONG Kind;                      /* Kennung = STR_MOUSEINFO */
        long Status;                    /* Ein/aus, welche Art */
        short MouseX, MouseY;           /* Pointer-Koordinaten */
        short BrushX, BrushY;           /* Pinsel/Brush-Koordinaten */
        short BrushWidth, BrushHeight;
        short RubberX1, RubberY1;       /* Rubber-Koordinaten (Rect, etc.) */
        short RubberX2, RubberY2;
        short RubberX3, RubberY3;
        short BoxX, BoxY;               /* Koordinaten für bewegte Rubber-Box */
        short BoxWidth, BoxHeight;
        short BoxOffX, BoxOffY;         /* gew. Offset der Rubber-Box zu Cursor */
        short WaitX, WaitY;             /* Merker für Wait-Pointer */
        BOOL WaitOn;
      };        
//-

//+ "Rubber-Defines"
#define  R_CROSS   1
#define  R_PFEIL   2
#define  R_STRICH  4
#define  R_RECT    8
#define  R_RECT2   16
#define  R_BEZIER  32
#define  R_RAUT    64
#define  R_ELLIPS  128
#define  R_LINE    256
#define  R_BRUSH   512
#define  R_PINSEL  1024
#define  R_STAMP   2048
#define  R_BUSY    4096
#define  R_BOX     8192


#define  AAM_POINTER    1               /* Defines für AA-Maus-Merker */
#define  AAM_BUSY       2
#define  AAM_EMPTY      3
#define  AAM_CROSS      4
//-

