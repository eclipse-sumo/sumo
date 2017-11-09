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
import de.tudresden.ws.container.SumoStringList;

/**
 * 
 * @author Mario Krumnow
 * @author Evamarie Wiessner
 *
 */

public class Route {

	//getter methods

	/**
	 * Returns the IDs of the edges this route covers.
	 * 
	 * @param routeID
	 *            a string identifying the route
	 * @return a list of IDs of the edges
	 */

	public static SumoCommand getEdges(String routeID){
		return new SumoCommand(Constants.CMD_GET_ROUTE_VARIABLE, Constants.VAR_EDGES, routeID, Constants.RESPONSE_GET_ROUTE_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * Returns a list of IDs of all currently loaded routes.
	 * 
	 * @return a list of ID's of all routes
	 */

	public static SumoCommand getIDList(){
		return new SumoCommand(Constants.CMD_GET_ROUTE_VARIABLE, Constants.ID_LIST, "", Constants.RESPONSE_GET_ROUTE_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * Returns the number of all Routes in the network.
	 * @return the number of routes in the network
	 */

	public static SumoCommand getIDCount(){
		return new SumoCommand(Constants.CMD_GET_ROUTE_VARIABLE, Constants.ID_COUNT, "", Constants.RESPONSE_GET_ROUTE_VARIABLE, Constants.TYPE_INTEGER);
	}
	
	/**
	 * Returns the chosen parameter
	 *
	 *  @param routeID a string identifying the route
	 *  @param param a string identifying the parameter
	 *  
	 * @return the specific parameter
	 */

	public static SumoCommand getParameter(String routeID, String param){
		return new SumoCommand(Constants.CMD_GET_POI_VARIABLE, Constants.VAR_PARAMETER, routeID, Constants.RESPONSE_GET_POI_VARIABLE, Constants.TYPE_STRING);
	}
	
	//setter methods

	/**
	 * Add a new route.
	 * 
	 * @param routeID
	 *            a string identifying the route
	 * @param edges
	 *            list of edges the new route is following
	 *  @return SumoCommand 
	 */

	public static SumoCommand add(String routeID, SumoStringList edges){
		return new SumoCommand(Constants.CMD_SET_ROUTE_VARIABLE, Constants.ADD, routeID, edges);
	}


}