@echo off
rem change the following line to set a new default dir:
set default_dir=%windir%\..

cmd /K "set PATH=%PATH%;%CD% && cd /d %default_dir% && echo info: added location of sumo executables to the search path && echo. && echo use the 'cd /d' command to change directory && echo example usage: && echo cd /d c:\foo\bar 
