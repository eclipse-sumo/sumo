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
package ch.epfl.transpor.calibration.demand;

import java.io.Serializable;

import ch.epfl.transpor.calibration.supply.NetworkData;

/**
 * 
 * @author Gunnar Flötteröd
 *
 * @param <L>
 */
public class Demand<L> extends NetworkData<L> implements Serializable {

	// -------------------- SERIALIZABLE IMPLEMENTATION --------------------

	private static final long serialVersionUID = 1L;

	/**
	 * empty constructor, only for serialization
	 */
	public Demand() {
		this(0, 0, 0);
	}

	// -------------------- CONSTRUCTION --------------------

	public Demand(int startTime_s, int timePeriod_s, int binCnt) {
		super(startTime_s, timePeriod_s, binCnt);
	}

	// -------------------- IMPLEMENTATION --------------------

	public boolean add(PlanStep<L> planStep) {

		// sensors are assumed to be located at the upstream end of link l
		// entry moves (null -> l) happen downstream of that
		// exit moves (other link -> null) cannot enter l != null
		if (planStep.getFrom() == null || planStep.getTo() == null) {
			return false;
		}

		final int bin = bin(planStep.getTime_s());
		if (bin < 0 || bin >= this.getBinCnt())
			return false;

		final L toLink = planStep.getTo();
		double[] demandSequence = this.getData().get(toLink);
		if (demandSequence == null) {
			demandSequence = new double[this.getBinCnt()];
			this.getData().put(toLink, demandSequence);
		}
		demandSequence[bin]++;
		return true;
	}
}

// ------------------------------------------------------------
// ------------------------------------------------------------
// ------------------------------------------------------------
//
// private final int startTime_s;
// private final int timePeriod_s;
// private final int binCnt;
// private final Map<L, int[]> demand = new HashMap<L, int[]>();
// private int bin(int time_s) {
// return (time_s - this.startTime_s) / this.timePeriod_s;
// }
// private int binStart_s(int bin) {
// return this.startTime_s + bin * this.timePeriod_s;
// }
// private int binEnd_s(int bin) {
// return binStart_s(bin + 1) - 1;
// }
// public void clear() {
// this.demand.clear();
// }
