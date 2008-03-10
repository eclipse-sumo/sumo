package ns2;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.PrintWriter;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.xml.stream.XMLInputFactory;
import javax.xml.stream.XMLStreamConstants;
import javax.xml.stream.XMLStreamException;
import javax.xml.stream.XMLStreamReader;

/**
 * class for writing ns2 mobility files
 * @author Thimor Bohn <bohn@itm.uni-luebeck.de>
 *
 */
public class MobilityWriter {
	/**
	 * working method
	 * @param trace name of sumo netstate-dump file
	 * @param mobility name of ns2 mobility file
	 * @param wantedVehicle list of vehicles to be selected for ns2
	 * @param vehicleNewId list of vehicle ids for ns2
	 * @param edges list of edges used in sumo simulation
	 * @param begin sumo time at which ns2 should start to simulate
	 * @param end sumo time at which ns2 should stop to simulate
	 */
	public static void write(
			String trace, 
			String mobility,
			List<String> wantedVehicle, 
			List<String> vehicleNewId,
			List<Edge> edges, 
			double begin, 
			double end,
			double penetration,
			boolean hasPenetration) {
		try {
			InputStream in = new FileInputStream(trace);
			PrintWriter out = new PrintWriter(mobility);
			XMLInputFactory factory = XMLInputFactory.newInstance();
			XMLStreamReader parser = factory.createXMLStreamReader(in);
			String edgeid = "";
			String laneid = "";
			float time = -1;
			Map<String, Double> initialX = new HashMap<String, Double>();
			Map<String, Double> initialY = new HashMap<String, Double>();
			// parse trace file
			for (int event = parser.next(); event != XMLStreamConstants.END_DOCUMENT; event = parser.next()) {
				if (event == XMLStreamConstants.START_ELEMENT) {
					// get current time
					if (parser.getLocalName().equals("timestep")) {
						for (int attr = 0; attr < parser.getAttributeCount(); attr++) {
							String attrName = parser.getAttributeLocalName(attr);
							String value = parser.getAttributeValue(attr);
							if ("time".equals(attrName)) {
								time = Float.parseFloat(value);
							}
						}
					}
					// wanted time
					if (time >= begin && time <= end) {
						// edge element found
						if (parser.getLocalName().equals("edge")) {
							for (int attr = 0; attr < parser.getAttributeCount(); attr++) {
								String attrName = parser.getAttributeLocalName(attr);
								String value = parser.getAttributeValue(attr);
								if ("id".equals(attrName)) {
									edgeid = value;
								}
							}
						}
						// lane element found
						if (parser.getLocalName().equals("lane")) {
							for (int attr = 0; attr < parser.getAttributeCount(); attr++) {
								String attrName = parser.getAttributeLocalName(attr);
								String value = parser.getAttributeValue(attr);
								if ("id".equals(attrName)) {
									laneid = value;
								}
							}
						}
						// vehicle element found
						if (parser.getLocalName().equals("vehicle")) {
							String id = "";
							float pos = 0;
							float x = 0;
							float y = 0;
							float v = 0;
							for (int attr = 0; attr < parser.getAttributeCount(); attr++) {
								String attrName = parser.getAttributeLocalName(attr);
								String value = parser.getAttributeValue(attr);
								if ("id".equals(attrName)) {
									id = value;
								}
								if ("pos".equals(attrName)) {
									pos = Float.parseFloat(value);
								}
								if ("speed".equals(attrName)) {
									v = Float.parseFloat(value);
								}
							}

							// find corresponding edge object
							Edge thisedge = null;
							for (Edge edge : edges) {
								thisedge = edge;
								if (edge.id.equals(edgeid)) {
									break;
								}
							}

							// get lane of edge
							Lane thislane = thisedge.lanes.get(laneid);
							
							// calculate positons of vehicle
							x = thislane.xfrom + pos * (thislane.xto - thislane.xfrom) / thislane.length;
							y = thislane.yfrom + pos * (thislane.yto - thislane.yfrom) / thislane.length;
							
							// add to inititial positions if not already inside
							if (!initialX.containsKey(id)) {
								initialX.put(id, new Double(x));
								initialY.put(id, new Double(y));
							}
							
							// write to mobility file
							String newId =  vehicleNewId.get(wantedVehicle.indexOf(id));
							double minP =  Double.parseDouble(newId)/wantedVehicle.size();
							if (hasPenetration) {
								if (penetration > minP) {
									out.println("$ns_ at " + (time-begin) + " \"$node_(" + newId + ") " + "setdest " + x + " " + y + " " + v + "\"");
								}
							} else {
								out.print("if { $opt(penetration) > " + minP  + " } { ");
								out.print("$ns_ at " + (time-begin) + " \"$node_(" + newId + ") " + "setdest " + x + " " + y + " " + v + "\"");
								out.println(" }");
							}
						}
					}
				}
			}
			parser.close();

			// write initial positions of vehicles to mobility file
	        for (String id: wantedVehicle) {
				String newId =  vehicleNewId.get(wantedVehicle.indexOf(id));
				double minP =  Double.parseDouble(newId)/wantedVehicle.size();
				if (hasPenetration) {
					if (penetration > minP) {
			            out.print("  $node_(" + newId + ") ");
			            out.print("  set X_ " + initialX.get(id));
			            out.println("\t# SUMO-ID: " + id);
			            out.print("  $node_(" + newId + ") ");
			            out.println("  set Y_ " + initialY.get(id) + " ");
			            out.print("  $node_(" + newId + ") ");
			            out.println("  set Z_ 0.0 ");
					}
	        	} else {
	        		out.println("if { $opt(penetration) > " + minP  + " } { ");
		            out.print("  $node_(" + newId + ") ");
		            out.println("  set X_ " + initialX.get(id));
		            out.print("  $node_(" + newId + ") ");
		            out.println("  set Y_ " + initialY.get(id) + " ");
		            out.print("  $node_(" + newId + ") ");
		            out.println("  set Z_ 0.0 ");
		            out.println("}");	        		
	        	}
	        }

			out.flush();
			out.close();
		} catch (XMLStreamException ex) {
			System.err.println(ex);
		} catch (IOException ex) {
			System.err.println(ex);
		}
	}
}
