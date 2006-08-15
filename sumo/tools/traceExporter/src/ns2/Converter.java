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
			new Converter(param.net, param.trace, param.activity, param.mobility, param.penetration);
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
		System.out.println("-p [penetration factor] with penetration factor in [0,1]");
	}
	
	/**
	 * constructor
	 * @param net name of net file
	 * @param trace name of trace file
	 * @param activity name of activity file
	 * @param mobility name of mobility file
	 */
	private Converter(String net, String trace, String activity, String mobility, double penetration) {
		List<Edge>               edges            = new LinkedList<Edge>();
		List<Vehicle>            vehicles         = new LinkedList<Vehicle>();
		List<Vehicle>            equippedVehicles = new LinkedList<Vehicle>();
		HashMap<String, Integer> vehicleIds       = new HashMap<String, Integer>();
		
		System.out.println("start: read netfile");
		NetReader.read(net, edges);
		System.out.println("finished: read netfile");
		System.out.println("start: write mobiliy file");
		MobilityWriter.write(mobility, trace, edges, vehicles, vehicleIds, equippedVehicles, penetration);
		System.out.println("finished: write mobiliy file");
		System.out.println("#edges = " +edges.size());
		System.out.println("#vehicles = " + vehicles.size());
		System.out.println("#vehicles equipped = " + equippedVehicles.size());
		System.out.println("start: write activity file");
		ActivityWriter.write(activity, vehicles, vehicleIds, equippedVehicles);
		System.out.println("finished: write activity file");
	}
}
