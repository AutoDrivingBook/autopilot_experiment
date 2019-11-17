@echo off
for %%I in (*.lib) do echo %%I>>all.txt¡¡¡¡
for %%I in (*debug.lib) do echo %%I>>adebug.txt
for %%I in (*release.lib) do echo %%I>>arelease.txt
pause