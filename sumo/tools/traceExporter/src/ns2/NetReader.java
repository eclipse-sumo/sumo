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
 * class for reading net file
 * @author Thimor Bohn <bohn@itm.uni-luebeck.de>
 * @author Matthias Röckl <matthias.roeckl@dlr.de>
 *
 */
public class NetReader {
	/**
	 * method for reading sumo net file
	 * @param net name of sumo net file
	 * @param edges holds net after execution
	 */
	public static void read(String net, List<Edge> edges, Map<String, Junction> junctions) {		
        try {
            InputStream in = new FileInputStream(net);
            XMLInputFactory factory = XMLInputFactory.newInstance();
            XMLStreamReader parser = factory.createXMLStreamReader(in);
            // parse net file
            Edge edge = null;
            for (int event = parser.next(); event != XMLStreamConstants.END_DOCUMENT; event = parser.next()) {
                if (event == XMLStreamConstants.START_ELEMENT) {
                	// edge element found
                    if (parser.getLocalName().equals("edge")) {
                        String id    = "";
                        String from  = "";
                        String to    = "";
                        // parse attributes of element
                        for (int attr=0; attr < parser.getAttributeCount(); attr++) {
                            String attrName = parser.getAttributeLocalName(attr);
                            String value = parser.getAttributeValue(attr);
                            if ("id".equals(attrName)) {
								id = value;
							}
                            if ("from".equals(attrName)) {
								from = value;
							}
                            if ("to".equals(attrName)) {
								to = value;
							}
                            
                        }
                        // construct edge element
                        edge = new Edge(id, from, to);
                        // store edge
                        edges.add(edge);
                    }
                    if (parser.getLocalName().equals("lane")) {
                    	String id      = "";
                    	float[] x = new float[2];
                    	float[] y = new float[2];
                        float length   = 0;
                        boolean hadShape = false;
                    	// parse attributes of element
                        for (int attr=0; attr < parser.getAttributeCount(); attr++) {
                            String attrName = parser.getAttributeLocalName(attr);
                            String value = parser.getAttributeValue(attr);
                            if ("id".equals(attrName)) {
    							id = value;
    						}
                            if ("length".equals(attrName)) {
    							length = Float.parseFloat(value);
    						}
                            if ("shape".equals(attrName)) {
        						String[] coordinatePairs = value.split(" ");   // coordinate pairs of the shape are separated by spaces
								x = new float[coordinatePairs.length];
								y = new float[coordinatePairs.length];
								for (int i=0; i<coordinatePairs.length; i++)
								{
									String[] xAndY = coordinatePairs[i].split(",");   // X and Y of a single coordinate pair are separated by comma
	        	                    x[i] = Float.parseFloat(xAndY[0]);
	        	                    y[i] = Float.parseFloat(xAndY[1]);
	        	                    hadShape = true;
								}
                            }
                        }
                        if(!hadShape) {
                        	String text   = parser.getElementText();
                        	String[] vals = text.split("[,\\ ]"); // separate by delimiters "," and " "
                        	x[0] = Float.parseFloat(vals[0]);
                        	y[0] = Float.parseFloat(vals[1]);
                        	x[1]   = Float.parseFloat(vals[2]);
                        	y[1]   = Float.parseFloat(vals[3]);
                        }
                		edge.lanes.put(id, new Lane(id, x, y, length));
                    }
                    if (parser.getLocalName().equals("junction")) {
                    	String id = "";
                    	float x = 0;
                    	float y = 0;
                    	// parse attributes of element
                        for (int attr=0; attr < parser.getAttributeCount(); attr++) {
                            String attrName = parser.getAttributeLocalName(attr);
                            String value = parser.getAttributeValue(attr);
                            if ("id".equals(attrName)) {
    							id = value;
                            }
                            if ("x".equals(attrName)) {
    							x = Float.parseFloat(value);
    						}
                            if ("y".equals(attrName)) {
    							y = Float.parseFloat(value);
                            }
                        }
                        Junction junction = new Junction(id, x, y);
                        junctions.put(id, junction);
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
