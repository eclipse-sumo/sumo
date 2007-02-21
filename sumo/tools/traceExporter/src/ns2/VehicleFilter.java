package ns2;

import java.util.List;
import java.util.Map;

/**
 * class for selecting vehicles
 * vehicles are selected if they are used between
 * begin and end
 * @author Thimor Bohn <bohn@itm.uni-luebeck.de>
 *
 */
public class VehicleFilter {
	/**
	 * working method
	 * @param vehicleId list of unique ids of vehicles in sumo simulation 
	 * @param wantedVehicle list of filtered vehicles ids
	 * @param vehicleFirstOcc map: vehicle id -> first occurence of vehicle in sumo
	 * @param vehicleLastOcc map: vehicle id -> last occurence of vehicle in sumo
	 * @param begin sumo time at which ns2 should start to simulate
	 * @param end sumo time at which ns2 should stop to simulate
	 */
	public static void filter(
			List<String> vehicleId, 
			List<String> wantedVehicle,
			Map<String, Double> vehicleFirstOcc, 
			Map<String, Double> vehicleLastOcc,
			double begin,
			double end) {
		for (String id: vehicleId) {
			// vehicle too late
			if (vehicleFirstOcc.get(id) > end) {
				wantedVehicle.remove(id); // id is an object
			}
			// begin == end means end is end of sumo simulation, see Parser.java
			if (begin != end) {
				// vehicle too early
				if (vehicleLastOcc.get(id) < begin) {
					wantedVehicle.remove(id); // id is an object
				}
			} else {
				// update stop time of ns2 simulation
				end = Math.max(end, vehicleLastOcc.get(id));
			}
		}
		// change occurence times for use in ns2
		for (String id: wantedVehicle) {
			if (vehicleFirstOcc.get(id) < begin) {
				vehicleFirstOcc.put(id, begin);
			}
			if (vehicleLastOcc.get(id) > end) {
				vehicleLastOcc.put(id, end);
			}
		}
	}
			
}
