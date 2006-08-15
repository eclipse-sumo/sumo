package ns2;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.PrintWriter;
import java.util.HashMap;
import java.util.List;

import javax.xml.stream.XMLInputFactory;
import javax.xml.stream.XMLStreamConstants;
import javax.xml.stream.XMLStreamException;
import javax.xml.stream.XMLStreamReader;

/**
 * class for reading trace file
 * 
 * @author Thimor
 * 
 */
public class TraceReader {
	/**
	 * method for second stage reading trace file (and writing transformed data
	 * on the fly to mobility file)
	 * 
	 * @param out
	 *            mobility file printwriter
	 * @param trace
	 *            name of trace file
	 * @param vehicles
	 *            holds all vehicles
	 * @param vehicleIds
	 *            holds vehicle ids
	 * @param edges
	 *            holds net
	 */
	public static void read(PrintWriter out, String trace,
			List<Vehicle> vehicles, HashMap<String, Integer> vehicleIds,
			List<Edge> edges, List<Vehicle> equippedVehicles, double penetration) {
		try {
			InputStream in = new FileInputStream(trace);
			XMLInputFactory factory = XMLInputFactory.newInstance();
			XMLStreamReader parser = factory.createXMLStreamReader(in);
			String edgeid = "";
			float time = -1;
			// parse trace file
			for (int event = parser.next(); event != XMLStreamConstants.END_DOCUMENT; event = parser
					.next()) {
				if (event == XMLStreamConstants.START_ELEMENT) {
					// get current time
					if (parser.getLocalName().equals("timestep")) {
						for (int attr = 0; attr < parser.getAttributeCount(); attr++) {
							String attrName = parser
									.getAttributeLocalName(attr);
							String value = parser.getAttributeValue(attr);
							if ("time".equals(attrName)) {
								time = Float.parseFloat(value);
							}
						}
					}
					// process only positive time
					if (time > 0) {
						// edge element found
						if (parser.getLocalName().equals("edge")) {
							for (int attr = 0; attr < parser
									.getAttributeCount(); attr++) {
								String attrName = parser
										.getAttributeLocalName(attr);
								String value = parser.getAttributeValue(attr);
								if ("id".equals(attrName)) {
									edgeid = value;
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
							for (int attr = 0; attr < parser
									.getAttributeCount(); attr++) {
								String attrName = parser
										.getAttributeLocalName(attr);
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

							Edge thisedge = null;
							// find corresponding edge object
							for (Edge edge : edges) {
								thisedge = edge;
								if (edge.id.equals(edgeid)) {
									break;
								}
							}

							// calculate positons of vehicle
							x = thisedge.xfrom + pos
									* (thisedge.xto - thisedge.xfrom)
									/ thisedge.length;
							y = thisedge.yfrom + pos
									* (thisedge.yto - thisedge.yfrom)
									/ thisedge.length;

							// new vehicles found
							if (!vehicleIds.containsKey(id)) {
								// create Vehicle object
								Vehicle vehicle = new Vehicle(id, x, y, time);
								// and save it
								vehicles.add(vehicle);
								// give it new id and save it too
								vehicleIds.put(id, vehicleIds.size());
								// apply pentration factor
								assert (penetration >= 0 && penetration <= 1);
								if (Math.random() <= penetration) {
									equippedVehicles.add(vehicle);
								}
							}

							if (equippedVehicles.contains(vehicles
									.get(vehicleIds.get(id)))) {
								// write to mobility file
								out.println("$ns_ at " + time + " \"$node_("
										+ vehicleIds.get(id) + ") setdest " + x
										+ " " + y + " " + v + "\"");
							}
							// save some data for activity file (last occurence
							// of vehicle)
							vehicles.get(vehicleIds.get(id)).time_last = time;
						}
					}
				}
			}
			out.flush();
			parser.close();
		} catch (XMLStreamException ex) {
			System.out.println(ex);
		} catch (IOException ex) {
			System.out.println("IOException while parsing " + trace);
		}
	}
}
