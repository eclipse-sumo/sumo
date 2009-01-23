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

/**
 * 
 * @author Gunnar Flötteröd
 *
 * @param <L>
 */
public class PlanStep<L> extends TurningMove<L> {

	// -------------------- MEMBERS --------------------

	private final int time_s;

	// -------------------- CONSTRUCTION --------------------

	public PlanStep(L from, L to, int time_s) {
		super(from, to);
		this.time_s = time_s;
	}

	// -------------------- SIMPLE FUNCTIONALITY --------------------

	public int getTime_s() {
		return this.time_s;
	}

	// -------------------- OVERRIDE TurningMove --------------------

	public boolean equals(Object other) {
		if (!super.equals(other) || other.getClass() != this.getClass())
			return false;
		return this.time_s == ((PlanStep<L>) other).time_s;
	}

	public int hashCode() {
		return 31 * super.hashCode() + this.time_s;
	}

}
