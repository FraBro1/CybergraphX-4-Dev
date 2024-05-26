
ASM = BAsm

GST    = GST=GST:StdInc.gst
SCOPTS = STRMERGE UNSCHAR NOSTKCHK OPTIMIZE

OBJ = LibStuff.o PBMBase.o ReadPBM.o FFR.o
LIB = LIB:amiga.lib

pbm.datatype: $(OBJ)
	slink FROM $(OBJ) LIB $(LIB) SC SD ND NOICONS TO $@
	Protect $@ RWD
	Copy $@ TO SYS:Classes/DataTypes CLONE
	Avail >NIL: FLUSH
	LoadResident $@

LibStuff.o: LibStuff.asm
	$(ASM) -iINCLUDE: -o$@ $*.asm

PBMBase.o: PBMBase.c PBM.h
	sc PBMBase.c $(GST) $(SCOPTS) OBJNAME=$@

ReadPBM.o: ReadPBM.c PBM.h
	sc ReadPBM.c $(GST) $(SCOPTS) UTILLIB OBJNAME=$@

FFR.o: FFR.c PBM.h
	sc FFR.c $(GST) $(SCOPTS) UTILLIB OBJNAME=$@
