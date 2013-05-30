/*   
    Copyright (C) 2013 Mario Krumnow, Evamarie Wieﬂner, Dresden University of Technology

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

package de.tudresden.sumo.cmd;
import de.tudresden.sumo.config.Constants;
import de.tudresden.sumo.util.SumoCommand;

public class Junction {

	//getter methods

	/**
	 * Returns a list of IDs of all junctions within the scenario.
	 */

	public static SumoCommand getIDList(){
		return new SumoCommand(Constants.CMD_GET_JUNCTION_VARIABLE, Constants.ID_LIST, "", Constants.RESPONSE_GET_JUNCTION_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * Returns the position of the named junction.
	 */

	public static SumoCommand getPosition(String junctionID){
		return new SumoCommand(Constants.CMD_GET_JUNCTION_VARIABLE, Constants.VAR_POSITION, junctionID, Constants.RESPONSE_GET_JUNCTION_VARIABLE, Constants.POSITION_2D);
	}

	//setter methods


}