OUTDIR=bin
PINBIN=bin
LINK=ilink
LFLAGS=/NOFREE /NOLOGO
CC_FLAGS=/Q /Iinclude /Ss /C

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

PPD_OBJS=obj\ppdenc.obj \
	 obj\listfls.obj

all: $(OUTDIR)\pin.exe $(OUTDIR)\ppdenc.exe



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

$(PINBIN)\ppdenc.def: makefile
	@echo Writing new  @<<$(PINBIN)\ppdenc.def
NAME PIN WINDOWCOMPAT
EXETYPE OS2
PROTMODE

DESCRIPTION
  'PPD ENCODING CONVERTER TOOL'

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

$(OUTDIR)\ppdenc.exe: makefile $(PPD_OBJS) $(PINBIN)\ppdenc.def
	$(LINK) $(LFLAGS) $(LDFLAGS) @<<$(PINBIN)\ppdenc.lnk
       	$(PPD_OBJS:.obj=.obj+
       	)
       	$(PINBIN)\ppdenc.exe
       	$(PINBIN)\ppdenc.map
       	
       	$(PINBIN)\ppdenc.def;
<<keep

{src}.c{obj}.obj:
	@echo generating $(@)
	@icc $(CC_FLAGS) /Fo$(@) src\$(@B).c
