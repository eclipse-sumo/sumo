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

package de.tudresden.sumo.cmd;
import de.tudresden.sumo.config.Constants;
import de.tudresden.sumo.util.SumoCommand;

/**
 * 
 * @author Mario Krumnow
 * @author Evamarie Wiessner
 *
 */

public class Junction {

	//getter methods

	/**
	 * Returns a list of IDs of all junctions within the scenario.
	 * 
	 * @return list of IDs of all junctions in the network
	 */

	public static SumoCommand getIDList(){
		return new SumoCommand(Constants.CMD_GET_JUNCTION_VARIABLE, Constants.ID_LIST, "", Constants.RESPONSE_GET_JUNCTION_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * Returns the number of all junctions in the network.
	 * @return the number of junctions
	 */

	public static SumoCommand getIDCount(){
		return new SumoCommand(Constants.CMD_GET_JUNCTION_VARIABLE, Constants.ID_COUNT, "", Constants.RESPONSE_GET_JUNCTION_VARIABLE, Constants.TYPE_INTEGER);
	}
	
	/**
	 * Returns the position of the named junction.
	 * 
	 * @param junctionID
	 *            a string identifying the junction
	 * @return the coordinates of the center of the junction
	 */
	 
	public static SumoCommand getPosition(String junctionID){
		return new SumoCommand(Constants.CMD_GET_JUNCTION_VARIABLE, Constants.VAR_POSITION, junctionID, Constants.RESPONSE_GET_JUNCTION_VARIABLE, Constants.POSITION_2D);
	}
	
	/**
	* getShape
 	*@param junctionID
	* a string identifying the junction
	* @return the shape
	*/

	public static SumoCommand getShape(String junctionID){
		return new SumoCommand(Constants.CMD_GET_JUNCTION_VARIABLE, Constants.VAR_SHAPE, junctionID, Constants.RESPONSE_GET_JUNCTION_VARIABLE, Constants.TYPE_POLYGON);
	}


}