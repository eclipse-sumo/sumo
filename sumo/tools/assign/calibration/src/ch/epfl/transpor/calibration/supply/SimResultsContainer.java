/*
 * Do not distribute this software. It is property of
 * 
 *   Gunnar Flötteröd
 *   Transport and Mobility Laboratory
 *   Ecole Polytechnique Fédérale de Lausanne
 *
 * Please mention explicitly the use of this software when publishing results.
 *
 * This software is provided WITHOUT ANY WARRANTY; without even the implied 
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. Nor myself
 * neither my employer (EPFL) shall be liable for any damage of any kind in 
 * connection with the use of this software.
 * 
 */
package ch.epfl.transpor.calibration.supply;

/**
 * 
 * Specifies a container that provides access to the network conditions that
 * result from a network loading.
 * 
 * @author Gunnar Flötteröd
 * 
 */
public interface SimResultsContainer<L> {

	/**
	 * @param link
	 *            the link for which the average flow is requested
	 * @param startTime_s
	 *            the start time of the considered time period (in seconds after
	 *            midnight)
	 * @param endTime_s
	 *            the end time of the considered time period (in seconds after
	 *            midnight)
	 * @return the average traffic flow in vehicles per hour that occurred in
	 *         the according network loading in the time interval [startTime_s,
	 *         endTime_s]
	 */
	public double getAvgFlow_veh_h(L link, int startTime_s, int endTime_s);

}
