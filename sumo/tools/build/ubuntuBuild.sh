#!/bin/bash
#
# This script can be used for automatic installation of SUMO on Ubuntu distributions.
# Currently, Ubuntu 8.04 - 10.04 are supported.
#
# If you don't install the supplemental packages (GDAL, etc.) via packet manager, you
# may have to adjust the configuration parameters below

# Specifies the absolute path to the SUMO sources
SOURCEPATH=`pwd`

# Specifies the path where the binaries are to be installed
INSTALL_PATH="--prefix=/opt/sumo/"

# Additional configuration parameters (The paths' to GDAL, etc. are specified further below
ADDITIONAL_CONFIG_PARAMS="$INSTALL_PATH"

# Description of SUMO which will be used as description of the package if you decide to use "checkinstall" instead of "make install"
DESCRIPTION="\"Simulation of Urban MObility\" (SUMO) is an open source, highly portable, microscopic road traffic simulation package designed to handle large road networks.
It is mainly developed by employees of the Institute of Transportation Systems at the German Aerospace Center.
SUMO is licensed under the GPL.
The project homepage can be found at http://sumo.sourceforge.net/"


#-----------------------------------------------------------------------------------------------------------------------------------

# Queries the user for the packet manager to be used to install supplemental packages
function read_packet_manager(){

	if [ "$PACKET_MANAGER" != "apt-get" ] && [ "$PACKET_MANAGER" != "apt-get" ];
	then
		while [ "$PACKMAN" != "1" ] && [ "$PACKMAN" != "2" ];
		do
			echo "Which packet manager front-end do you use? (1) apt-get, (2) aptitude?";
			read PACKMAN;
		done

		if [ "$PACKMAN" == "1" ]; then
			PACKET_MANAGER="apt-get";
		else
			PACKET_MANAGER="aptitude";
		fi

		sudo $PACKET_MANAGER update;
	fi
}


#------------------------------------------------------------------------------------------------------------------------------------
#
# Fetch the SUMO sources from the repository's trunk or locate previously fetched ones
#
#
echo "You can choose to fetch the latest SUMO version from the SUMO repository or to install SUMO from sources located in your local file system."
echo "Do you want to fetch the latest version from the repository?"
while [ "$FETCH" != "y" ] && [ "$FETCH" != "n" ]; do
	echo "[y,n]"
	read FETCH
done

echo "Please specify the path to the source code's destination."
echo "If the correct path equals (\""$SOURCEPATH"\"), just press [Enter]."
read SRCPATH;
if [ -n "$SRCPATH" ]; then
	SOURCEPATH="$SRCPATH"
fi

if [ "$FETCH" == "y" ];
then
	# prepare the directory where the sources will be placed
	if [ -d "$SOURCEPATH/sumo" ];
	then 
		echo "The content of the directory \"$SOURCEPATH/sumo\" will be completely destroyed."
		echo "Press [ENTER] to continue."
		read xyz
		rm -rf "$SOURCEPATH/sumo"
	fi
	mkdir -p "$SOURCEPATH"
	cd "$SOURCEPATH"

	# install subversion if it is not installed, yet	
	if [ -z "$(dpkg -l | grep subversion)" ];
	then
		read_packet_manager
		sudo $PACKET_MANAGER install subversion;
	fi
	echo "Start svn checkout..."
	svn co https://sumo.svn.sourceforge.net/svnroot/sumo/trunk/sumo
	SOURCEPATH="$SOURCEPATH/sumo"
	cd "$SOURCEPATH"
	make -f Makefile.cvs
fi




#------------------------------------------------------------------------------------------------------------------------------------
#
# Installation of required packages
#
#
echo "Sumo depends on some packages packages which are available in the Ubuntu sources or on the projects' sites as source code."
echo " You can choose to install them from the sources or to compile them on your own."

while [ "$PM" != "y" ] && [ "$PM" != "N" ]; do
	echo "Do you want to install them via packet manager? [y,N]"
	read PM
done

