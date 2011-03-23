You need to set the VC environment variables before you can use these
batch files.
This can be done by running the vcvars32.bat which should be in your 
Visual C++ 'bin' directory.

To make one test project (i.e. Fuzz):
-------------------------------------
nmake /f Fuzz.mak [CFG=ReleaseDS]
nmake /f Fuzz.mak CFG=ReleaseWMME
nmake /f Fuzz.mak CFG=DebugDS
nmake /f Fuzz.mak CFG=DebugWMME

Batch files:
------------
Clear.bat		Deletes files created with make* batch files
makeDDS.bat		Build all examples, Debug, DirectSound
makeDWMME.bat	Build all examples, Debug, WMME
makeRDS.bat		->>- Release, DS
makeRWMME.bat	->>- Release, WMME

