package ns2;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

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
			new Converter(param.net, param.trace, param.activity, param.mobility, param.config, param.begin, param.end, param.penetration, param.seed, param.hasPenetration);
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
		System.out.println("-n <sumo net.xml file>");
		System.out.println("-t <sumo netstate-dump file>");
		System.out.println("-a <output activity file>");
		System.out.println("-m <output mobility file>");
		System.out.println("-c <output config file]");
		System.out.println("-b <converting begin time>");
		System.out.println("-e <converting end time>");
		System.out.println("[ -p <penetration rate (0..1)> ] ");
		System.out.println("[ -s <seed for random numbers (used to determine fraction of equipped cars)> ] ");
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
	private Converter(String net, String trace, String activity, String mobility, String config, double begin, double end, double penetration, long seed, boolean hasPenetration) {
		// 1. get net
		List<Edge> edges = new LinkedList<Edge>();
		Map<String, Junction> junctions = new HashMap<String, Junction>();
		NetReader.read(net, edges, junctions);
		
		// 2. translate net
		NetTranslater.translate(edges, junctions);
		
		// 3. get all vehicles (IDs, first occurence, last occurence)
		List<String> vehicleId = new LinkedList<String>();
		HashMap<String, Double> vehicleFirstOcc = new HashMap<String, Double>();
		HashMap<String, Double> vehicleLastOcc  = new HashMap<String, Double>();
		VehicleReader.read(trace, vehicleId, vehicleFirstOcc, vehicleLastOcc);
		
		// 4. filter vehicles (intersection: [first occurence, last occurence], [begin time, end time]
		List<String> wantedVehicle = new LinkedList<String>(vehicleId);
		VehicleFilter.filter(vehicleId, wantedVehicle, vehicleFirstOcc, vehicleLastOcc, begin, end);
		
		// 5. Randomize new id
		List<String> vehicleNewId = new LinkedList<String>();
		IdRandomizer.randomize(wantedVehicle, vehicleNewId, seed);
		
		// 6. write mobility file (contains every movement of all (wanted) vehicles
		MobilityWriter.write(trace, mobility, wantedVehicle, vehicleNewId, edges, begin, end, penetration, hasPenetration);
		
		// 7. write activity file (contains first and last occurence of all (wanted -> filtered) vehicles
		ActivityWriter.write(activity, wantedVehicle, vehicleNewId, vehicleFirstOcc, vehicleLastOcc, begin, penetration, hasPenetration);

		// 8. write config file (contains statical information about simulation)
		ConfigWriter.write(config, activity, mobility, edges, wantedVehicle, vehicleFirstOcc, vehicleLastOcc, begin, penetration, hasPenetration);
	}
}
