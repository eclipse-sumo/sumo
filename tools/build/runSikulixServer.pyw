#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runSikulixServer.pyw
# @author  Pablo Alvarez Lopez
# @date    2016-08-31
# @version $Id$

import gtk
import os
import subprocess
import sys
import socket
import platform

# Define global variables for Server
TRAY_TOOLTIP = 'Sikulix server'
TRAY_ICON = os.environ.get('SUMO_HOME', '.') + "/tests/sikulixicon.png"

# Open message dialog to User
def messageDialog(data=None):
    msg = gtk.MessageDialog(None, gtk.DIALOG_MODAL, gtk.MESSAGE_INFO, gtk.BUTTONS_OK, data)
    msg.run()
    msg.destroy()


# Get status of sikulixServer
def checkStatus():
    # Create socket
    statusSocket = socket.socket()
    # Try to connect to localhost trought Port 50001
    try:
        # Connect status socket
        statusSocket.connect(("localhost", 50001))
        # Send get status code
        statusSocket.send("GET / HTTP/1.1\n\n")
        # Receive status of SikulixServer
        statusReceived = statusSocket.recv(1024)
        # Close status socket
        statusSocket.close()
        # If status of server contains "200 OK", return true. False in other case
        if "200 OK" in statusReceived:
            return True
        else:
            return False
    except:
        # Cannot connect to SikulixServer, then return false
        return False


# start Sikulix Server
def startSikulixServer():
    # Call a subprocess of this Python Script to run Sikulix Server depending of operating system
    subprocess.Popen([os.environ.get('RUN_SIKULIX', 'runsikulix'), "-s"], 
                     env=os.environ, stdout=None, stderr=None, shell=(platform.system() != 'Linux'))
    #return status of sikulixServer
    return checkStatus()


# close Sikulix server
def closeSikulixServer():
    # Create socket
    stopSocket = socket.socket()
    # Try to connect to localhost trought Port 50001
    try:
        # Connect status socket
        stopSocket.connect(("localhost", 50001))
        # send stop code to sikulixServer
        stopSocket.send("GET /STOP HTTP/1.1\n\n")
        # Receive stop information of SikulixServer
        statusStop = stopSocket.recv(1024)
        # Close stop socket
        stopSocket.close()
        # If status of server contains "stopping server", return true. False in other case
        if "stopping server" in statusStop:
            return True
        else:
            return False
    except:
        # Cannot connect to SikulixServer, then return false
        return False


# Show status of sikulix server
def showStatus(data=None):
    # Show a message dialog with the status of server
    if checkStatus():
        messageDialog("Sikulix server is running")
    else:
        messageDialog("Sikulix server is stopped")


# Close SikulixServer
def closeApp(data=None):
    # Show message dialog if there are problems closing sikulix server
    if closeSikulixServer() == False:
        messageDialog("Error stopping Sikulix Server")
    # Quit GTK
    gtk.main_quit()


# Make menu
def makeMenu(event_button, event_time, data=None):
    # Create gtk menu
    menu = gtk.Menu()
    # Create menu items
    checkStatusItem = gtk.MenuItem("Check Status")
    closeServerItem = gtk.MenuItem("Close Sikulix server")
    #Append the menu items  
    menu.append(checkStatusItem)
    menu.append(closeServerItem)
    #add callbacks
    checkStatusItem.connect_object("activate", showStatus, "Check status")
    closeServerItem.connect_object("activate", closeApp, "Close Sikulix Server")
    #Show the menu items
    checkStatusItem.show()
    closeServerItem.show()
    #Popup the menu
    menu.popup(None, None, None, event_button, event_time)


# Called when user do a right click over status icon
def onRightClickOverTrayIcon(data, event_button, event_time):
    makeMenu(event_button, event_time)


def buildTrayIcon():
    # Set Tray icon
    sikulixIcon = gtk.status_icon_new_from_file(TRAY_ICON)
    # Connect popup menu with right click over tray icon
    sikulixIcon.connect('popup-menu', onRightClickOverTrayIcon)
    # start gtk
    gtk.main()


# Main function of RunSikulixServer
if __name__ == '__main__':
    # Check if sikulixServer is already opened
    if checkStatus() == False :
        # Start Sikulix Server
        startSikulixServer()
        buildTrayIcon()
