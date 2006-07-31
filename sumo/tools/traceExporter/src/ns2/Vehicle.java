/*
 * Vehicle.java
 *
 * Created on 14. Februar 2006, 14:17
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package ns2;

/**
 * model of vehicle
 * @author Thimor Bohn <bohn@itm.uni-luebeck.de>
 */
public class Vehicle {

	/**
	 * constructor
	 * @param id
	 * @param x
	 * @param y
	 * @param time_first
	 */
	public Vehicle(String id, float x, float y, float time_first) {
		this.id = id;
		this.x = x;
		this.y = y;
		this.time_first = time_first;
		this.time_last = time_first;
	}

	public String id;

	public float x;

	public float y;

	public float time_first;

	public float time_last;
}
