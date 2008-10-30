package de.psi.telco.sumoplayer;

import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

import de.psi.telco.sumoplayer.util.PointImpl;

/**
 * This SAX parser can be used to read SUMO networkfiles.
 * It builds a SUMONetwork object containing the networks topology
 * @author will
 *
 */
public class SUMOSAXNeworkfileHandler extends DefaultHandler{

	// build data
	public SUMONetwork network = new SUMONetwork();;
	
	// element state
	private static int UNKNOWN = 0;
	private static int EDGE = 1;
	private static int LANE = 2;
	private static int CEDGE = 3;
	private static int JUNCTION = 4;
	private static int OFFSET = 5;
	private static int PROJ = 6;
	private int currentElementType;
	
	// store
	private String sumoEdgeId; 
	private String sumoLaneId;
	private String cEdgeId;
	private String junctionId;
	
	public void startElement (String uri, String localName, String qName, Attributes attributes) throws SAXException{
		if (qName.equals("edge")){
			this.currentElementType = EDGE;
			sumoEdgeId = attributes.getValue("id");
			
		}else if (qName.equals("lane")){
			this.currentElementType = LANE;
			sumoLaneId = attributes.getValue("id");
			
			network.lanes.put(sumoLaneId, new SUMOLane(sumoLaneId));
			
		}else if (qName.equals("cedge")){	// cedges seem to be mappings from sumo.edg.xml IDs to sumoIDs
			this.currentElementType = CEDGE;
			cEdgeId = attributes.getValue("id");
		}
		
		/*	// note junctions not interesting. all coordinates are stored within lanes
		else if (qName.equals("junction")){	// junction found. add it
			this.currentEementType = JUNCTION;
			
			//  i.e. <junction id="124667945" type="priority" x="3186.50" y="17778.50">...</junction>
			junctionId = attributes.getValue("id");
			double x = Double.parseDouble(attributes.getValue("x"));
			double y = Double.parseDouble(attributes.getValue("y"));
			network.addJunction(junctionId, x, y);
		}*/
		
		// some network cfg
		else if (qName.equals("net-offset")){
			this.currentElementType = OFFSET;
		}else if (qName.equals("orig-proj")){
			this.currentElementType = PROJ;
		}
		
	}
	
	public void endElement (String uri, String localName, String qName) throws SAXException{
		this.currentElementType = UNKNOWN;
	}
	
	// this can be used to read coordinates from edges for example:  <edge id="bla"...>COORD1X,COORD1Y COORD2X,COORD2Y</edge>  
	public void characters (char ch[], int start, int length){
		
		if (currentElementType == LANE){	// read coords from lane
			
			char[] chars = new char[length];	// copy stuff from sax buffer
			for (int i = 0; i<length; i++){
				chars[i] = ch[start+i];
			}
			String data = new String(chars);
			
			String[] coordPairs = data.split(" ");	// split by pairs (  COORD1X,COORD1Y COORD2X,COORD2Y ...)
			for (int i = 0 ; i < coordPairs.length; i++){
				String[] coordElements = coordPairs[i].split(",");
				if (coordElements != null && coordElements.length == 2){	// split by comma ( COORD1X,COORD1Y )
					try{
						double x = Double.parseDouble(coordElements[0]);
						double y = Double.parseDouble(coordElements[1]);
						
						network.lanes.get(sumoLaneId).points.add(new PointImpl(x,y));
					}catch(NumberFormatException e){
					}
				}				
			}

		}else if (currentElementType == OFFSET){	// read offset. i.e:  <net-offset>-755969.000000,-5660071.000000</net-offset>
			char[] chars = new char[length];	// copy stuff from sax buffer
			for (int i = 0; i<length; i++){
				chars[i] = ch[start+i];
			}
			String data = new String(chars);
			String[] vectorElements = data.split(",");
			if (vectorElements.length==2){
				network.offset = new PointImpl(Double.parseDouble(vectorElements[0]),Double.parseDouble(vectorElements[1]));
			}
		}else if (currentElementType == PROJ){	// read projection string. i.e:  <orig-proj>+proj=utm +ellps=bessel +units=m</orig-proj>
			char[] chars = new char[length];	// copy stuff from sax buffer
			for (int i = 0; i<length; i++){
				chars[i] = ch[start+i];
			}
			network.projString = new String(chars); 
		}
	}
}
