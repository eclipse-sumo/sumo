/*   
    Copyright (C) 2013 Mario Krumnow, Evamarie Wießner, Dresden University of Technology

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

public class Gui {

	//getter methods

	/**
	 * Returns the coordinates of the lower left and the upper right corner of the currently visible view.
	 */

	public static SumoCommand getBoundary(String viewID){
		return new SumoCommand(Constants.CMD_GET_GUI_VARIABLE, Constants.VAR_VIEW_BOUNDARY, viewID, Constants.RESPONSE_GET_GUI_VARIABLE, Constants.TYPE_BOUNDINGBOX);
	}

	/**
	 * Returns the list of available views (open windows).
	 */

	public static SumoCommand getIDList(){
		return new SumoCommand(Constants.CMD_GET_GUI_VARIABLE, Constants.ID_LIST, "", Constants.RESPONSE_GET_GUI_VARIABLE, Constants.TYPE_STRINGLIST);
	}

	/**
	 * Returns the x and y offset of the center of the current view.
	 */

	public static SumoCommand getOffset(String viewID){
		return new SumoCommand(Constants.CMD_GET_GUI_VARIABLE, Constants.VAR_VIEW_OFFSET, viewID, Constants.RESPONSE_GET_GUI_VARIABLE, Constants.POSITION_2D);
	}

	/**
	 * Returns the name of the current coloring scheme.
	 */

	public static SumoCommand getSchema(String viewID){
		return new SumoCommand(Constants.CMD_GET_GUI_VARIABLE, Constants.VAR_VIEW_SCHEMA, viewID, Constants.RESPONSE_GET_GUI_VARIABLE, Constants.TYPE_STRING);
	}

	/**
	 * Returns the current zoom factor.
	 */

	public static SumoCommand getZoom(String viewID){
		return new SumoCommand(Constants.CMD_GET_GUI_VARIABLE, Constants.VAR_VIEW_ZOOM, viewID, Constants.RESPONSE_GET_GUI_VARIABLE, Constants.TYPE_DOUBLE);
	}

	//setter methods

	/**
	 * Save a screenshot for the given view to the given filename.
	 */

	public static SumoCommand screenshot(String viewID, String filename){

		return new SumoCommand(Constants.CMD_SET_GUI_VARIABLE, Constants.VAR_SCREENSHOT, viewID, filename);
	}

	/**
	 * Set the current boundary for the given view (see getBoundary()).
	 */

	public static SumoCommand setBoundary(String viewID, double xmin, double ymin, double xmax, double ymax){

		Object[] array = new Object[]{xmin, ymin, xmax, ymax};
		return new SumoCommand(Constants.CMD_SET_GUI_VARIABLE, Constants.VAR_VIEW_BOUNDARY, viewID, array);
	}

	/**
	 * Set the current offset for the given view.
	 */

	public static SumoCommand setOffset(String viewID, double x, double y){

		Object[] array = new Object[]{x, y};
		return new SumoCommand(Constants.CMD_SET_GUI_VARIABLE, Constants.VAR_VIEW_OFFSET, viewID, array);
	}

	/**
	 * Set the current coloring scheme for the given view.
	 */

	public static SumoCommand setSchema(String viewID, String schemeName){

		return new SumoCommand(Constants.CMD_SET_GUI_VARIABLE, Constants.VAR_VIEW_SCHEMA, viewID, schemeName);
	}

	/**
	 * Set the current zoom factor for the given view.
	 */

	public static SumoCommand setZoom(String viewID, double zoom){

		return new SumoCommand(Constants.CMD_SET_GUI_VARIABLE, Constants.VAR_VIEW_ZOOM, viewID, zoom);
	}

	/**
	 * Start visually tracking the given vehicle on the given view.
	 */

	public static SumoCommand trackVehicle(String viewID, String vehID){

		return new SumoCommand(Constants.CMD_SET_GUI_VARIABLE, Constants.VAR_TRACK_VEHICLE, viewID, vehID);
	}


}