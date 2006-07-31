package ansim;

import java.io.IOException;
import java.io.PrintWriter;
import java.util.HashMap;
import java.util.List;

/**
 * Class for writing ansim tracefiles
 * @author Thimor Bohn <bohn@itm.uni-luebeck.de>
 *
 */
public class AnsimWriter {
	/**
	 * method for writing ansim tracefile
	 * @param out name of trace file
	 * @param vehicles vehicles to be written
	 * @param vehicleIds contains vehicle ids
	 * @param edges net
	 */
	public static void write(String outfile, String trace, List<Vehicle> vehicles, HashMap<String, Integer> vehicleIds, List<Edge> edges) {
		PrintWriter out = null;
		try {
			out = new PrintWriter(outfile);
		} catch (IOException e) {
			System.err.println(e);
			System.exit(1);
		}
		
		// find extend of net
        float xmin = 0;
        float ymin = 0;
        float xmax = 0;
        float ymax = 0;
        
        xmin = edges.get(0).xfrom;
        xmax = xmin;
        ymin = edges.get(0).yfrom;
        ymax = ymin;
        for (Edge edge: edges) {
            xmin = Math.min(xmin, edge.xfrom);
            xmin = Math.min(xmin, edge.xto);
            xmax = Math.max(xmax, edge.xfrom);
            xmax = Math.max(xmax, edge.xto);
            ymin = Math.min(ymin, edge.yfrom);
            ymin = Math.min(ymin, edge.yto);
            ymax = Math.max(ymax, edge.yfrom);
            ymax = Math.max(ymax, edge.yto);
        }
        
        // write extend to header
        out.println("<?xml version=\"1.0\" ?>");
        out.println("<simulation xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://www.i-u.de/schools/hellbrueck/ansim/xml/spmobtrace.xsd\">");
        
        out.println("<parameter>");
        out.println("  <field_shape>rectangle</field_shape>");
        out.println("  <xmin>" + xmin + "</xmin>");
        out.println("  <xmax>" + xmax + "</xmax>");
        out.println("  <ymin>" + ymin + "</ymin>");
        out.println("  <ymax>" + ymax + "</ymax>");
        out.println("  <numberOfNodes>" + vehicles.size() + "</numberOfNodes>");
        out.println("</parameter>");
        out.println("<node_settings>");
        
        // write initial vehicle positions to header 
        for (Vehicle vehicle: vehicles) {
            out.println("  <node>");
            out.println("    <node_id>" + vehicleIds.get(vehicle.id) + "</node_id>");
            out.println("    <name>" + vehicle.id + "</name>");
            out.println("    <position>");
            out.println("      <xpos>" + vehicle.x + "</xpos>");
            out.println("      <ypos>" + vehicle.y + "</ypos>");
            out.println("    </position>");
            out.println("  </node>");
        }
        out.println("</node_settings>");		
        
        System.out.println("start: read trace file - stage 2");
        TraceReader.readSecond(out, trace, vehicles, vehicleIds, edges);
        System.out.println("finished: read trace file - stage 2");
        out.println("</simulation>");
        out.println();
        out.close();
	}
}
