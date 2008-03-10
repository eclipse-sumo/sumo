package ns2;

import java.io.FileNotFoundException;
import java.io.PrintWriter;
import java.util.Map;
import java.util.List;

/**
 * class for writing ns2 activity files
 * activity files enable vehicles at first occurence and
 * disable vehicles at last occurence
 * @author Thimor Bohn <bohn@itm.uni-luebeck.de>
 *
 */
public class ActivityWriter {
	/**
	 * working method
	 * @param activity name of ns2 activity file
	 * @param wantedVehicle list of vehicles to be selected for ns2
	 * @param vehicleNewId list of vehicle ids for ns2
	 * @param vehicleFirstOcc map: vehicle id -> first occurence of vehicle in sumo
	 * @param vehicleLastOcc map: vehicle id -> last occurence of vehicle in sumo
	 * @param begin sumo time at which ns2 should start to simulate
	 */
	public static void write(
			String activity,
			List<String> wantedVehicle,
			List<String> vehicleNewId,
			Map<String, Double> vehicleFirstOcc, 
			Map<String, Double> vehicleLastOcc,
			double begin,
			double penetration,
			boolean hasPenetration) {
		try {
			PrintWriter out = new PrintWriter(activity);
	        for (String id: wantedVehicle) {
	        	String newId =  vehicleNewId.get(wantedVehicle.indexOf(id));
				double minP =  Double.parseDouble(newId)/wantedVehicle.size();
				if (hasPenetration) {
					if (penetration > minP) {
					    out.println("$ns_ at " + (vehicleFirstOcc.get(id)-begin) + " \"$g(" + newId + ") start\"" + "\t# SUMO-ID: " + id);
					    out.println("$ns_ at " + (vehicleLastOcc.get(id)-begin) + " \"$g(" + newId + ") stop\"" + "\t# SUMO-ID: " + id);
					}
				} else {
					out.println("if { $opt(penetration) > " + minP  + " } { ");
					out.println("  $ns_ at " + (vehicleFirstOcc.get(id)-begin) + " \"$g(" + newId + ") start\"" + "\t# SUMO-ID: " + id);
					out.println("  $ns_ at " + (vehicleLastOcc.get(id)-begin) + " \"$g(" + newId + ") stop\"" + "\t# SUMO-ID: " + id);
					out.println("}");
				}
	        }
	        out.flush();
	        out.close();
		} catch (FileNotFoundException ex) {
			System.err.println(ex);
		}
	}
}
