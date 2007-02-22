package ns2;

import java.io.FileNotFoundException;
import java.io.PrintWriter;
import java.util.List;
import java.util.Map;

/**
 * class for writing ns2 config files
 * config files contain statical information about the ns2 simulation
 * @author Thimor Bohn <bohn@itm.uni-luebeck.de>
 *
 */
public class ConfigWriter {
	/**
	 * working method
	 * @param config name of ns2 config file
	 * @param activity name of ns2 activity file
	 * @param mobility name of ns2 mobility file
	 * @param edges list of edges
	 * @param wantedVehicle list of vehicles to be selected for ns2
	 * @param vehicleFirstOcc map: vehicle id -> first occurence of vehicle in sumo
	 * @param vehicleLastOcc map: vehicle id -> last occurence of vehicle in sumo
	 * @param begin sumo time at which ns2 should start to simulate
	 */
	public static void write(
			String config,
			String activity,
			String mobility,
			List<Edge> edges,
			List<String> wantedVehicle,
			Map<String, Double> vehicleFirstOcc, 
			Map<String, Double> vehicleLastOcc,
			double begin,
			double penetration,
			boolean hasPenetration) {
        float xmin = 0, xmax = 0, ymin = 0, ymax = 0;
        float end = 0;

        // extend of map
        boolean first = true;
        for (Edge edge: edges) {
        	for (Lane lane: edge.lanes.values() ) {
	        	if (first) {
	        		first = false;
	        		xmin = Math.min(lane.xfrom, lane.xto);
	        		xmax = Math.max(lane.xfrom, lane.xto);
	        		ymin = Math.min(lane.yfrom, lane.yto);
	        		ymax = Math.max(lane.yfrom, lane.yto);
	        	} else {
	        		xmin = Math.min(Math.min(lane.xfrom, lane.xto), xmin);
	        		xmax = Math.max(Math.max(lane.xfrom, lane.xto), xmax);
	        		ymin = Math.min(Math.min(lane.yfrom, lane.yto), ymin);
	        		ymax = Math.max(Math.max(lane.yfrom, lane.yto), ymax);
	        	}
        	}
        }
        xmin = (float) Math.floor(xmin);
        xmax = (float) Math.ceil(xmax);
        ymin = (float) Math.floor(ymin);
        ymax = (float) Math.ceil(ymax);
        

        // duration of simulation
        first = true;
        for (String id: wantedVehicle) {
        	if (first) {
        		first = false;
        		end   = vehicleLastOcc.get(id).floatValue();
        	} else {
        		end   = Math.max(vehicleLastOcc.get(id).floatValue(), end);
        	}
        }

        // write!
        try {
			PrintWriter out = new PrintWriter(config);
			
			out.println("# set number of nodes");
			if (hasPenetration) {
				out.println("set opt(nn) " + Math.floor(penetration*wantedVehicle.size()));
			} else {
				int N = wantedVehicle.size();
				for (int i = 0; i <= N-1; i++) {
					out.print("if { $opt(penetration) > " + ((double) i/N)  + " } { ");
					out.print("set opt(nn) " + (i+1));
					out.println(" }");
				}
			}
			out.println();
			out.println("# set activity file");
			out.println("set opt(af) $opt(config-path)");
			out.println("append opt(af) /" + activity);
			out.println();
			out.println("# set mobility file");
			out.println("set opt(mf) $opt(config-path)");
			out.println("append opt(mf) /" + mobility);
			out.println();
			out.println("# set start/stop time");
			out.println("set opt(start) 0.0");
			out.println("set opt(stop) " + (end-begin));
			out.println();
			out.println("# set floor size");
			out.println("set opt(x) " + (int) xmax);
			out.println("set opt(y) " + (int) ymax);
			out.println("set opt(min-x) " + (int) xmin);
			out.println("set opt(min-y) " + (int) ymin);
			
			out.flush();
			out.close();
		} catch (FileNotFoundException ex) {
			System.err.println(ex);
		}
	}
}
