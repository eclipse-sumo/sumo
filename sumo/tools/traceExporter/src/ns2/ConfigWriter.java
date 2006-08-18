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
        	if (first) {
        		first = false;
        		xmin = (edge.xfrom>edge.xto) ? edge.xto   : edge.xfrom;
        		xmax = (edge.xfrom>edge.xto) ? edge.xfrom : edge.xto;
        		ymin = (edge.yfrom>edge.yto) ? edge.yto   : edge.yfrom;
        		ymax = (edge.yfrom>edge.yto) ? edge.yfrom : edge.yto;
        	} else {
        		float _xmin = (edge.xfrom>edge.xto) ? edge.xto   : edge.xfrom;
        		float _xmax = (edge.xfrom>edge.xto) ? edge.xfrom : edge.xto;
        		float _ymin = (edge.yfrom>edge.yto) ? edge.yto   : edge.yfrom;
        		float _ymax = (edge.yfrom>edge.yto) ? edge.yfrom : edge.yto;
        		xmin = (xmin>_xmin) ? _xmin : xmin;
        		xmax = (xmin>_xmin) ? _xmax : xmax;
        		ymin = (ymin>_ymin) ? _ymin : ymin;
        		ymax = (ymin>_ymin) ? _ymax : ymax;
        	}
        }

        // duration of simulation
        first = true;
        for (Vehicle vehicle: vehicles) {
        	if (first) {
        		first = false;
        		start = vehicle.time_first;
        		end   = vehicle.time_last;
        	} else {
        		start = (start<vehicle.time_first) ? start             : vehicle.time_first;
        		end   = (end<vehicle.time_last)    ? vehicle.time_last : end;
        	}
        }

        // write!
        try {
			PrintWriter out = new PrintWriter(config);
			
			out.println("# set number of nodes");
			out.println("set opt(nn) " + equippedVehicles.size());
			out.println();
			out.println("# set activity file");
			out.println("set opt(af) opt(config-path)");
			out.println("append opt(af) /" + activity);
			out.println();
			out.println("# set mobility file");
			out.println("set opt(mf) opt(config-path)");
			out.println("append opt(mf) /" + mobility);
			out.println();
			out.println("# set start/stop time");
			out.println("set opt(start) " + start);
			out.println("set opt(stop) " + end);
			out.println();
			out.println("# set floor size");
			out.println("set opt(x) " + xmax);
			out.println("set opt(y) " + ymax);
			out.println("set opt(min-x) " + xmin);
			out.println("set opt(min-y) " + ymin);
			out.flush();
			out.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
	}
}
