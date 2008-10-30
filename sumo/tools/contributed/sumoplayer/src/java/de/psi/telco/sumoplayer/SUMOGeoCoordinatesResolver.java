package de.psi.telco.sumoplayer;

import java.io.File;
import java.io.IOException;

import javax.xml.parsers.ParserConfigurationException;
import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;

import org.xml.sax.SAXException;

import de.psi.telco.sumoplayer.util.PROJTransformer;
import de.psi.telco.sumoplayer.util.Point;
import de.psi.telco.sumoplayer.util.PointImpl;

public class SUMOGeoCoordinatesResolver {
	
	private File networkFile;
	private SUMONetwork network = new SUMONetwork();
	private PROJTransformer proj;
	
	public SUMOGeoCoordinatesResolver(String netfile){
		this.networkFile = new File(netfile);
		this.readNetwork();
		
		this.proj = new PROJTransformer(network.projString, network.offset);
	}
	
	private void readNetwork() {
        SAXParserFactory saxFactory = SAXParserFactory.newInstance();
        SAXParser saxParser = null;
        try {
			saxParser = saxFactory.newSAXParser();
		} catch (ParserConfigurationException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (SAXException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		SUMOSAXNeworkfileHandler saxHandler = new SUMOSAXNeworkfileHandler();
		System.out.println("Reading SUMO Networkfile...");
        try {
			saxParser.parse(networkFile, saxHandler);
		} catch (SAXException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
        System.out.println("done.");
        
        this.network = saxHandler.network;
	}

	public Point resolv(String edgeId, String laneId, double pos){
		//System.out.println("Resolver call - edge:"+edgeId+" - lane:"+laneId+" - pod:"+pos);
		if (this.network != null){	// network loaded. do look up
			Point out = network.lanes.get(laneId).getPositionWithin(pos);
			
			out = proj.transformInvers(out);
			return out;
		}else{
			return new PointImpl(0,0);
		}
	}
}
