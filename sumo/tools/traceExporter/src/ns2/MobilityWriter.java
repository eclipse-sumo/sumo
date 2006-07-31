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
	public static void write(String mobility,  String trace, List<Edge> edges, List<Vehicle> vehicles, HashMap<String, Integer> vehicleIds) {
		try {
			PrintWriter out = new PrintWriter(mobility);
			
			// write initial positions
	        for (Vehicle vehicle: vehicles) {
	            out.print("$node_(" + vehicleIds.get(vehicle.id) + ") ");
	            out.println("set X_ " + vehicle.x);
	            out.print("$node_(" + vehicleIds.get(vehicle.id) + ") ");
	            out.println("set Y_ " + vehicle.y + " ");
	            out.print("$node_(" + vehicleIds.get(vehicle.id) + ") ");
	            out.println("set Z_ 0.0 ");
	        }
	        System.out.println("start: read trace file - stage 2");
	        // write following positions
	        TraceReader.readSecond(out, trace, vehicles, vehicleIds, edges);
	        System.out.println("finished: read trace file - stage 2");
		} catch (IOException e) {
			System.err.println(e);
		}
	}
}
