package de.psi.telco.sumoplayer.util;

/**
 * Interfacing a generic Line containing 2 Points 
 * 
 * @author will
 *
 */
public interface Line {

	/**
	 * Returns the starting Point of this Line
	 * @return
	 */
	public abstract Point getA();

	/**
	 * Returns the ending Point of this Line
	 * @return
	 */
	public abstract Point getB();

}