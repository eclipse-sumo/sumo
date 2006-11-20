package ns2;

import java.io.FileNotFoundException;
import java.io.PrintWriter;
import java.util.List;

/**
 * class for writing config file
 * @author Thimor Bohn <bohn@itm.uni-luebeck.de>
 *
 */
public class ConfigWriter {
	/**
	 * method for writing config file
	 * @param config name of config file
	 * @param mobility name of mobility file
	 * @param activity name of activity file
	 * @param edges stored edges
	 * @param vehicles stored vehicles
	 */
	public static void write(String config, String mobility, String activity, List<Edge> edges, List<Vehicle> vehicles, List<Vehicle> equippedVehicles) {
        float xmin = 0, xmax = 0, ymin = 0, ymax = 0;
        float start = 0, end = 0;

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
        for (Vehicle vehicle: vehicles) {
        	if (first) {
        		first = false;
        		start = vehicle.time_first;
        		end   = vehicle.time_last;
        	} else {
        		start = Math.min(vehicle.time_first, start) ;
        		end   = Math.max(vehicle.time_last, end);
        	}
        }

        // write!
        try {
			PrintWriter out = new PrintWriter(config);
			
			out.println("# set number of nodes");
			out.println("set opt(nn) " + equippedVehicles.size());
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
			out.println("set opt(start) " + start);
			out.println("set opt(stop) " + end);
			out.println();
			out.println("# set floor size");
			out.println("set opt(x) " + (int) xmax);
			out.println("set opt(y) " + (int) ymax);
			out.println("set opt(min-x) " + (int) xmin);
			out.println("set opt(min-y) " + (int) ymin);
			out.flush();
			out.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
	}
}
