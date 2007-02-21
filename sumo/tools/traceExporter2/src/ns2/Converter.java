package ns2;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;

/**
 * central class of ns2-converter
 * @author Thimor Bohn <bohn@itm.uni-luebeck.de>
 *
 */
public class Converter {
	/**
	 * object creation 
	 * @param args command line arguments
	 */
	public static void convert(String[] args) {
		// parse parameters
		Parameter param = null;
		try {
			param = Parser.parse(args);
		} catch (IllegalArgumentException e) {
			System.err.println(e);
			help();
		}
		// call constructor
		if (param != null) {
			new Converter(param.net, param.trace, param.activity, param.mobility, param.config, param.begin, param.end);
		} else {
			System.err.println("param == null");
		}
	}
	
	/**
	 * some error occured
	 * print parameter options
	 *
	 */
	private static void help() {
		System.out.println("needed options:");
		System.out.println("-n [netfile]");
		System.out.println("-t [tracefile]");
		System.out.println("-a [activityfile]");
		System.out.println("-m [mobilityfile]");
		System.out.println("-c [configfile]");
		System.out.println("-b [begin]");
		System.out.println("-e [end]");
	}
	
	/**
	 * constructor
	 * @param net name of sumo net file
	 * @param trace name of sumo trace file
	 * @param activity name of ns2 activity file
	 * @param mobility name of ns2 mobility file
	 * @param config name of ns2 config file
	 * @param begin sumo time at which ns2 should start to simulate 
	 * @param end sumo time at which ns2 should stop to simulate
	 */
	private Converter(String net, String trace, String activity, String mobility, String config, double begin, double end) {
		// 1. get net
		List<Edge> edges = new LinkedList<Edge>();
		NetReader.read(net, edges);
		
		// 2. get all vehicles (IDs, first occurence, last occurence)
		List<String> vehicleId = new LinkedList<String>();
		HashMap<String, Double> vehicleFirstOcc = new HashMap<String, Double>();
		HashMap<String, Double> vehicleLastOcc  = new HashMap<String, Double>();
		VehicleReader.read(trace, vehicleId, vehicleFirstOcc, vehicleLastOcc);
		
		// 3. filter vehicles (intersection: [first occurence, last occurence], [begin time, end time]
		List<String> wantedVehicle = new LinkedList<String>(vehicleId);
		VehicleFilter.filter(vehicleId, wantedVehicle, vehicleFirstOcc, vehicleLastOcc, begin, end);
		
		// 4. Randomize new id
		List<String> vehicleNewId = new LinkedList<String>();
		IdRandomizer.randomize(wantedVehicle, vehicleNewId);
		
		// 5. write mobility file (contains every movement of all (wanted) vehicles
		MobilityWriter.write(trace, mobility, wantedVehicle, vehicleNewId, edges, begin, end);
		
		// 6. write activity file (contains first and last occurence of all (wanted -> filtered) vehicles
		ActivityWriter.write(activity, wantedVehicle, vehicleNewId, vehicleFirstOcc, vehicleLastOcc, begin);

		// 7. write config file (contains statical information about simulation)
		ConfigWriter.write(config, activity, mobility, edges, wantedVehicle, vehicleFirstOcc, vehicleLastOcc, begin);
	}
}
