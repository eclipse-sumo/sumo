package ns2;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.List;
import java.util.Map;

import javax.xml.stream.XMLInputFactory;
import javax.xml.stream.XMLStreamConstants;
import javax.xml.stream.XMLStreamException;
import javax.xml.stream.XMLStreamReader;

/**
 * class for reading vehicle information
 * @author Thimor Bohn <bohn@itm.uni-luebeck.de>
 *
 */
public class VehicleReader {
	/**
	 * working method
	 * @param trace name of sumo trace file
	 * @param vehicleId list of unique ids of vehicles in sumo simulation
	 * @param vehicleFirstOcc map: vehicle id -> first occurence of vehicle in sumo
	 * @param vehicleLastOcc map: vehicle id -> last occurence of vehicle in sumo
	 */
	public static void read(
			String trace, 
			List<String> vehicleId, 
			Map<String, Double> vehicleFirstOcc, 
			Map<String, Double> vehicleLastOcc) {
		try {
			InputStream in = new FileInputStream(trace);
			XMLInputFactory factory = XMLInputFactory.newInstance();
			XMLStreamReader parser = factory.createXMLStreamReader(in);
			float time = -1;
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
					// vehicle element found
					if (parser.getLocalName().equals("vehicle")) {
						String id = "";
						for (int attr = 0; attr < parser.getAttributeCount(); attr++) {
							String attrName = parser.getAttributeLocalName(attr);
							String value = parser.getAttributeValue(attr);
							if ("id".equals(attrName)) {
								id = value;
							}
						}
						
						// add id if not already added
						if (!vehicleId.contains(id)) {
							vehicleId.add(id);
							// id is new -> first occurence of id
							vehicleFirstOcc.put(id, new Double(time));
						}
						// maybe last occurence of id
						vehicleLastOcc.put(id, new Double(time));
					}
				}
			}
			parser.close();
		} catch (XMLStreamException ex) {
			System.err.println(ex);
		} catch (IOException ex) {
			System.err.println(ex);
		}
	}
}
