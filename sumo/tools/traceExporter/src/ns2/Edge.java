/*
 * Edge.java
 *
 * Created on 14. Februar 2006, 10:13
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package ns2;

import java.util.HashMap;
import java.util.Map;

/**
 * model for edges
 * @author Thimor Bohn <bohn@itm.uni-luebeck.de>
 */
public class Edge {
	public String id;
	public float length;
	public float speed;
	public String name;
	public float xfrom;
	public float yfrom;
	public float xto;
	public float yto;
	public Map<String, Lane> lanes;

	/**
	 * constructor
	 * @param id unique id of edge
	 * @param length length of edge in m
	 * @param speed allowed speed of vehicles on edge in m/s 
	 * @param name name of edge
	 * @param xfrom x-coordinate at which vehicles start on the edge
	 * @param yfrom y-coordinate at which vehicles start on the edge
	 * @param xto x-coordinate at which vehicles leave the edge
	 * @param yto y-coordinate at which vehicles leave the edge
	 */
	public Edge(String id, float length, float speed, String name, float xfrom,
			float yfrom, float xto, float yto) {
		this.id     = id;
		this.length = length;
		this.speed  = speed;
		this.name   = name;
		this.xfrom  = xfrom;
		this.yfrom  = yfrom;
		this.xto    = xto;
		this.yto    = yto;
		this.lanes  = new HashMap<String, Lane>();
	}
}
