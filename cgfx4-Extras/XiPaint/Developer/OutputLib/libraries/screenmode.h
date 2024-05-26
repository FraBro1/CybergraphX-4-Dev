/*
 * ScreenMode.h   enthält Deklarationen von Konstanten und Strukturen,
 *                die von anderen Funktionen gebraucht werden.
 *
 * $Id: ScreenMode.h,v 1.2 1993/06/24 13:28:19 David_Göhler Exp David_Göhler $
 */

#ifndef VILINTUISUP_SCREENMODE_H
#define VILINTUISUP_SCREENMODE_H

#ifndef EXEC_NODES_H
#include <exec/nodes.h>
#endif

#ifndef UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif

#define ZEICHENZAHL 40             /* Zeichenzahl des Listview-Gadgets  */

/*
 * Struktur zur Verwaltung von ScreenModes in einem normalen Listview
 * Da eine Exec-Node noch keine Speicherplatz für einen String enthält
 * (sie hat nur einen Zeiger auf einen String, aber keine Platz dafür)
 * muß man diesen der Node anfügen. Das gleiche gilt verständlicherweise
 * für die DisplayID.
 */

struct Zeile {                     /* Die Struktur heißt Zeile           */
    struct Node Node;              /* Zur Verkettung untereinander       */
    char        Text[ZEICHENZAHL]; /* ZEICHENZAHL Zeichen Platz          */
    ULONG       DisplayID;         /* ScreenModus-Display-ID             */
    UWORD       Width;             /* Breite dieses Modus                */
    UWORD       Height;            /* Höhe dieses Modus                  */
    UWORD       Depth;             /* Tiefe dieses Modus                 */
};

/*
 * Für das spezielle ScreenMode-Fenster der VilIntuiSup-Library kann
 * man ein TagItem-Array angeben. Hierüber läßt sich eine Auswahl
 * treffen, was für Texte ausgegeben werden und welche ScreenModes im
 * Fenster erscheinen.
 *
 * Im Hilfstext sollten "\n" vorkommen, wenn er länger wird.
 *
 * Font- und Fenstereinstellungen (Größe, Auswahl) kann man momentan
 * nicht treffen. Als Font wird der System-Default-Font verwendet.
 * Das Fenster wird immer mittig auf dem sichtbaren Bildschirmteil
 * des Default-Public-Screens geöffnet (wichtig bei Auto-Rollen-Screens).
 * Die Breite des Fensters wird durch die Breite des breitesten
 * Eintrags im gültigen Font errechnet. Die Höhe wird so gewählt, das
 * höchstens 12 Zeilen im Listview zu sehen sind.
 */

#define TAVIS_FIRSTITEM         TAG_USER + 4711 /* TAVIS = TagItemVilIntuiSup */

/****** Values for filtering alien modes *********************/
#define TAVIS_MINDEPTH          TAVIS_FIRSTITEM +  0
#define TAVIS_MAXDEPTH          TAVIS_FIRSTITEM +  1
#define TAVIS_MINHEIGHT         TAVIS_FIRSTITEM +  2
#define TAVIS_MAXHEIGHT         TAVIS_FIRSTITEM +  3
#define TAVIS_MINWIDTH          TAVIS_FIRSTITEM +  4
#define TAVIS_MAXWIDTH          TAVIS_FIRSTITEM +  5

/****** Values for string constants **************************/
#define TAVIS_WINDOWTITLE       TAVIS_FIRSTITEM +  6

#define TAVIS_LISTVIEW          TAVIS_FIRSTITEM +  7
#define TAVIS_OKGADGET          TAVIS_FIRSTITEM +  8
#define TAVIS_CANCELGADGET      TAVIS_FIRSTITEM +  9

#define TAVIS_HELP_TEXT         TAVIS_FIRSTITEM +  10
#define TAVIS_ABOUT_TEXT        TAVIS_FIRSTITEM +  11
#define TAVIS_NOTICE_TEXT       TAVIS_FIRSTITEM +  12
#define TAVIS_CHOOSE_TEXT       TAVIS_FIRSTITEM +  13

