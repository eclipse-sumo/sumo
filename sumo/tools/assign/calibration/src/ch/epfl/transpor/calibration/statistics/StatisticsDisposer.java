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

import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import ch.epfl.transpor.calibration.demand.Plan;

/**
 * 
 * @author Gunnar Flötteröd
 * 
 */
public class StatisticsDisposer<L> {

	// -------------------- MEMBERS --------------------

	private final int maxAgentsForIndStat;

	private final List<Statistic<L>> gblStats = new ArrayList<Statistic<L>>();

	// concrete List implementation makes ArrayList.trimToSize() accessible
	private final Map<Object, ArrayList<Statistic<L>>> indStats = new LinkedHashMap<Object, ArrayList<Statistic<L>>>();

	// -------------------- CONSTRUCTION --------------------

	public StatisticsDisposer(final int maxAgentsForIndStat) {
		if (maxAgentsForIndStat < 0) {
			throw new IllegalArgumentException(
					"maximum agent count for individual statistic cannot be negative");
		}
		this.maxAgentsForIndStat = maxAgentsForIndStat;
	}

	// -------------------- CONTENT MODIFICATION --------------------

	public void addStatistic(final Statistic<L> statistic) {
		if (statistic == null) {
			throw new IllegalArgumentException("statistics reference is null");
		}
		if (statistic.getAgents().size() <= this.maxAgentsForIndStat) {
			for (Object agent : statistic.getAgents()) {
				ArrayList<Statistic<L>> indStatList = this.indStats.get(agent);
				if (indStatList == null) {
					indStatList = new ArrayList<Statistic<L>>(1);
					this.indStats.put(agent, indStatList);
				}
				if (!indStatList.contains(statistic)) {
					indStatList.add(statistic);
					indStatList.trimToSize();
				}
			}
		} else {
			if (!this.gblStats.contains(statistic)) {
				this.gblStats.add(statistic);
			}
		}
	}

	public void registerPlan(final Object agent, final Plan<L> plan) {
		for (Statistic<L> stat : this.gblStats) {
			stat.registerPlan(agent, plan);
		}
		final List<Statistic<L>> statistics = this.indStats.get(agent);
		if (statistics != null) {
			for (Statistic<L> stat : statistics) {
				stat.registerPlan(agent, plan);
			}
		}
	}

	// -------------------- CONTENT ACCESS --------------------

	// TODO

}
