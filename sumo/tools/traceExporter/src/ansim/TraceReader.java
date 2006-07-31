package ansim;

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

import ansim.Edge;
import ansim.Vehicle;

/**
 * class for reading sumo trace file
 * @author Thimor Bohn <bohn@itm.uni-luebeck.de>
 *
 */
public class TraceReader {
	/**
	 * method for reading trace file - stage 1
	 * @param trace name of trace file
	 * @param vehicles holds all vehicles after execution
	 * @param vehicleIds holds corresponding ids
	 * @param edges net
	 */
	public static void readFirst(String trace, List<Vehicle> vehicles, HashMap<String, Integer> vehicleIds, List<Edge> edges) {
		try {
            InputStream in = new FileInputStream(trace);
            XMLInputFactory factory = XMLInputFactory.newInstance();
            XMLStreamReader parser = factory.createXMLStreamReader(in);
            for (int event = parser.next(); event != XMLStreamConstants.END_DOCUMENT; event = parser.next()) {
                String edgeid = "";
                if (event == XMLStreamConstants.START_ELEMENT) {
                	// edge-element found
                    if (parser.getLocalName().equals("edge")) {
                        for (int attr=0; attr < parser.getAttributeCount(); attr++) {
                            String attrName = parser.getAttributeLocalName(attr);
                            String value = parser.getAttributeValue(attr);
                            if ("id".equals(attrName)) edgeid = value;
                        }
                    }
                    // vehicle-element found
                    if (parser.getLocalName().equals("vehicle")) {
                        String id = "";
                        float pos = 0;
                        float x = 0;
                        float y = 0;
                        for (int attr=0; attr < parser.getAttributeCount(); attr++) {
                            String attrName = parser.getAttributeLocalName(attr);
                            String value = parser.getAttributeValue(attr);
                            if ("id".equals(attrName))       id = value;
                            if ("pos".equals(attrName))      pos = Float.parseFloat(value);
                        }
                        // get vehicle position
                        if (!vehicleIds.containsKey(id)) {
                            Edge thisedge = null;
                            for (Edge edge: edges) {
                                thisedge = edge;
                                if (edge.id.equals(edgeid)) break;
                            }
                            x = thisedge.xfrom + pos * (thisedge.xto - thisedge.xfrom) / thisedge.length;
                            y = thisedge.yfrom + pos * (thisedge.yto - thisedge.yfrom) / thisedge.length;
                            Vehicle vehicle = new Vehicle(id, x, y);
                            vehicles.add(vehicle);
                            vehicleIds.put(id, vehicleIds.size());
                        }
                    }
                }
            }
            parser.close();
        } catch (XMLStreamException e) {
            System.err.println(e);
        } catch (IOException e) {
        	System.err.println(e);
            //System.out.println("IOException while parsing " + trace);
        }
	}
	
	/**
	 * method for reading sumo tracefile - stage 2 (and writing movements to ansim tracefile)
	 * @param out writer for ansim tracefile
	 * @param trace name of sumo tracefile
	 * @param vehicles holds all vehicles to be written
	 * @param vehicleIds holds corresponding ids
	 * @param edges net
	 */
	public static void readSecond(PrintWriter out, String trace, List<Vehicle> vehicles, HashMap<String, Integer> vehicleIds, List<Edge> edges) {
        try {
            InputStream in = new FileInputStream(trace);
            XMLInputFactory factory = XMLInputFactory.newInstance();
            XMLStreamReader parser = factory.createXMLStreamReader(in);
            String edgeid = "";
            float time = -1;
            out.println("<mobility>");
            // parse tracefile
            for (int event = parser.next(); event != XMLStreamConstants.END_DOCUMENT; event = parser.next()) {
                if (event == XMLStreamConstants.START_ELEMENT) {
                	// get current time
                    if (parser.getLocalName().equals("timestep")) {
                        for (int attr=0; attr < parser.getAttributeCount(); attr++) {
                            String attrName = parser.getAttributeLocalName(attr);
                            String value = parser.getAttributeValue(attr);
                            if ("time".equals(attrName))       time = Float.parseFloat(value);
                        }
                    }
                    // process only positive time
                    if (time>0) {
                    	// edge found
                        if (parser.getLocalName().equals("edge")) {
                            for (int attr=0; attr < parser.getAttributeCount(); attr++) {
                                String attrName = parser.getAttributeLocalName(attr);
                                String value = parser.getAttributeValue(attr);
                                if ("id".equals(attrName))       edgeid =        value;
                            }
                        }
                        // vehicle found
                        if (parser.getLocalName().equals("vehicle")) {
                            String id = "";
                            float pos = 0;
                            float x = 0;
                            float y = 0;
                            for (int attr=0; attr < parser.getAttributeCount(); attr++) {
                                String attrName = parser.getAttributeLocalName(attr);
                                String value = parser.getAttributeValue(attr);
                                if ("id".equals(attrName))       id = value;
                                if ("pos".equals(attrName))      pos = Float.parseFloat(value);
                            }
                            Edge thisedge = null;
                            // find/calc position of vehicle
                            for (Edge edge: edges) {
                                thisedge = edge;
                                if (edge.id.equals(edgeid)) break;
                            }
                            x = thisedge.xfrom + pos * (thisedge.xto - thisedge.xfrom) / thisedge.length;
                            y = thisedge.yfrom + pos * (thisedge.yto - thisedge.yfrom) / thisedge.length;

                            // write current entry to ansim tracefile
                            out.println("  <position_change>");
                            out.println("    <node_id>" + vehicleIds.get(id) + "</node_id>");
                            out.println("    <start_time>" + time + "</start_time>");
                            out.println("    <end_time>" + (time+1) + "</end_time>");
                            out.println("    <destination>");
                            out.println("      <xpos>" + x + "</xpos>");
                            out.println("      <ypos>" + y + "</ypos>");
                            out.println("    </destination>");
                            out.println("  </position_change>");

                        }
                    }
                }
            }
            out.println("</mobility>");
            parser.close();
        } catch (XMLStreamException e) {
        	System.err.println(e);
        } catch (IOException e) {
        	System.err.println(e);
        }
	}
}
