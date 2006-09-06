package ns2;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.PrintWriter;
import java.util.HashMap;
import java.util.Iterator;
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
			HashMap<String, Integer> partialVehicleIds, List<Edge> edges,
			List<Vehicle> equippedVehicles, double penetration) {
		try {
			InputStream in = new FileInputStream(trace);
			XMLInputFactory factory = XMLInputFactory.newInstance();
			XMLStreamReader parser = factory.createXMLStreamReader(in);
			String edgeid = "";
			String laneid = "";
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
					// lane element found
					if (parser.getLocalName().equals("lane")) {
						for (int attr = 0; attr < parser
								.getAttributeCount(); attr++) {
							String attrName = parser
									.getAttributeLocalName(attr);
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
						
						// get lane of edge
						Lane thislane = thisedge.lanes.get(laneid);
						
						// calculate positons of vehicle
						x = thislane.xfrom + pos
								* (thislane.xto - thislane.xfrom)
								/ thisedge.length;
						y = thislane.yfrom + pos
								* (thislane.yto - thislane.yfrom)
								/ thisedge.length;
						Vehicle vehicle = null;
						// new vehicle found
						// * create new vehicles object
						// * creation time := 0    if time <  0
						//                    time if time >= 0
						if (!vehicleIds.containsKey(id)) {
							// create Vehicle object
							float _time = (time>=0) ? time : 0; // (!) time >= 0
							vehicle = new Vehicle(id, x, y, _time, v);
							// and save it
							vehicles.add(vehicle);
							// give it new id and save it too
							vehicleIds.put(id, vehicleIds.size());
							// apply pentration factor
							assert (penetration >= 0 && penetration <= 1);
							if (Math.random() <= penetration) {
								equippedVehicles.add(vehicle);
							}
						} else {
							vehicle = vehicles.get(vehicleIds.get(id));
							vehicle.setX(x);
							vehicle.setY(y);
							vehicle.setSpeed(v);
						}

						// write vehicle movement if time >= 0
						if (time >= 0) {
							if (equippedVehicles.contains(vehicles.get(vehicleIds.get(id)))) {
								if (!partialVehicleIds.containsKey(id)) {
									partialVehicleIds.put(id, partialVehicleIds.size());
								}
								// write to mobility file
								out.println("$ns_ at " + time + 
										" \"$node_(" + partialVehicleIds.get(id) + ") "
										+ "setdest " + vehicle.getX() + " " + vehicle.getY() + " " + vehicle.getSpeed() + "\"");
							}
						}
						// save some data for activity file (last occurence of vehicle)
						vehicles.get(vehicleIds.get(id)).setTime(time);
					}
				}
			}
			parser.close();
		} catch (XMLStreamException ex) {
			System.out.println(ex);
		} catch (IOException ex) {
			System.out.println("IOException while parsing " + trace);
		}
		// remove vehicles existing only at time<=0
		Iterator<Vehicle> iter = vehicles.iterator();
		while (iter.hasNext()) {
			Vehicle vehicle = iter.next();
			if (vehicle.getStopTime() <= vehicle.getStartTime()) {
				equippedVehicles.remove(vehicle);
				iter.remove();
			}
		}
	}
}
