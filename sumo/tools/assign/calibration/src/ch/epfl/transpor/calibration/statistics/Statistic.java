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
import java.util.HashSet;
import java.util.Set;

import ch.epfl.transpor.calibration.demand.Plan;

/**
 * 
 * @author Gunnar Flötteröd
 * 
 */
public abstract class Statistic<L> {

	// -------------------- MEMBER VARIABLES --------------------

	private final String paramLabel;
	private final Set<Object> agents;
	private long totalCount = 0;

	// -------------------- CONSTRUCTION --------------------

	protected Statistic(final String parameterLabel, final Collection<Object> agents) {
		this.paramLabel = parameterLabel;
		this.agents = Collections.unmodifiableSet(new HashSet<Object>(agents));
	}

	// -------------------- INTERFACE SPECIFICATION --------------------

	abstract protected boolean internalProcessPlan(final Plan<L> plan);

	// -------------------- CONTENT MODIFICATION --------------------

	boolean registerPlan(final Object agent, final Plan<L> plan) {
		if (this.agents.contains(agent)) {
			final boolean processed = this.internalProcessPlan(plan);
			if (processed) {
				this.totalCount++;
			}
			return processed;
		} else {
			return false;
		}
	}

	// -------------------- CONTENT ACCESS --------------------

	public String getParameterLabel() {
		return this.paramLabel;
	}

	public long getTotalCount() {
		return this.totalCount;
	}

	public Set<Object> getAgents() {
		return this.agents;
	}

}
