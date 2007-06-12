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
	public String from;
	public String to;
	public Map<String, Lane> lanes;

	/**
	 * constructor
	 * @param id unique id of edge
	 * @param from junction at which edge begins
	 * @param to junction at which edge ends
	 */
	public Edge(String id, String from, String to)
	{
		this.id     = id;
		this.from   = from;
		this.to     = to;
		this.lanes  = new HashMap<String, Lane>();
	}
}
