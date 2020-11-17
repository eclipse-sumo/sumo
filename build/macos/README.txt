These application bundles are necessary wrappers for the SUMO binaries in order to have a native feel on macOS. They have been built with macOS Automator. 
In order to rebuild these bundles, the icon files and the launcher scripts are supplied in the build subfolder. The following steps are necessary to create the bundle:

1) Create the launcher application
   - Start Automator app 
   - Create new application
   - Look for an action named "Execute shell script"
   - Select "/bin/zsh" as shell and provide input "as arguments"
   - Add the content of the starter zsh script into the content field
   - Save as an application with the name "sumo-gui" (or netedit or OSM Web Wizard)

2) Change the application launcher icon
   - Select the new application and hit [apple+i] to open info pane
   - Drag and drop the icns-file on top of the automator icon
   
The launcher scripts try to find the SUMO_HOME variable setting by sourcing several configuration files (.profile, .bash_profile, .zshrc). If the script was unable to find a SUMO_HOME declaration a warning is issued. You may extend the list of configuration files, if you defined the SUMO_HOME in a different file.
