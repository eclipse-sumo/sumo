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
	public Vehicle(String id, float x, float y, float time_first, float speed) {
		this.id = id;
		this.x = x;
		this.y = y;
		this.curr_x = x;
		this.curr_y = y;
		this.time_first = time_first;
		this.time_last = time_first;
		this.speed = speed;
	}

	public float getSpeed() {
		return this.speed;
	}
	
	public void setSpeed(float speed) {
		this.speed = speed;
	}
	
	public float getX() {
		return this.curr_x;
	}
	
	public float getY() {
		return this.curr_y;
	}
	
	public void setX(float x, float time) {
		this.curr_x = x;
		if (time < 0) {
			this.x = x;
		}
	}

	public void setY(float y, float time) {
		this.curr_y = y;
		if (time < 0) {
			this.y = y;
		}
	}
	
	public float getInitialX() {
		return this.x;
	}
	
	public float getInitialY() {
		return this.y;
	}
	
	public float getStartTime() {
		return time_first;
	}
	
	public float getStopTime() {
		return time_last;
	}
	
	public void setTime(float time) {
		time_last = time;
	}
	
	public String id;

	/** x inital x-position */
	public float x;

	/** y inital y-position */
	public float y;

	/** curr_x current x-position */
	public float curr_x;
	
	/** curr_y current y-position */
	public float curr_y;
	
	/** time_first time of node creation >= 0*/
	public float time_first;

	/** time_first time of node leave */
	public float time_last;
	
	protected float speed;
}
