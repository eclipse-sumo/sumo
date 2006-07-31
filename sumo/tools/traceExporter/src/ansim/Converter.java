package ansim;

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
			new Converter(param.net, param.trace, param.out);
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
		System.out.println("-o [outfile]");
	}
	
	/**
	 * constructor
	 * @param net name of net file
	 * @param trace name of trace file
	 * @param out name of ansim out file
	 */
	private Converter(String net, String trace, String out) {
		List<Edge>               edges      = new LinkedList<Edge>();
		List<Vehicle>            vehicles   = new LinkedList<Vehicle>();
		HashMap<String, Integer> vehicleIds = new HashMap<String, Integer>();
		
		System.out.println("start: read netfile");
		NetReader.read(net, edges);
		System.out.println("finished: read netfile");
		System.out.println("#edges = " +edges.size());
		System.out.println("start: read trace file - stage 1");
		TraceReader.readFirst(trace, vehicles, vehicleIds, edges);
		System.out.println("finished: read trace file - stage 1");
		System.out.println("#vehicles = " + vehicles.size());
		System.out.println("start: write ansim trace file");
		AnsimWriter.write(out, trace, vehicles, vehicleIds, edges);
		System.out.println("finished: write ansim trace file");
	}
}
