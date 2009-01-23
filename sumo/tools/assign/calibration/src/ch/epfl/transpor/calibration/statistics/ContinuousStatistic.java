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

import ch.epfl.transpor.calibration.demand.Plan;

/**
 * 
 * @author Gunnar Flötteröd
 * 
 */
public class ContinuousStatistic<L> extends Statistic<L> {

	// -------------------- MEMBERS --------------------

	private double valSum = 0;

	private double valSqSum = 0;

	// -------------------- CONSTRUCTION --------------------

	public ContinuousStatistic(final String parameterLabel,
			final Collection<Object> agents) {
		super(parameterLabel, agents);
	}

	// -------------------- INTERFACE IMPLEMENTATION --------------------

	@Override
	protected boolean internalProcessPlan(final Plan<L> plan) {
		if (plan.containsContinuousParameter(this.getParameterLabel())) {
			final double value = plan.getContinuousParameter(this
					.getParameterLabel());
			this.valSum += value;
			this.valSqSum += value * value;
			return true;
		} else {
			return false;
		}
	}

	// -------------------- CONTENT ACCESS --------------------

	public double getAverage() {
		return this.valSum / this.getTotalCount();
	}

	public double getVariance() {
		final double cnt = this.getTotalCount();
		return this.valSqSum / (cnt - 1.0) - this.valSum * this.valSum
				/ (cnt - 1.0) / cnt;
	}
}
