
/*
**  This C include file was generated automatically
**  from an assembly include file
**  using I2H written by Henning Friedl.
*/



/* --------------------------------------------------------------------------- */
#define RETINAEMU_LIB_VERSION	(1)	/* the current version of retina.library */
/* --------------------------------------------------------------------------- */
/*  tags for RE_OpenDeepScreen(): */

#define RESA_Depth	(TAG_USER+1)	/* Retina screen depth: must be 16 or 24 */
#define RESA_RetinaScreenPtr	(TAG_USER+2)	/* ti_Data points to a ULONG in which */
/*   RE_OpenDeepScreen() will stick the */
/*   pointer to the Retina screen. */
#define RESA_Obsolete	(TAG_USER+3)	/* do not use */
#define RESA_SmartMenu	(TAG_USER+4)	/* BOOL, enable/disable smart menu refresh */
/*   DEFAULT: TRUE */
/* --------------------------------------------------------------------------- */
/*  the public-part of retinaemu.library */

struct RetinaEmuBase {
	struct	Library Library;

	ULONG	Reserved;

};

/*  subsequent fields in the library base are RETINA PRIVATE */
/* --------------------------------------------------------------------------- */
/*  You may use this macro to call the functions in retinaemu.library. To avoid */
/*  conflicts with existing functions, they have a slightly unusual name. */

/* --------------------------------------------------------------------------- */
