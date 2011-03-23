#
# Start
#

#
# Project Name
#
PRJNAME=Clip

#
# Configurations
#
CFG1=ReleaseDS
CFG2=ReleaseWMME
CFG3=DebugDS
CFG4=DebugWMME

#
# If no configuration specified, default to the first
#
!IF "$(CFG)" == ""
CFG=$(CFG1)
!MESSAGE No configuration specified. Defaulting to $(CFG)
!ENDIF

!IF "$(CFG)" == "$(CFG1)"
IS_DEBUG=No
USE_DS=Yes
!ELSEIF "$(CFG)" == "$(CFG2)"
IS_DEBUG=No
USE_DS=No
!ELSEIF "$(CFG)" == "$(CFG3)"
IS_DEBUG=Yes
USE_DS=Yes
!ELSEIF "$(CFG)" == "$(CFG4)"
IS_DEBUG=Yes
USE_DS=No
!ENDIF

#
# Check for invalid configuration
#
!IF "$(CFG)" != "$(CFG1)" && "$(CFG)" != "$(CFG2)" && "$(CFG)" != "$(CFG3)" && "$(CFG)" != "$(CFG4)" 
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "$(PRJNAME).mak" CFG="$(CFG1)"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "$(CFG1)"
!MESSAGE "$(CFG2)"
!MESSAGE "$(CFG3)"
!MESSAGE "$(CFG4)"
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

#
# Fix
#
!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF

#
# Common for all configurations
#
OUTDIR=.\EXE
INTDIR=.\OBJ
PALIBDIR=..\LIB
CPP=cl.exe
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\$(PRJNAME).bsc"
BSC32_SBRS = \
LINK32=link.exe
CPP_PROJ_COMMON = \
	/nologo /W3 /I "../../pa_common" \
	/D "WIN32" /D "_CONSOLE" /D "_MBCS" \
	/Fp"$(INTDIR)\$(PRJNAME).pch" \
	/YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c		
DEF_LIBS=winmm.lib dsound.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
LINK32_COMMON=										\
		/nologo /subsystem:console /machine:I386	\
		/pdb:"$(OUTDIR)\$(PRJNAME).pdb"				\
		/out:"$(OUTDIR)/$(PRJNAME).exe"				

#
# First target seems to be the default
#
ALL : MAKEALL

#
# Input and object files
#
SOURCE=..\..\pa_tests\patest_clip.c
LINK32_OBJS= \
	"$(INTDIR)\patest_clip.obj"

"$(INTDIR)\patest_clip.obj" : MAKEOBJ

CLEAN :
!IF "$(IS_DEBUG)" == "Yes"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\$(PRJNAME).pdb"
	-@erase "$(OUTDIR)\$(PRJNAME).ilk"
!ENDIF
	-@erase "$(INTDIR)\patest_clip.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\$(PRJNAME).pch"
	-@erase "$(OUTDIR)\$(PRJNAME).exe"

#
# Settings based on IS_DEBUG and USE_DS
#
!IF "$(IS_DEBUG)" == "Yes"

CPP_PROJ_CFG=/MLd /Gm /GX /ZI /Od /D "_DEBUG" /GZ
LINK32_FLAGS=/incremental:yes /debug /pdbtype:sept

!IF "$(USE_DS)" == "Yes"
PALIB=$(PALIBDIR)/PAStaticDSD.lib 
!ELSE
PALIB=$(PALIBDIR)/PAStaticWMMED.lib
!ENDIF

!ELSE

CPP_PROJ_CFG=/ML /GX /O2 /D "NDEBUG"
LINK32_FLAGS=/incremental:no

!IF "$(USE_DS)" == "Yes"
PALIB=$(PALIBDIR)/PAStaticDS.lib 
!ELSE
PALIB=$(PALIBDIR)/PAStaticWMME.lib
!ENDIF

!ENDIF

#
#
#
CPP_PROJ=$(CPP_PROJ_COMMON) $(CPP_PROJ_CFG)
LINK32_FLAGS=$(PALIB) $(DEF_LIBS) $(LINK32_COMMON) $(LINK32_CFG)
LINK32=link.exe

#
# Targets
#
"$(OUTDIR)" :
	if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"
"$(INTDIR)" :
	if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

MAKEALL : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
	$(LINK32) @<<
	$(LINK32_FLAGS) $(LINK32_OBJS)
<<

MAKEOBJ : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)

#
#
#
.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

#
# Dependencies
#
!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("$(PRJNAME).dep")
!INCLUDE "$(PRJNAME).dep"
!ELSE 
!MESSAGE Warning: cannot find "$(PRJNAME).dep"
!ENDIF 
!ENDIF 
