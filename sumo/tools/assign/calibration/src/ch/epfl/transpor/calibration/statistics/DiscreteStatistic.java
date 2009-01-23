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
package ch.epfl.transpor.calibration.statistics;

import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.Map;

import ch.epfl.transpor.calibration.demand.Plan;

/**
 * 
 * @author Gunnar Flötteröd
 * 
 */
public class DiscreteStatistic<L> extends Statistic<L> {

	// -------------------- MEMBERS --------------------

	private final Map<Object, Long> counts = new HashMap<Object, Long>();

	// -------------------- CONSTRUCTION --------------------

	public DiscreteStatistic(final String parameterLabel,
			final Collection<Object> agents) {
		super(parameterLabel, agents);
	}

	// -------------------- INTERFACE IMPLEMENTATION --------------------

	@Override
	protected boolean internalProcessPlan(final Plan<L> plan) {
		if (plan.containsDiscreteParameter(this.getParameterLabel())) {
			final Object param = plan.getDiscreteParameter(this
					.getParameterLabel());
			final Long cnt = this.counts.get(param);
			this.counts.put(param, (cnt == null) ? 1 : cnt + 1);
			return true;
		} else {
			return false;
		}
	}

	// -------------------- CONTENT ACCESS --------------------

	public Map<Object, Double> getHistogram() {
		final Map<Object, Double> result = new LinkedHashMap<Object, Double>();
		for (Map.Entry<Object, Long> entry : this.counts.entrySet()) {
			result.put(entry.getKey(), ((double) entry.getValue())
					/ this.getTotalCount());
		}
		return Collections.unmodifiableMap(result);
	}
}
