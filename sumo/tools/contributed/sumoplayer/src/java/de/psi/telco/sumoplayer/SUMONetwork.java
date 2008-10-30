package de.psi.telco.sumoplayer;

import java.util.HashMap;
import java.util.Map;

import de.psi.telco.sumoplayer.util.Point;
import de.psi.telco.sumoplayer.util.PointImpl;
import de.psi.telco.sumoplayer.util.Vector;

/**
 * This file represents a SUMO network and is read by SUMOSAXNetworkfileHandler
 * 
 * @author will
 *
 */
public class SUMONetwork {
	
	public Map<String, SUMOLane> lanes = new HashMap<String,SUMOLane>();
	public String projString = null;
	public Vector offset = new PointImpl(0,0);
	private Map<String,Point> junctions = new HashMap<String,Point>();
	
	public void addJunction(String id, double x, double y){
		junctions.put(id, new PointImpl(x,y));
	}
	
	public Point getJunction(String id){
		return this.junctions.get(id);
	}
	
}
