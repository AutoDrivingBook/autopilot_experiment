@echo off
for %%I in (*.lib) do echo %%I>>all.txt
for %%I in (*7.0.lib) do echo %%I>>arelease.txt
for %%I in (*c++.lib) do echo %%I>>arelease.txt
for %%I in (*-gd.lib) do echo %%I>>adebug.txt
pause