if [ "$PM" == "y" ];
then

	while [ "$GUI" != "y" ] && [ "$GUI" != "N" ]; do
		echo "Do you want to build the SUMO GUI? [y,N]";
		read GUI;
	done

	read_packet_manager

	if [ -n "$(lsb_release -d | grep 10.04)" ] || [ -n "$(lsb_release -d | grep 9.10)" ] || [ -n "$(lsb_release -d | grep 9.04)" ] || [ -n "$(lsb_release -d | grep 8.10)" ];
	then

		if [ -n "$(lsb_release -d | grep 10.04)" ] ;
		then
			GDAL="/usr/lib/libgdal1.6.0.so"
			FOX="1.6"
		else
			GDAL="/usr/lib/libgdal1.5.0.so"
			if [ -n "$(lsb_release -d | grep 9.10)" ] ;
			then
				FOX="1.6"
			else
				FOX="1.4"
			fi
		fi

		PKGLIST="g++ autoconf libtool libgdal1-dev proj libxerces-c2-dev"

		if [ "$GUI" == "y" ];
		then
			PKGLIST="$PKGLIST libfox-$FOX-dev libgl1-mesa-dev libglu1-mesa-dev"
		fi

		sudo $PACKET_MANAGER install $PKGLIST
		
		if [ -z "$(ls -l /usr/lib/libgdal.so)" ]
		then
			if [ -n "$(ls -l $GDAL)" ];
						  
			then
				echo "creating symbolic link:  /usr/lib/libgdal.so --> $GDAL"
				sudo ln -s  "$GDAL" /usr/lib/libgdal.so
			else
				echo "/usr/lib/libgdal.so was not found. You have to create a symbolic link to the version which is currently installed!"
			fi
		fi
	else
		if [ -n "$(lsb_release -d | grep 8.04)" ];
		then

			FOX="1.4"
			PKGLIST="g++ autoconf libtool libgdal1-dev proj libxerces27-dev"

        	        if [ "$GUI" == "y" ];
               		then
                        	PKGLIST="$PKGLIST libfox$FOX-dev libgl1-mesa-dev libglu1-mesa-dev"
	                fi

			sudo $PACKET_MANAGER install "$PKGLIST"

                	if [ -z "$(ls -l /usr/lib/libgdal.so)" ]
	                then
        	                if [ -n "$(ls -l /usr/lib/libgdal1.4.0.so.1)" ];
                	        then
	                                echo "creating symbolic link:  /usr/lib/libgdal.so --> /usr/lib/libgdal1.4.0.so.1"
        	                        sudo ln -s  /usr/lib/libgdal1.4.0.so.1 /usr/lib/libgdal.so
                	        else
	                                echo "/usr/lib/libgdal.so was not found. You have to create it as symbolic link to the version which is currently installed!"
        	                fi  
                	fi 
		
		fi
	fi
fi


#------------------------------------------------------------------------------------------------------------------------------------
#
# Configuration
#
#
if [ "$PM" != "y" ]; then
	echo "You did not install the required packages via packet manager."
	echo "If you did not install them previously and adjusted the configuration parameters,"
	echo "the installation process will almost certainly fail!"
fi
echo "Press [Enter] to start the compilation and installation process"
read

cd $SOURCEPATH

# generate configure script
make -f Makefile.cvs

# configure the path's to the libraries
# If you installed one of the libraries manually, you may have to adjust the paths'
./configure	--with-fox-includes=/usr/include/fox-$FOX \
		--with-gdal-includes=/usr/include/gdal \
		--with-proj-libraries=/usr \
		--with-gdal-libraries=/usr \
		--with-proj-gdal \
		$ADDITIONAL_CONFIG_PARAMS|| exit


#------------------------------------------------------------------------------------------------------------------------------------
#
# Compilation and installation
#
#

# run a 'make clean' first to get rid of intermediate files which my be left from an aborted compilation process
make clean || exit
make || exit

echo "The SUMO binaries were build but not installed, yet."
echo "However, the installation process is optional since SUMO can be can be run from the source folder by calling \"src/sumo\" and \"src/sumo-gui\", respectively."
echo "The installation process will copy the binaries to the previously configured path and you may delete the source and intermediate files afterwards." 
echo "Do you want to install SUMO now? (Note that the source and intermediate files will not be deleted automatically by this script!)"
while [ "$INSTALLSUMO" != "y" ] && [ "$INSTALLSUMO" != "N" ]; do
        echo "[y,N]"
        read INSTALLSUMO
done

if [ "$INSTALLSUMO" == "y" ];
then

	echo "When using \"checkinstall \" instead of \"make install\" SUMO will be installed as *.deb packet via packet manager."
	while [ "$CHECKINSTALL" != "y" ] && [ "$CHECKINSTALL" != "N" ]; do
        	echo "Do you want to use \"checkinstall\" to install SUMO? [y,N]"
	        read CHECKINSTALL
	done

	if [ "$CHECKINSTALL" == "y" ];
	then

		# check if checkinstall is already installed
		if [ -z "$(dpkg -l | grep checkinstall)" ];
		then
			read_packet_manager
			sudo $PACKET_MANAGER install checkinstall
		fi

		# auto-configure packet name, etc.
		cd $SOURCEPATH

		# if information about a subversion revision is available - use them
		if [ -e ./src/version.h ]; then
			VERSION=`grep VERSION_STRING src/version.h | cut -d " " -f 3`
		else
			VERSION=`grep VERSION_STRING src/config.h | cut -d " " -f 3`
		fi
		echo $DESCRIPTION > description-pak
		sudo checkinstall --pkgname "sumo" --bk --pkgversion $VERSION || exit
		echo "The packet was created successfully."
	else
		make install || exit
	fi
fi



# generate documentation
echo "Do you want to create the documentation via doxygen?"

while [ "$DOXY" != "y" ] && [ "$DOXY" != "N" ]; do
	echo "[y,N]"
	read DOXY
done

if [ "$DOXY" == "y" ]; then
	if [ -z "$(dpkg -l | grep doxygen)" ]; then
		read_packet_manager
		sudo $PACKET_MANAGER install doxygen
	fi
	cd $SOURCEPATH
	doxygen sumo.doxyconf
fi
