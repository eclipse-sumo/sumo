package ns2;

import java.io.FileNotFoundException;
import java.io.PrintWriter;
import java.util.HashMap;
import java.util.List;

/**
 * class for writing activity file
 * @author Thimor Bohn <bohn@itm.uni-luebeck.de>
 *
 */
public class ActivityWriter {
	/**
	 * method for writing activity file
	 * @param activity name of activity file
	 * @param vehicles stored vehicles to be written
	 * @param vehicleIds map holds vehicle ids
	 */
	public static void write(String activity, List<Vehicle> vehicles, HashMap<String, Integer> vehicleIds) {
        PrintWriter out;
		try {
			out = new PrintWriter(activity);
	        for (Vehicle vehicle: vehicles) {
	            out.println("$ns_ at " + vehicle.time_first + " \"$g(" + vehicleIds.get(vehicle.id) + ") start\"");
	            out.println("$ns_ at " + vehicle.time_last + " \"$g(" + vehicleIds.get(vehicle.id) + ") stop\"");
	        }
	        out.println();
	        out.close();
		} catch (FileNotFoundException e) {
			System.err.println(e);
		}
	}
}
