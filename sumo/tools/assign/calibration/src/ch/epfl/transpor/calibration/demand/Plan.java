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

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

/**
 * 
 * @author Gunnar Flötteröd
 *
 * @param <L>
 */
public class Plan<L> implements Iterable<PlanStep<L>> {

	// -------------------- MEMBERS --------------------

	private Map<String, Object> discrParams;

	private Map<String, Double> contParams;

	private final List<PlanStep<L>> planSteps = new ArrayList<PlanStep<L>>();

	// -------------------- CONSTRUCTION --------------------

	Plan() {
	}

	// -------------------- CONTENT MODIFICATIONS --------------------

	void addDiscreteParameter(final String label, final Object value) {
		if (this.discrParams == null) {
			this.discrParams = new HashMap<String, Object>();
		}
		this.discrParams.put(label, value);
	}

	void addContinuousParameter(final String label, final double value) {
		if (this.contParams == null) {
			this.contParams = new HashMap<String, Double>();
		}
		this.contParams.put(label, value);
	}

	void addStep(final PlanStep<L> step) {
		this.planSteps.add(step);
	}

	// -------------------- CONTENT ACCESS --------------------

	public boolean containsDiscreteParameter(final String label) {
		return (this.discrParams != null && this.discrParams.containsKey(label));
	}

	public boolean containsContinuousParameter(final String label) {
		return (this.contParams != null && this.contParams.containsKey(label));
	}

	public Object getDiscreteParameter(final String label) {
		if (this.discrParams == null) {
			return null;
		} else {
			return this.discrParams.get(label);
		}
	}

	public Double getContinuousParameter(final String label) {
		if (this.contParams == null) {
			return null;
		} else {
			return this.contParams.get(label);
		}
	}

	Iterable<PlanStep<L>> getPlanSteps() {
		return this.planSteps;
	}

	// -------------------- IMPLEMENTATION OF Iterable --------------------

	public Iterator<PlanStep<L>> iterator() {
		return this.planSteps.iterator();
	}

}
