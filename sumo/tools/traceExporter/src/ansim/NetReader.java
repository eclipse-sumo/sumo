package ansim;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.List;

import javax.xml.stream.XMLInputFactory;
import javax.xml.stream.XMLStreamConstants;
import javax.xml.stream.XMLStreamException;
import javax.xml.stream.XMLStreamReader;

import ansim.Edge;

/**
 * class for reading net file
 * 
 * @author Thimor Bohn <bohn@itm.uni-luebeck.de>
 * 
 */
public class NetReader {
	/**
	 * method for reading net file
	 * 
	 * @param net name of netfile
	 * @param edges holds net after execution
	 */
	public static void read(String net, List<Edge> edges) {
		try {
			InputStream in = new FileInputStream(net);
			XMLInputFactory factory = XMLInputFactory.newInstance();
			XMLStreamReader parser = factory.createXMLStreamReader(in);
			for (int event = parser.next(); event != XMLStreamConstants.END_DOCUMENT; event = parser.next()) {
				if (event == XMLStreamConstants.START_ELEMENT) {
					if (parser.getLocalName().equals("edge")) {
						String id = "";
						float length = 0;
						float speed = 0;
						String name = "";
						float xfrom = 0;
						float yfrom = 0;
						float xto = 0;
						float yto = 0;
						for (int attr = 0; attr < parser.getAttributeCount(); attr++) {
							String attrName = parser.getAttributeLocalName(attr);
							String value = parser.getAttributeValue(attr);
							if ("id".equals(attrName))
								id = value;
							if ("Length".equals(attrName))
								length = Float.parseFloat(value);
							if ("Speed".equals(attrName))
								speed = Float.parseFloat(value);
							if ("Name".equals(attrName))
								name = value;
							if ("XFrom".equals(attrName))
								xfrom = Float.parseFloat(value);
							if ("YFrom".equals(attrName))
								yfrom = Float.parseFloat(value);
							if ("XTo".equals(attrName))
								xto = Float.parseFloat(value);
							if ("YTo".equals(attrName))
								yto = Float.parseFloat(value);

						}
						Edge edge = new Edge(id, length, speed, name, xfrom,yfrom, xto, yto);
						edges.add(edge);
					}
				}
			}
			parser.close();
		} catch (XMLStreamException ex) {
			System.out.println(ex);
		} catch (IOException ex) {
			System.out.println("IOException while parsing " + net);
		}
	}
}
