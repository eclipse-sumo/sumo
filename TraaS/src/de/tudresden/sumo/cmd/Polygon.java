/*   
    Copyright (C) 2013 Mario Krumnow, Dresden University of Technology

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
import de.tudresden.ws.container.SumoColor;
import de.tudresden.ws.container.SumoGeometry;
import de.tudresden.ws.container.SumoStringList;

public class Polygon {

	//getter methods

	/**
	 * Returns the subscription results for the last time step and the given poi.
	 */

	public static SumoCommand getColor(String polygonID){
		return new SumoCommand(Constants.CMD_GET_POLYGON_VARIABLE, Constants.VAR_COLOR, polygonID, Constants.RESPONSE_GET_POLYGON_VARIABLE, Constants.TYPE_COLOR);
	}

	/**
	 * Returns a list of all polygons in the network.
	 */

	public static SumoCommand getIDList(){
		return new SumoCommand(Constants.CMD_GET_POLYGON_VARIABLE, Constants.ID_LIST, "", Constants.RESPONSE_GET_POLYGON_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * getShape
	 */

	public static SumoCommand getShape(String polygonID){
		return new SumoCommand(Constants.CMD_GET_POLYGON_VARIABLE, Constants.VAR_SHAPE, polygonID, Constants.RESPONSE_GET_POLYGON_VARIABLE, Constants.TYPE_POLYGON);
	}

	/**
	 * getType
	 */

	public static SumoCommand getType(String polygonID){
		return new SumoCommand(Constants.CMD_GET_POLYGON_VARIABLE, Constants.VAR_TYPE, polygonID, Constants.RESPONSE_GET_POLYGON_VARIABLE, Constants.TYPE_STRING);
	}

	//setter methods

	/**
	 * add
	 */

	public static SumoCommand add(String polygonID, SumoGeometry shape, SumoColor color, boolean fill, String polygonType, int layer){

		Object[] array = new Object[]{shape, color, fill, polygonType, layer};
		return new SumoCommand(Constants.CMD_SET_POLYGON_VARIABLE, Constants.ADD, polygonID, array);
	}

	/**
	 * remove
	 */

	public static SumoCommand remove(String polygonID, int layer){

		return new SumoCommand(Constants.CMD_SET_POLYGON_VARIABLE, Constants.CMD_SET_POLYGON_VARIABLE, polygonID, layer);
	}

	/**
	 * setColor
	 */

	public static SumoCommand setColor(String polygonID, SumoColor color){

		return new SumoCommand(Constants.CMD_SET_POLYGON_VARIABLE, Constants.TYPE_COLOR, polygonID, color);
	}

	/**
	 * setShape
	 */

	public static SumoCommand setShape(String polygonID, SumoStringList shape){

		return new SumoCommand(Constants.CMD_SET_POLYGON_VARIABLE, Constants.CMD_SET_POLYGON_VARIABLE, polygonID, shape);
	}

	/**
	 * setType
	 */

	public static SumoCommand setType(String polygonID, String polygonType){

		return new SumoCommand(Constants.CMD_SET_POLYGON_VARIABLE, Constants.CMD_SET_POLYGON_VARIABLE, polygonID, polygonType);
	}


}