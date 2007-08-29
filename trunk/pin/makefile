OUTDIR=bin
PINBIN=bin
LINK=ilink
LFLAGS=/NOFREE

PIN_OBJS=obj\afm2pak.obj \
         obj\config.obj \
         obj\conv_afm.obj \
         obj\conv_ppd.obj \
         obj\driverea.obj \
         obj\listdevn.obj \
         obj\listfls.obj \
         obj\listreqf.obj \
         obj\pinmain.obj \
         obj\ppd2pak.obj

all: $(OUTDIR)\pin.exe



$(PINBIN)\pin.def: makefile
        @echo Writing new  @<<$(PINBIN)\pin.def
NAME PIN WINDOWCOMPAT
EXETYPE OS2
PROTMODE

DESCRIPTION
  'POSTSCRIPT IMPORT NEW RESOURCE UTILITY'

CODE PRELOAD
STACKSIZE       32768
<<KEEP

# THE EXE
$(OUTDIR)\pin.exe: makefile $(PIN_OBJS) $(PINBIN)\pin.def
       $(LINK) $(LFLAGS) $(LDFLAGS) @<<$(PINBIN)\pin.lnk
       $(PIN_OBJS:.obj=.obj+
       )
       $(PINBIN)\pin.exe
       $(PINBIN)\pin.map

       $(PINBIN)\pin.def;
<<keep

