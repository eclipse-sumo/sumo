package ns2;

import java.io.IOException;
import java.io.PrintWriter;
import java.util.HashMap;
import java.util.List;

/**
 * class for writing mobility file
 * @author Thimor Bohn <bohn@itm.uni-luebeck.de>
 *
 */
public class MobilityWriter {
	/**
	 * method for writing mobilty file
	 * @param mobility name of mobility file
	 * @param trace name of trace file
	 * @param edges holds net
	 * @param vehicles stored vehicles to be written
	 * @param vehicleIds holds vehicle ids
	 */
	public static void write(String mobility,  String trace, List<Edge> edges, List<Vehicle> vehicles, HashMap<String, Integer> vehicleIds, List<Vehicle> equippedVehicles, double penetration) {
		try {
			PrintWriter out = new PrintWriter(mobility);
			
			// write initial positions
	        for (Vehicle vehicle: vehicles) {
	        	if (equippedVehicles.contains(vehicle)) {
		            out.print("$node_(" + vehicleIds.get(vehicle.id) + ") ");
		            out.println("set X_ " + vehicle.x);
		            out.print("$node_(" + vehicleIds.get(vehicle.id) + ") ");
		            out.println("set Y_ " + vehicle.y + " ");
		            out.print("$node_(" + vehicleIds.get(vehicle.id) + ") ");
		            out.println("set Z_ 0.0 ");
	        	}
	        }
	        // write following positions
	        TraceReader.read(out, trace, vehicles, vehicleIds, edges, equippedVehicles, penetration);
		} catch (IOException e) {
			System.err.println(e);
		}
	}
}
