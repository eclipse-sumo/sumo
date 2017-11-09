/*   
    Copyright (C) 2017 Mario Krumnow, Dresden University of Technology

    This file is part of TraaS.

    TraaS is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    TraaS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with TraaS.  If not, see <http://www.gnu.org/licenses/>.
*/

package de.tudresden.ws.container;

/**
 * 
 * @author Mario Krumnow
 * @author Anja Liebscher
 *
 */

public enum SumoVehicleSignalState implements SumoObject {
	
	VEH_SIGNAL_BLINKER_RIGHT (0),
	VEH_SIGNAL_BLINKER_LEFT (1),
	VEH_SIGNAL_BLINKER_EMERGENCY (2),
	VEH_SIGNAL_BRAKELIGHT(3),
	VEH_SIGNAL_FRONTLIGHT(4),
	VEH_SIGNAL_FOGLIGHT(5),
	VEH_SIGNAL_HIGHBEAM(6),
	VEH_SIGNAL_BACKDRIVE(7),
	VEH_SIGNAL_WIPER(8),
	VEH_SIGNAL_DOOR_OPEN_LEFT(9),
	VEH_SIGNAL_DOOR_OPEN_RIGHT(10),
	VEH_SIGNAL_EMERGENCY_BLUE(11),
	VEH_SIGNAL_EMERGENCY_RED(12),
	VEH_SIGNAL_EMERGENCY_YELLOW(13); 
	
	private final int pos;
	
	SumoVehicleSignalState(int pos){this.pos = pos;}

	public int getPos(){return this.pos;}
	
}
