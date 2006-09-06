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
	 * @param penetration value of penetration in [0,1]
	 */
	public static void write(String mobility,  String trace, List<Edge> edges, List<Vehicle> vehicles, HashMap<String, Integer> vehicleIds, HashMap<String, Integer> partialVehicleIds, List<Vehicle> equippedVehicles, double penetration) {
		try {
			// write none-initial positions
			PrintWriter out = new PrintWriter(mobility);
	        TraceReader.read(out, trace, vehicles, vehicleIds, partialVehicleIds, edges, equippedVehicles, penetration);
	        out.flush();

			// write initial positions
	        for (Vehicle vehicle: vehicles) {
	        	if (equippedVehicles.contains(vehicle)) {
		            out.print("$node_(" + partialVehicleIds.get(vehicle.id) + ") ");
		            out.println("set X_ " + vehicle.getInitialX());
		            out.print("$node_(" + partialVehicleIds.get(vehicle.id) + ") ");
		            out.println("set Y_ " + vehicle.getInitialY() + " ");
		            out.print("$node_(" + partialVehicleIds.get(vehicle.id) + ") ");
		            out.println("set Z_ 0.0 ");
	        	}
	        }
	        out.flush();
		} catch (IOException e) {
			System.err.println(e);
		}
	}
}
