/* Last Change: Thomas /   6. August 1995  08:46:49 */
/****  Header zu UserHandle.c  ****/

/****  Strukturen und Defines zu User-Messages  ****/

/* //////////////     B E G I N N     öffentliches XiUser.h      ////////////// */

/**  User-Message-Struktur  **/

/* Sie muß vom User an den xiuser.port gesendet werden. Da die Messages mit Reply behandelt
   werden, muß der Reply-Port in der ExecMess ausgefüllt werden.
   Je nach bestellter Aktion sind die benötigten Parameter-Felder auszufüllen.
   War die Aktion erfolgreich, so ist Success auf TRUE, und die Rückgabe-Werte sind in den
   entsprechenden Feldern eingetragen. Warten auf das Reply ist daher immer nötig, da XiPaint
   nicht immer sofort auf ein Kommando reagieren kann. Genaueres zu den Kommandos siehe bei den
   Defines.

   Struct for sending to the xiuser.port. Please check Success;
   if the command was Successfull, it returns TRUE, else FALSE.
   You have to wait for the reply - because this is a synchronious
   port!

   The memory within XiPaint is 32-Bit-wide: RGBA RGBA ....
*/

struct XiMessage {
        struct Message ExecMess;        /* Exec-Message-Struktur */
        ULONG Label;                    /* Erkennungsmarke, muß 0x12345678 sein */
        UWORD Command;                  /* gewünschtes Kommando, siehe Defines */
        BOOL Success;                   /* bei Reply: Kommando war erfolgreich, wenn TRUE */
        char *ProjectName;              /* Name des zu bearbeitenden Projekts */
        SHORT Width, Height;            /* Größe des Projekts */
        UBYTE *Memory;                  /* Zeiger auf Speicher, 4 Byte/Pixel mit RGBA */
        char **ProjectList;             /* Zeiger auf Liste der Projekt-Namen, bei XI_GET_PROJECT_LIST */
        SHORT ProjectCount;             /* Anzahl der Projekte */
      };


#define KENNUNG "XIPAINT-EXTERNAL000"
struct xi_ext_header{
  char key[20];             // XIPAINT-EXTERNAL000
  WORD version, revision;   // From XiPaint
  char what;                // L=Loader, S=Saver, F=Filter
  char format[20];          // Which format
  char author[40];          // Name and Copyright from Author
  WORD dver, drev;          // Version of this driver
  char f24,f16,f8,f4,f2,f1; // supported depths (0/1) (saver!)
  char alpha,clut,cmap;     // 0/1
  };


struct io_pic{
  struct io_pic *next;    // For internal chaining
  UWORD picw, pich;       // Real Picdimension
  UWORD memw, memh;       // Memory-Dimension (for loading)
  char *memory;           // Memory to save from
  char name[128];         // Full name with path for saver
  char param[128];        // Special parameter from user
  char port[30];          // Name of the XiPaint - Port
  UWORD xdpi, ydpi;       // DPI-Chunk
  char depth;             // 1,2,4,8,16,24
  char jpegf;             // Jpeg loss factor
  char jpegs;             // Jpeg smooth 0/1
  char *clut;             // Pointer to a clut (clutf-entries)
  struct xi_ext_header ext; //
  /*                       Nur für dieses Modul ... */
  char proname[80];       // Projektname
  WORD brushnr;           // Brushnummer
  char todo;              // Brush/Project,Load/pAlette
  char minipic;           // 0/1
  };

/**  mögliche Kommandos  **/

/* Achtung: Kommando war nur erfolgreich, wenn bei Reply Success auf TRUE ist !!! */

#define  XI_GET_PROJECT_LIST    1

/*      Aktion:         gibt Liste mit Namen aller offenen Projekte zurück
                        Gives a list of all available XiPaint Projects
        Parameter:      keine
                        none

        Return:         ProjectList:    Zeiger auf Array von Projekt-Namen (nach XiPaint-Konvention)
                                        Pointer to an array of Project-Names
                        ProjectCount:   Anzahl der Projekte
                                        Count of Projects

                Achtung: Listen-Speicher muß vom User mit "FreeVec(ProjectList)" freigegeben werden !!!
                Attention: You have to free the List-memory with the function FreeVec(ProjectList)!
*/


#define  XI_LOCK_PROJECT        2

