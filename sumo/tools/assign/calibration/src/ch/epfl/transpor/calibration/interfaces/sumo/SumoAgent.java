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

import java.util.ArrayList;
import java.util.List;

/**
 * 
 * @author Gunnar Flötteröd
 *
 */
public class SumoAgent {

	// -------------------- MEMBERS --------------------

	private final String id;

	private final int dpt_s;

	private final List<SumoPlan> plans = new ArrayList<SumoPlan>();

	// -------------------- CONSTRUCTION --------------------

	SumoAgent(final String id, final int dpt_s) {
		this.id = id;
		this.dpt_s = dpt_s;
	}

	// -------------------- CONTENT ACCESS --------------------

	void addSumoPlan(final SumoPlan plan) {
		this.plans.add(plan);
	}

	String getId() {
		return this.id;
	}

	int getDptTime_s() {
		return this.dpt_s;
	}

	List<SumoPlan> getPlans() {
		return this.plans;
	}

	public String toString() {
		final StringBuffer result = new StringBuffer();
		result.append("agent(id=");
		result.append(this.id);
		result.append(",dpt=");
		result.append(this.dpt_s);
		result.append("s");
		for (SumoPlan plan : this.plans) {
			result.append(",");
			result.append(plan);
		}
		result.append(")");
		return result.toString();
	}
}