@echo off
for %%I in (*.lib) do echo %%I>>all.txt
for %%I in (*mt-gd-1_55.lib) do echo %%I>>adebug.txt
for %%I in (*mt-sgd-1_55.lib) do echo %%I>>adebug.txt
for %%I in (*vc120-gd-1_55.lib) do echo %%I>>adebug.txt
for %%I in (*vc120-sgd-1_55.lib) do echo %%I>>adebug.txt
for %%I in (*mt-1_55.lib) do echo %%I>>arelease.txt
for %%I in (*mt-s-1_55.lib) do echo %%I>>arelease.txt
for %%I in (*vc120-1_55.lib) do echo %%I>>arelease.txt
for %%I in (*vc120-s-1_55.lib) do echo %%I>>arelease.txt
pause