!include config.in


LIBS        = E:\ddk\base\lib
COMPILER    = E:\IBMCPP
DDKTOOLS    = E:\os2tk45\bin
ROOT            = $(MAKEDIR)

O       = $(ROOT)\obj
B       = $(ROOT)\bin
S       = $(ROOT)\src


OBJS =  $(O)\ecups.obj\
        $(O)\echelp.obj\
        $(O)\ecset.obj\
        $(O)\ecui.obj \
        $(O)\ecuidata.obj \
        $(O)\callcups.obj \
        $(O)\_call_filedlg.obj



LINK = $(COMPILER)\bin\ilink /nofree
RC=$(DDKTOOLS)\rc.exe
MAPSYM=$(DDKTOOLS)\mapsym.exe

.SUFFIXES: .c .obj .dll .idl .h .ih .rc .res .i .ptr .ico .bmp


P      = 437
RFLAGS     = -n -cp $(P) -r -x -i $(ROOT)
RF         = -n -x -p

!ifndef DEBUG
DBG_CFLAGS=
DBG_LFLAGS= /NOE
!    else
DBG_CFLAGS= /Tm+ /Ti /DDEBUG
DBG_LFLAGS=  /DE /NOE
!endif

ICCFLAGS=-Ss -Gd-e- -Ss+ -Ms -C+ -Fo$@ -Dcdecl= -D_cdecl= -Rn -Wcnd- -Wpro -Q+ -Yp+ -Iinclude
UIICCFLAGS=-Ss -Gd-e- -Ss+ -Ms -C+ -Fo$@ -Dcdecl= -D_cdecl= -Rn -Wcnd- -Wpro -Q+ -Yp+ -Iinclude
CFLAGS =  $(DBG_CFLAGS) $(ICCFLAGS)
UICFLAGS =  $(DBG_CFLAGS) $(UIICCFLAGS)
LFLAGS = /noi /map /nol /exepack:2 /packcode /packdata /align:2 /segments:300 $(DBG_LFLAGS)


{$(S)}.c{$(O)}.obj:
        @Echo -
        @Echo --------------------------------------------------------
        @Echo - Generating $(@)
        @Echo --------------------------------------------------------
        $(CC)  $(CFLAGS) $(S)\$(@B).c



.asm.obj:
        @Echo -
        @Echo --------------------------------------------------------
        @Echo - Generating $(@)
        @Echo --------------------------------------------------------
         $(AS) $(AFLAGS) $(AINC) $(@B).asm ;

.c.i:
        @Echo -
        @Echo --------------------------------------------------------
        @Echo - Generating $(@)
        @Echo --------------------------------------------------------
         $(CC) $(CFLAGS) -P  $(@B).c

.rc.obj:
        @Echo -
        @Echo --------------------------------------------------------
        @Echo - Generating $(@)
        @Echo --------------------------------------------------------
         $(RC) $(RFLAGS) $(@B).rc



#------------------------------------------------------------------------
#Component defines start here.
#------------------------------------------------------------------------

!ifdef DEBUG
SerialLibs  =   pmprintf.lib \
                helpers.lib \
                os2386.lib\
                cppom30.lib
!else
SerialLibs  =   helpers.lib \
                os2386.lib\
                cppom30.lib
!endif




#--------------------------------------------------------------------------
# The all target is used to specify a list of targets that need to be built
# Please don't delete "dep" from the target list.  It is used to generate the
# depend.mak file.
# Please look at the example for more details.
#-------------------------------------------------------------------------

all: display_bld_info\
     create_paths\
     $(ROOT)\bin\ecups.pdr \

display_bld_info:
        @Echo -
        @Echo --------------------------------------------------------
        @Echo - Building $(PRODUCT)
        @Echo --------------------------------------------------------

#----------------------------------------------------------------------------
# Create Tree Structure
#----------------------------------------------------------------------------
# Use the following to create a directory structure that may not exist
#
# A path can be any pathname valid for a filename {without the filename and
# no trailing "\"}.
#
# makepath is an utility to create all subdirectories in the path.
#
# Use the createpaths target to make sure paths are created before
# other dependencies
#--------------------------------------------------------------------------

create_paths:
        @Echo --------------------------------------------------------
        @Echo - Creating paths
        @Echo --------------------------------------------------------
        @if not exist obj md obj >nul 2>nul
        @if not exist bin md bin >nul 2>nul



#---------------------------------------------------------------------
#  ECUPS.DEF
#---------------------------------------------------------------------
$(O)\ecups.def: makefile
        @Echo --------------------------------------------------------
        @Echo - Generating $(O)\ecups.def
        @Echo --------------------------------------------------------

         @Rem Create DEF file <<$(O)\ecups.def
LIBRARY ECUPS INITINSTANCE TERMINSTANCE
Description '@#Bart van Leeuwen:1.0.5#@ eCUPS Port driver'
PROTMODE
DATA MULTIPLE NONSHARED LOADONCALL
CODE LOADONCALL
EXPORTS
   SplPdEnumPort
   SplPdInstallPort
   SplPdGetPortIcon
   SplPdQueryPort
   SplPdSetPort
   SplPdRemovePort
   SplPdWrite
   SplPdOpen
   SplPdClose
   SplPdAbortDoc
   SplPdNewPage
   SplPdQuery
   SplPdSet

   SPLPDENUMPORT      = SplPdEnumPort
   SPLPDINSTALLPORT   = SplPdInstallPort
   SPLPDGETPORTICON   = SplPdGetPortIcon
   SPLPDQUERYPORT     = SplPdQueryPort
   SPLPDSETPORT       = SplPdSetPort
   SPLPDREMOVEPORT    = SplPdRemovePort
   SPLPDWRITE         = SplPdWrite
   SPLPDCLOSE         = SplPdClose
   SPLPDOPEN          = SplPdOpen
   SPLPDABORTDOC      = SplPdAbortDoc
   SPLPDNEWPAGE       = SplPdNewPage
   SPLPDQUERY         = SplPdQuery
   SPLPDSET           = SplPdSet
<<keep


#---------------------------------------------------------------------
# ECUPS.O or ECUPS.RES
#---------------------------------------------------------------------
$(O)\ecups.res: $(ROOT)\mri\ecups.rc\
                          $(ROOT)\mri\ecups.dlg\
                          $(ROOT)\mri\ecups.ico
        @Echo --------------------------------------------------------
        @Echo - Compiling Resource  $(ROOT)\obj\ecups.res
        @Echo --------------------------------------------------------
        @cd $(ROOT)\mri
        @$(RC) $(RFLAGS) ecups.rc $(O)\ecups.res
        @cd $(ROOT)



#---------------------------------------------------------------------
# ECUPS.PDR
#---------------------------------------------------------------------
$(ROOT)\bin\ecups.pdr:    $(OBJS)\
                          $(O)\ecups.def\
                          $(O)\ecups.res
        @Echo --------------------------------------------------------
        @Echo - Linking $(@B).pdr
        @Echo --------------------------------------------------------

        @$(LINK) $(LFLAGS)  @<<$(O)\$(@B).lnk
$(LFLAGS) +
$(OBJS: =+^
)
$(B)\ecups.dll
$(B)\$(@B).map
$(SerialLibs: =+^
)
$(O)\$(@B).def
<<keep
        copy $(ROOT)\bin\ecups.dll $@
        del  $(ROOT)\bin\ecups.dll
        $(RC) $(RF) $(O)\ecups.res $(B)\$(@B).pdr
        copy $(ROOT)\bin\$(@B).pdr $(@B).pdr
        $(MAPSYM) -n $(B)\$(@B).map


clean:
       @rm -r obj\*
