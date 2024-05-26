;
;	ILBM Datatype
;
;	(c) Copyright 1995-1997 by Matthias Scheler
;	    All Rights Reserved
;

		IFD BARFLY
		 CODE
		 SMALLDATA a4
		ELSE
		 CSECT text,0
		ENDC

		NOLIST
		INCLUDE "exec/types.i"
		INCLUDE "exec/libraries.i"
		INCLUDE "exec/resident.i"
		INCLUDE "exec/initializers.i"
		INCLUDE "exec/semaphores.i"
		INCLUDE	"dos/dos.i"
		LIST

; Macros

XLIB:		MACRO
		XREF _LVO\1
		ENDM

CALL:		MACRO
		jsr _LVO\1(a6)
		ENDM

; External References

		XREF	_LinkerDB
		XREF	_SysBase

		XREF	_InitPBMBase
		XREF	_ReleasePBMBase

		XLIB	Remove
		XLIB	FreeMem
		XLIB	CloseLibrary
		XLIB	OpenLibrary
		XLIB	InitSemaphore
		XLIB	ObtainSemaphore
		XLIB	ReleaseSemaphore

;	our Library Structure

		STRUCTURE PBM_DataType,LIB_SIZE
		BPTR	PB_SegList
		APTR	PB_Class
		STRUCT	PB_Semaphore,SS_SIZE
		LABEL	PB_SIZE

;	Program Code

		moveq	#-1,d0
		rts

;	Library Resident Module Tag

VERSION		EQU	43	; PBM_ID !
REVISION	EQU	10	; PBM_ID !

LibResTag:	dc.w	RTC_MATCHWORD
		dc.l	LibResTag
		dc.l	LibResTag+RT_SIZE
		dc.b	RTF_AUTOINIT
		dc.b	VERSION
		dc.b	NT_LIBRARY
		dc.b	0
		dc.l	PBM_Name,PBM_ID
		dc.l	LibInit

PBM_Name:	dc.b	"pbm.datatype",0
PBM_ID:		dc.b	"PBM datatype 43.10 "
		dc.b	"(c) 1995-1997 by Matthias Scheler",0
		ds.w	0

;	Initialization Table for Library Structure

LibInit:	dc.l	PB_SIZE
		dc.l	FuncTab
		dc.l	DataTab
		dc.l	Init

FuncTab:	dc.l	Open
		dc.l	Close
		dc.l	Expunge
		dc.l	ReturnNull

		dc.l	ObtainPBMMachine

		dc.l	-1

DataTab:	INITBYTE LN_TYPE,NT_LIBRARY
                INITLONG LN_NAME,PBM_Name
                INITBYTE LIB_FLAGS,LIBF_SUMUSED+LIBF_CHANGED
                INITWORD LIB_VERSION,VERSION
                INITWORD LIB_REVISION,REVISION
                INITLONG LIB_IDSTRING,PBM_ID
                dc.l	0

; Init Function

Init:		movem.l	a4/a5,-(sp)
		lea	_LinkerDB,a4

		move.l	d0,a5
		move.l	a0,PB_SegList(a5)
		move.l	a6,_SysBase(a4)

		lea	PB_Semaphore(a5),a0
		CALL	InitSemaphore

		move.l	a5,d0
		movem.l	(sp)+,a4/a5
		rts

; Open Function

Open:		movem.l	d2/a4-a6,-(sp)
		lea	_LinkerDB,a4
		move.l	a6,d2
		move.l	a6,a5
		move.l	_SysBase(a4),a6

		bclr	#LIBB_DELEXP,LIB_FLAGS(a5)
		addq.w	#1,LIB_OPENCNT(a5)

		lea	PB_Semaphore(a5),a0
		CALL	ObtainSemaphore

		tst.l	PB_Class(a5)
		bne.s	GotClass

		move.l	d2,-(sp)
		bsr	_InitPBMBase
		addq.w	#4,sp
		move.l	d0,d2
		bne.s	GotClass

		subq.w	#1,LIB_OPENCNT(a5)

GotClass:	lea	PB_Semaphore(a5),a0
		CALL	ReleaseSemaphore

		move.l	d2,d0
		movem.l	(sp)+,d2/a4-a6
		rts

; Close Function

Close:		subq.w	#1,LIB_OPENCNT(a6)
		bne.s	ReturnNull

		btst	#LIBB_DELEXP,LIB_FLAGS(a6)
		bne.s	TimeToGo

ReturnNull:	moveq	#0,d0
		rts

; Expunge Function

Expunge:        tst.w	LIB_OPENCNT(a6)
                beq.s	TimeToGo

                bset	#LIBB_DELEXP,LIB_FLAGS(a6)
                bra.s	ReturnNull

TimeToGo:	movem.l d2/a4-a6,-(sp)
		lea	_LinkerDB,a4

		tst.l	PB_Class(a6)
		beq.s	ClassNotBusy

		move.l	a6,-(sp)
		bsr	_ReleasePBMBase
		addq.w	#4,sp

		tst.l	d0
		bne.s	ClassNotBusy
		movem.l	(sp)+,d2/a4-a6
		rts

ClassNotBusy:	move.l	a6,a5
		move.l	PB_SegList(a5),d2

		move.l	_SysBase(a4),a6
		move.l	a5,a1
		CALL	Remove

		move.l	a5,a1
		moveq	#0,d0
		move.w	LIB_NEGSIZE(a5),d0
		sub.w	d0,a1
		add.w	LIB_POSSIZE(a5),d0
		CALL	FreeMem

		move.l	d2,d0
		movem.l	(sp)+,d2/a4-a6
		rts

; Our own function

ObtainPBMMachine:
		move.l PB_Class(a6),d0
		rts

		END
