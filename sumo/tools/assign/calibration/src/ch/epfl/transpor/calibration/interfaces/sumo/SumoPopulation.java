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
public class SumoPopulation {

	// -------------------- MEMBERS --------------------

	private List<SumoAgent> agents = new ArrayList<SumoAgent>();

	// -------------------- CONSTRUCTION --------------------

	SumoPopulation() {
	}

	// -------------------- CONTENT ACCESS --------------------

	void addAgent(final SumoAgent agent) {
		this.agents.add(agent);
	}

	List<SumoAgent> getAgents() {
		return this.agents;
	}

	public String toString() {
		final StringBuffer result = new StringBuffer();
		for (SumoAgent agent : this.agents) {
			result.append(agent.toString());
			result.append("\n");
		}
		return result.toString();
	}
}