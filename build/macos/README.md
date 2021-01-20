These application bundles are necessary wrappers for the SUMO binaries in order to have a native feel on macOS. They have been built with macOS Automator. 

To use the launchers:
   - Just move the launchers to /Applications
   - Make sure to have SUMO_HOME defined in .zshrc or .profile or .bash_profile

In order to rebuild these bundles, the icon files and the launcher scripts are supplied in the build subfolder. The following steps are necessary to create the bundle:

1) Create the launcher application
   - Start Automator app 
   - Create new application
   - Look for an action named "Execute shell script"
   - Select "/bin/zsh" as shell and provide input "as arguments"
   - Add the content of the starter zsh script into the content field
   - Save as an application with the name "sumo-gui" (or netedit or OSM Web Wizard)

2) Change the application launcher icon
   - Right-click on the launcher app 
   - Click on "view package contents"
   - Add .icns file to Contents/Resources folder and replace existing AutomatorApplet.icns file
 
The launcher scripts try to find the SUMO_HOME variable setting by sourcing several configuration files (.profile, .bash_profile, .zshrc). If the script was unable to find a SUMO_HOME declaration a warning is issued. You may extend the list of configuration files, if you defined the SUMO_HOME in a different file.

---
(Optional) Create a Disk Image (dmg):

1) First steps
   - Open Disk Utility
   - Select File -> New Image -> Blank Image
   - Save as "SUMO_temp.dmg"
   - Under Name, put "Eclipse SUMO". Leave everything else as is (Size, Format, Encryption, etc.) Image Format should be "read/write disk image"
   - Click on Save

On the Desktop, double click to open the newly mounted disk "Eclipse SUMO". A small Finder window showing the blank partition should open.

2) Add the background image
   - Copy the "background.png" file to the mounted disk
   - Right click on an empty spot on the Finder window and select "Show View Options"
   - In the Background section select "Picture" and drag and drop the recently copied image. Resize the Finder window, to display properly the green background
   - On the same View Options window, make sure to select the "Always open in icon view" option at the top
   - Open the Terminal and navigate to "/Volumes/Eclipse\ SUMO". Rename the background image (add a dot at the beginning) in order to hide it on the disk image -> "mv background.png .background.png"
   - Close the Terminal

3) Add the actual content
   - Copy the .app files (sumo-gui.app, netedit.app and OSM Web Wizard.app) to the mounted disk
   - Create an Alias for the /Applications folder (Command + Option + drag the Applications icon into the mounted disk). Make sure the name of the Alias is "Applications"
   - Copy the "ReadMe.rtf" file too (NOT this file!)
   - On the "Show View Options" window, adjust the "Icon size" and "Grid spacing" properties
   - Move around the icons so that they are well placed on the disk (.app icons above the green arrow, Applications alias below the arrow, ReadMe to the side)

4) Final steps
   - Eject the "Eclipse SUMO" disk (drag it to the recycle bin)
   - Double click on the "SUMO_temp.dmg" file
   - In the Disk Utility app, select "Eclipse SUMO" from the Disk Images sidebar
   - Select File -> New Image -> Image from "Eclipse SUMO"
   - Save As: "SUMO_launchers" with the following options:
      - Format: compressed
      - Encryption: none

Done! Unmount and delete the "SUMO_temp.dmg" disk. The final dmg file should be "SUMO_launchers.dmg".