#define TAVIS_MENU_QUIT         TAVIS_FIRSTITEM +  14
#define TAVIS_MENU_HELP         TAVIS_FIRSTITEM +  15
#define TAVIS_MENU_ABOUT        TAVIS_FIRSTITEM +  16
#define TAVIS_MENU_PROJECT      TAVIS_FIRSTITEM +  17

/****** Values for special functions *************************/
#define TAVIS_CUSTOM_LIST       TAVIS_FIRSTITEM +  20

/*
 * Der neuen Funktion VillageModeID kann man diverse
 * TagItems übergeben. Die neuen IDs werden im folgenden
 * definiert. Die bereits definierten TAVIS_MIN/MAX...-
 * TagItems können auch bei VillageModeID verwendet werden.
 *
 * Nicht alle TagItems lassen sich mit allen anderen
 * frei kombinieren. Im Zweifelsfall hat das zuerst genannte
 * Tag Vorrang.
 */

/****** Values for screen handling ***************************/
#define TAVIS_SCREEN_HEIGHT     TAVIS_FIRSTITEM +  30
#define TAVIS_SCREEN_WIDTH      TAVIS_FIRSTITEM +  31
#define TAVIS_SCREEN_DEPTH      TAVIS_FIRSTITEM +  32
#define TAVIS_SCREEN_MODEID     TAVIS_FIRSTITEM +  33

/* set this TRUE in conjunction with the above tags          */
#define TAVIS_BEST_MODEID       TAVIS_FIRSTITEM +  34

/* to open a screen behind all others                        */
#define TAVIS_OPEN_BEHIND       TAVIS_FIRSTITEM +  35

/* if you want autoscroll, set this to TRUE and
 * suplly MODEID, DEPTH, HEIGHT and WIDTH                    */
#define TAVIS_AUTOSCROLL        TAVIS_FIRSTITEM +  36

/* if it is ok to get a smaller screen than the desired one
 * set ti_Data to TRUE, default ist TRUE.                    */
#define TAVIS_ALLOW_FALLBACK    TAVIS_FIRSTITEM +  37

/* if you want to get additional memory on the board
 * supply a pointer to a ULONG, where the desired size
 * is written. After allocation, you will find an
 * offset to the first adress of the screen you can get
 * via LockVillageScreen in the ULONG. If the offset is
 * zero, the allocation failed. The offset is only valid,
 * if the screen is locked.                                  */
#define TAVIS_ADDITIONAL_MEM    TAVIS_FIRSTITEM +  38

/* to here about errors, supply the address of a ULONG
 * for #define's see below                                   */
#define TAVIS_ERROR_CODE        TAVIS_FIRSTITEM +  39

/* if you want to get two screens, you can easily switch     */
#define TAVIS_DOUBLE_BUFFER     TAVIS_FIRSTITEM +  40

/* to supply an old style struct Dimensions structure
 * ti_Data should be a pointer to the structure              */
#define TAVIS_DM_STRUCT         TAVIS_FIRSTITEM +  41

/****** Error Codes for TAVIS_ERROR_CODE *********************/
#define TAVIS_ERR_OK                    0  /* all O.K.                        */
#define TAVIS_ERR_NOMEM                 1  /* not enough memory for screen    */
#define TAVIS_ERR_MODE_NOT_AVAILABLE    2  /* Mode with ModeID not present    */
#define TAVIS_ERR_DIM_TOO_BIG           3  /* dimensions are too big          */
#define TAVIS_ERR_DIM_TOO_DEPTH         4  /* color depth is too big          */
#define TAVIS_ERR_DOUBLE_BUF_TOO_BIG    5  /* not enough memory for doublebuf */
#define TAVIS_ERR_CANT_OPEN_SCREEN      6  /* original OpenScreen failed      */

#endif /* VILINTUISUP_SCREENMODE_H */
