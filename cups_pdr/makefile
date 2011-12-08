# CUPS.PDR Makefile for IBM C Compiler/NMAKE32
#

BASE=CUPS
DEF_PORTNAME=CUPS

ICCOPTS=-Gd-e- -Sp1 -Ss+ -Ms -C+ -Fo$@ -Dcdecl= -D_cdecl= -Rn -O -Wpro -Q+ -Yp+ -Ss+
LINKOPTS=/NOFREEFORMAT /MAP /A:16 /PM:PM

!IFDEF DEBUG
ICCOPTS=-Gd-e- -Sp1 -Ss+ -Ms -C+ -Fo$@ -Dcdecl= -D_cdecl= -Rn -O -Wpro -Q+ -Yp+ -Ti
LINKOPTS=$(LINKOPTS) /DEBUG /CO
!ENDIF

$(BASE).PDR:                    $(BASE).DLL
        copy $(BASE).DLL $(BASE).PDR
        del $(BASE).DLL

$(BASE).DLL:                    $(BASE).OBJ \
                                $(BASE).RES \
                                $(BASE).DEF
        @makedesc -Rcups.def -N"netlabs.org" -V"^#define=DRIVER_VERSION,cups.h" cups.def
        ILINK $(LINKOPTS) @<<
$(BASE)
$(BASE).DLL
$(BASE)
OS2386
$(BASE)
<<
        RC $(BASE).RES $(BASE).DLL
        ea2 -e DEFAULT_PORT=$(DEF_PORTNAME) $(BASE).DLL

$(BASE).RES:                    $(BASE).RC \
                                $(BASE).H
        RC -r $(BASE).RC $(BASE).RES

$(BASE).OBJ:                    $(BASE).C \
                                $(BASE).H
        ICC $(ICCOPTS) $(BASE).C

clean:
        if exist $(BASE).PDR del $(BASE).PDR
        if exist $(BASE).DLL del $(BASE).DLL
        if exist $(BASE).SYM del $(BASE).SYM
        if exist $(BASE).MAP del $(BASE).MAP
        if exist $(BASE).RES del $(BASE).RES
        if exist $(BASE).OBJ del $(BASE).OBJ