/*      Aktion:         sperrt bestimmtes Projekt für Zugriffe durch XiPaint
                        Locks a specified project.

        Parameter:      ProjectName:    Name des zu sperrenden Projekts (XiPaint-Konvention)
                        Name of the project to lock.

        Return:         Width:
                        Height:         Größe des Projekts
                        Memory:         Zeiger auf Speicher des Projekts
*/


#define  XI_UNLOCK_PROJECT      3

/*      Aktion:         gibt Projekt für XiPaint wieder frei
                        Unlocks the project. Now XiPaint can work...

        Parameter:      ProjectName:    Name des freizugebenden Projekts (XiPaint-Konvention)

        Return:         keine
*/


#define  XI_NEW_PROJECT         4

/*      Aktion:         Öffnen eines neuen Projektes in XiPaint, wird gleichzeitig auch
                        gesperrt, muß daher nach Benutzung mit XI_UNLOCK_PROJECT freigegeben
                        werden

                        Opens a new Project in XiPaint and locks it for you to use. After your work
                        you have to unlock it wiht XI_UNLOCK_PROJECT

        Parameter:      ProjectName:    Name des neuen Projekts (muß hier Zeiger auf initialisiertes char-Array (String) sein !!!)
                                        WICHTIG: Die ersten 4 Zeichen müssen mit "____" (4 'Underscores') gefüllt sein,
                                                 da XiPaint dort im Puffer eine eindeutige Kennung einsetzt !
                                                 Erfüllt der Name nicht diese Voraussetzung, wird die alte Konvention verwendet
                                                 (Kompatibilität mit alten Programmen, siehe unten).
                                                 Nach dem Reply der Message steht Puffer der neue Name mit Kennung,
                                                 der danach zur Referenz auf das Projekt verwendet werden muß.
                                        Attention: The first 4 charcters have to be filled out as "____", because
                                                   XiPaint fills out a unic number.
                        Width:
                        Height:         gewünschte Größe

        Return:         Memory:         Zeiger auf Speicher des Projektes
                        Width:
                        Height:         tatsächliche geöffnete Größe !!!
*/


#define  XI_UPDATE_PROJECT              5

/*      Aktion:         zeigt Veränderungen des Projekt-Puffers an
                        sollte nach Veränderungen aufgerufen werden

                        Refreshs the picture in XiPaint. Use this after manipulating
                        the memory of a project.

        Parameter:      ProjectName:    Name des Projekts (XiPaint-Konvention)

        Return:         keine
*/


#define  XI_SCREEN_TO_FRONT             6

/*      Aktion:         bringt den Bildschirm von XiPaint nach vorne

        Parameter:      keine

        Return:         keine
*/


#define  XI_SCREEN_TO_BACK              7

/*      Aktion:         bringt den Bildschirm von XiPaint nach hinten

        Parameter:      keine

        Return:         keine
*/

#define  XI_NEW_BRUSH         8

/*      Aktion:         Holt Speicher für einen Brush. Dieser kann dann beschrieben werden.
                        Um den Brush dann dem System bekanntzugeben, muß noch
                        XI_UPDATE_BRUSH aufgerufen werden.

                        Gets the memory for an XiPaint-Brush. To activate the
                        Brush in XiPaint, call XI_UPDATE_BRUSH.

        Parameter:
                        Width:
                        Height:         gewünschte Größe

        Return:         Memory:         Zeiger auf Speicher des Projektes  (oder NULL)
                        Width:
                        Height:         tatsächliche geöffnete Größe !!!
*/

#define  XI_UPDATE_BRUSH      9

/*      Aktion:         Fügt den Brush in die Brush-Queue ein und macht ein Update.
                        Puts the manipulated brush in the XiPaint brush-queue.

        Parameter:      Memory: Speicher des Brushs.
                        Width:
                        Height:         Größe

        Return:         Keine
*/

#define  XI_SET_PICINFO      10

/*      Aktion:         Setzt zum zugehoerigen Projekt xdpi, ydpi und den Author

                        Sets xdpi and ydpi of the specified Project

        Parameter:      ProjectName:    Name of the Projekts (XiPaint-Konvention)
                        Memory:         0-Terminated String des Authors
                        Width:          xdpi
                        Height:         ydpi

        Return:         Keine
*/

#define  XI_GET_BRUSH        11

/*      Action:         Gets the memory, width & height of the current brush

        Parameter:      none

        Return:         Memory: Pointer to the brush-memory
                        Width:  of the brush
                        Height: of the brush
*/

