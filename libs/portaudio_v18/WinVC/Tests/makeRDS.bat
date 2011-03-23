@echo off
SET CFG=ReleaseDS
nmake /f clip.mak
nmake /f devs.mak
nmake /f devs2.mak
nmake /f dither.mak
nmake /f errs.mak
nmake /f Fuzz.mak
nmake /f minlat.mak
nmake /f pink.mak
nmake /f record.mak
nmake /f sine.mak
nmake /f wire.mak
SET CFG=
