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
package ch.epfl.transpor.calibration.interfaces.sumo;

import ch.epfl.transpor.calibration.demand.Plan;
import ch.epfl.transpor.calibration.demand.PlanStep;

/**
 * 
 * @author Gunnar Flötteröd
 *
 */
public class SumoPlan {

	// -------------------- MEMBERS --------------------

	private final String id;

	private final Plan<String> plan;

	private final double prob;

	// -------------------- CONSTRUCTION --------------------

	SumoPlan(final String id, final Plan<String> plan, final double prob) {
		this.id = id;
		this.plan = plan;
		this.prob = prob;
	}

	// -------------------- CONTENT ACCESS --------------------

	String getId() {
		return this.id;
	}

	Plan<String> getPlan() {
		return this.plan;
	}

	double getProb() {
		return this.prob;
	}

	public String toString() {
		final StringBuffer result = new StringBuffer();
		result.append("plan(id=");
		result.append(this.id);
		result.append(",links=<");
		for (PlanStep<String> step : this.plan) {
			final String from = step.getFrom();
			final String to = step.getTo();
			if (to != null) {
				if (from != null) {
					result.append(",");
				}
				result.append(to);
				result.append("[");
				result.append(step.getTime_s());
				result.append("s]");
			}
		}
		result.append(">,prob=");
		result.append(this.prob);
		result.append(")");
		return result.toString();
	}
}