#define  XI_PUT_ERROR        12

/*      Action:         Pops up an Error-Message in XiPaint

        Parameter:      Memory: 0-Terminated String with your Message

        Return:         nothing
*/

#define  XI_OPEN_GAUGE       13

/*      Action:         opens up a gauge

        Parameter:      Memory: 0-Terminated String to the Title

        Return:         Memory:  Pointer to the box of the gauge
*/

#define  XI_MOVE_GAUGE       14

/*      Action:         Moves the gauge. 100 terminates the gauge.

        Parameter:      Memory: Pointer to the box of the gauge
                        Width: Value between 0 - 100

        Return:         nothing
*/

#define  XI_GET_PALETTE      15

/*      Action:         Gives the pointer of a 256 Items Entry RGBA Color Palette

        Parameter:      none

        Return:         Memory: Pointer of a union color Array like:
                        union color{
                             LONG c24;
                             UBYTE c8[4];
                             };
                        #define RED 0
                        #define GRN 1
                        #define BLU 2
                        #define KEY 3
*/
#define  XI_UPDATE_PALETTE    16

/*      Action:         Updates alle dependices to the palette

        Parameter:      none

        Return:         nothing
*/

#define  XI_MOVE_PIC          17

/*      Action:         XiPaint copies the memory-block for further action
                        E.g.: Minipic etc.
                        This function returns after doing something with this
                        memory.
                        The application has to free this memory by itself.
                        The memory has to be RGBK-organised, like the Projects.

        Parameter:      Memory-Pointer, Width, Height, ID
                        char *ProjectName   ... ID-String (from Request)
                        SHORT Width, Height ... Memorysize
                        UBYTE *Memory;      ... Memory

        Return:         nothing
*/

/*
   !!!  HINWEIS  !!!

   Die Projektnamen in XiPaint folgen von nun an der Konvention  "XXX_BildName", wobei XXX für eine
   dreistellige von XiPaint erzeugte Kennung steht, mit der das Bild auch bei gleichen Namen
   von mehreren Projekten unterschieden werden kann. Wird ein "alter" Name ohne die Kennung am
   Anfang verwendet, sucht XiPaint das Projekt nach dem alten Verfahren, das aber nicht eindeutig ist.
   Daher sind beim Erzeugen eines neuen Projekts mit XI_NEW_PROJECT die 4 Underscores am Anfang des
   Namens notwendig, um XiPaint anzuzeigen daß die neue Konvention verwendet werden kann. Anstelle
   der Underscores füllt XiPaint die aktuelle Kennung ein. Werden keine Underscores angegeben, so erfolgt
   die Öfnnung und der Zugriff auf dieses Projekt nach dem alten Modus, bei dem die Eindeutigkeit des Zugriffs
   nicht gegeben ist und es so zu Problemen kommen kann. Neue Programme sollten auf jeden Fall NUR die neue
   Methode verwenden.
*/



/* //////////////     E N D E     öffentliches XiUser.h      ////////////// */


/****  Strukturen und Defines für Studio-Anbindung  ****/

#define STUDIO_SERVER   1       /* version of server message structure */

/* definitions for "command"   */
#define STUDIO_PRINT  0x01      /* Print pic file or picture in memory                          */
#define STUDIO_QUIT       0x02  /* Quit Studio after all commands are performed. This command may
                                   be combined with STUDIO_PRINT Even if issueing the QUIT command,
                                   the server port remains as long as a print task is running. You
                                   can issue any print command as long as the port is running and
                                   thus disable the previous QUIT command if wanted                   */

/* definitions for "perform" */
#define STUDIO_DELETE   0x01    /* Set if you want to automatic delete file after printing This
                                   flag is only of use if prtfile[0] <> 0! This flag has no meaning
                                   when printing from memory. If you want to free the image memory,
                                   append the image memory to the message and do not define a
                                   replyport. As the server calls FreeVec on any print command
                                   message not having a replyport after printing, the image memory
                                   is freed with the message                                              */

#define STUDIO_NOREQ    0x02    /* Set if you don't want Studio error requesters to popup       */
 /* Warning: in some real bad cases requesters still may popup   */
 /* NOT SUPPORTED YET!                                           */

#define STUDIO_NOWINDOW 0x04    /* No print status window will be opened by Studio.             */
 /* NOT SUPPORTED YET!                                           */
