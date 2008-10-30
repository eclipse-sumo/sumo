package de.psi.telco.sumoplayer;

import java.util.HashSet;
import java.util.Set;

import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

import de.psi.telco.sumoplayer.util.GeoCalc;
import de.psi.telco.sumoplayer.util.Point;
import de.psi.telco.sumoplayer.util.PointImpl;

/**
 * This SAX Handler parses SUMO "--network-dump" output files.
 * It guesses which vehicles are tracked by a given propability.
 * 
 * @author will
 *
 */
public class SUMOSAXHandler extends DefaultHandler{
	
	/**
	 * The given propability which decides which cars are beeing tracked
	 * 0 means no vehicles 1 means all vehicles. anything else between. 
	 */
	private double car4carEquippedPropbaility = 0.01;
	private int jitter = 0;
	
	private Set<String> trackedVehicles = new HashSet<String>();	// The set of vehicles which are tracked
	private Set<String> knownVehicles = new HashSet<String>();
	
	private int timestep;
	private long startTime;

	private String edgeId;
	private String laneId;
	
	private String vehicleId;
	private double pos;
	private double speed;
	
	private SUMOLocationListener listener;
	private SUMOGeoCoordinatesResolver resolver;
	
	public SUMOSAXHandler(String netfilename){
		this.resolver = new SUMOGeoCoordinatesResolver(netfilename);
	}
	
	public void startDocument(){
		startTime = System.currentTimeMillis();
	}
	
	public void startElement (String uri, String localName, String qName, Attributes attributes) throws SAXException{
		if (qName.equals("timestep")){
			timestep = Integer.parseInt(attributes.getValue("time"));
			
			long timeToBe = startTime + timestep*1000;	// dirty realtime trick
			long now = System.currentTimeMillis();
			try {
				if (timeToBe-now>0){
					Thread.sleep(timeToBe-now);
				}
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}else if(qName.equals("edge")){
			edgeId = attributes.getValue("id"); 
		}else if(qName.equals("lane")){
			laneId = attributes.getValue("id");
		}else if(qName.equals("vehicle")){
			vehicleId = attributes.getValue("id");
			pos = Double.parseDouble(attributes.getValue("pos"));
			speed = Double.parseDouble(attributes.getValue("speed"));
			
			handleVehicle();
		}
	}
	
	private void handleVehicle(){
		if (trackedVehicles.size() == 0){	// add first vehicle anyway for simulations whith only one vehicle
			knownVehicles.add(vehicleId);
			trackedVehicles.add(vehicleId);
		}else{	// add vehicles probabilistic
			if (!knownVehicles.contains(vehicleId)){	// check if vehicle is already known
				
				knownVehicles.add(vehicleId);	// then add it as known
				if (car4carEquippedPropbaility > Math.random()){	// and calculate propability
					trackedVehicles.add(vehicleId);					// ... to add it
				}
			}
		}
		
		if (trackedVehicles.contains(vehicleId)){	// handle a tracked vehicle only
			// calculate geo position
			Point p = resolver.resolv(edgeId, laneId, pos);
			
			// calculate jitter if required
			if (jitter > 0){
				double jitterLon = GeoCalc.getLonOffset(p.getX(), p.getY(), jitter)*((Math.random()*2)-1);
				double jitterLat = GeoCalc.getLatOffset(p.getX(), p.getY(), jitter)*((Math.random()*2)-1);
				p = new PointImpl(p.getX()+jitterLon,p.getY()+jitterLat);
			}
			
			// call the listener
			this.listener.LocationUpdated(vehicleId, timestep, p.getX(), p.getY(), speed*3.6);
		}
	}

	public double getCar4carEquippedPropbaility() {
		return car4carEquippedPropbaility;
	}

	public void setCar4carEquippedPropability(double car4carEquippedPropbaility) {
		this.car4carEquippedPropbaility = car4carEquippedPropbaility;
	}

	public void setListener(SUMOLocationListener listener) {
		this.listener = listener;
	}

	public void setResolver(SUMOGeoCoordinatesResolver resolver) {
		this.resolver = resolver;
	}
	
	/**
	 * This function sets the GPS jitter in meters
	 * @param jitter
	 */
	public void setJitter(int jitter){
		this.jitter = jitter;
	}
}
