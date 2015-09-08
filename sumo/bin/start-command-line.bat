@echo off
set current_dir=%CD%

rem change the following line to customize the location of your sumo installation
set sumo_home=%CD%\..

rem change the following line to customize the location of your python installation
set python_dir=C:\Python27

rem change the following line to set a new working dir:
set default_dir=%sumo_home%\..

cmd /K "set PATH=%PATH%;%sumo_home%\bin;%python_dir%;%sumo_home%\tools;&& set SUMO_HOME=%sumo_home%& cd /d %default_dir% && echo info: added location of sumo, tools and python to the search path && echo info: variables SUMO_HOME is set to %SUMO_HOME% && echo. && echo use the 'cd /d' command to change directory && echo example usage: && echo cd /d c:\foo\bar 