#define STUDIO_NOASYNC  0x08    /* Set if you don't want to print in background. This flag must
                                   only be set if a message replyport was set in the message body.                                     */
#define STUDIO_NOSETUP  0x10    /* If set, no window will appear to the user allowing him to define
                                   the usual server print parameters. Do not use this setting
                                   unless you don't run under Intuition */
#define STUDIO_PARAM    0x20    /* If set, you can define some global Studio parameters. This flag
                                   should be used if STUDIO_NOSETUP or STUDIO_NOWINDOW is defined */


struct ServerMsg
  {
    struct Message message;     /* message body - if no replyport is given, FreeVec will be called
                                   on message pointer by Studio after processing                                           */
    ULONG version;              /* version of struct StudioMsg alias STUDIO_SERVER      */
    UBYTE servername[36];       /* Server name - Length limited to 35 CHARS!            */
    ULONG command;              /* MAIN command to perform - definitions see above      */

/* If STUDIO_PRINT is set in "command", fill in necessary print infos below     */

    ULONG perform;              /* Attributes for "command" - flags see above           */
    UBYTE prtfile[512];         /* file to print -  or prtfile[0]=0 if memory is passed */
    UWORD err;                  /* if not 0 an error has occured while printing: 1 = fatal error 2
                                   = job was canceled - maybe because of error... however user was
                                   informed unless STUDIO_NOREQ was defined 3 = User intentionaly
                                   canceled your job - can't make up his mind! */
    UBYTE errormsg[256];        /* returns EasyRequest message text with exact cause of possible
                                   Studio failure. Note: Text may contain several lines devided by
                                   \n. The errormsg was allready shown by Studio unless
                                   STUDIO_NOREQ was defined. If STUDIO_NOREQ was defined, make sure
                                   that errortext is shown (only if errormsg[0] <> 0 && err==1) */
    UBYTE xAspect;              /* Aspect of picture - is used by Studio to automaticly determine
                                   height of printout.        */
    UBYTE yAspect;              /* Aspect should only be supplied if printing from memory Aspect is
                                   overriden by dpi below                                                          */
    UWORD dpi_x;                /* Density of picture - is used by Studio to automaticly determine
                                   height of printout.       */
    UWORD dpi_y;                /* Should only be supplied if printing from memory                                           */

    /* These are some controls over basic functions of Studio and should be setable in the
       application program if STUDIO_NOSETUP is used. Note: the paramters are only accessable if
       STUDIO_PARAM is set                 */

    WORD red;                   /* global red adjustment. Range -50 (dark) up to 50 (bright) */
    WORD green;                 /* global green adjustment. Range -50 up to 50             */
    WORD blue;                  /* global blue adjustment. Range -50 up to 50              */
    WORD brightness;            /* global brightness adjustment. Range -50 up to 50        */
    WORD contrast;              /* global brightness adjustment. Range -50 up to 50        */
    WORD gamma;                 /* global gamma adjustment. Range -50 up to 50             */
    UWORD copies;               /* number of copies to print                               */
    UWORD landscape;            /* 0=portrait  1=landscape - rotation is handled by Studio */

    /* if prtfile == NULL these settings define the picture in memory to print */

    UBYTE *redsource;           /* ptr to first ubyte red|greyscale source row */
    UBYTE *greensource;         /* ptr to first ubyte green source row - or NULL if greyscale */
    UBYTE *bluesource;          /* ptr to first ubyte blue source row  - or NULL if greyscale */
    LONG redinc;                /* number that must be added to redsource address to get to next
                                   pixel in row */
    LONG greeninc;              /* number that must be added to greensource address to get to next
                                   pixel in row */
    LONG blueinc;               /* number that must be added to bluesource address to get to next
                                   pixel in row */
    LONG redmod;                /* number that must be added to redsource address to get to next
                                   row */
    LONG greenmod;              /* number that must be added to greensource address to get to next
                                   row */
    LONG bluemod;               /* number that must be added to bluesource address to get to next
                                   row */
    UWORD width;                /* image width if image is passed through memory */
    UWORD height;               /* image height if image is passed through memory */
  };



/****  öffentliche Funktionen  ****/

void HandleUserMessage(struct Display *Display, void *UserMessage);

SHORT PrintProject(struct Box *Project, BOOL Async);
SHORT PrintBrush(struct Display *Display, struct brush *mybrush);
void PrintTP(struct Box *box);
void PrintBrushTP(struct Display *Display, struct brush *mybrush);
