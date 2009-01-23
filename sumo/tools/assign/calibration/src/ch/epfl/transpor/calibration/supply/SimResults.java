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

import java.io.Serializable;

/**
 * 
 * @author Gunnar Flötteröd
 *
 * @param <L>
 */
public class SimResults<L> extends NetworkData<L> implements
		SimResultsContainer<L>, Serializable {

	// -------------------- SERIALIZABLE IMPLEMENTATION --------------------

	private static final long serialVersionUID = 1L;

	public SimResults() {
		this(0, 0, 0);
	}

	// -------------------- CONSTRUCTION --------------------

	public SimResults(final int startTime_s, final int timePeriod_s,
			final int binCnt) {
		super(startTime_s, timePeriod_s, binCnt);
	}

	// -------------------- INTERFACE IMPLEMENTATION --------------------

	@Override
	public double getAvgFlow_veh_h(L link, int startTime_s, int endTime_s) {
		final double flow_veh = this.get(link, startTime_s, endTime_s);
		final double dur_h = (endTime_s - startTime_s + 1.0) / 3600.0;
		return flow_veh / dur_h;
	}

}
