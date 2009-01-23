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

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Random;

import ch.epfl.transpor.calibration.Calibrator;
import ch.epfl.transpor.calibration.demand.PlanStep;
import ch.epfl.transpor.calibration.supply.SimResultsContainer;

/**
 * 
 * @author Gunnar Flötteröd
 *
 */
public class SumoCalibrator extends Calibrator<String, Object> implements
		Serializable {

	// -------------------- SERIALIZABLE IMPLEMENTATION --------------------

	private static final long serialVersionUID = 1L;

	/**
	 * bin size for network conditions.
	 */
	private int binSize_s;

	public void setBinSize_s(final int binSize_s) {
		if ((24 * 3600) % binSize_s != 0) {
			throw new IllegalArgumentException("One day cannot be divided in "
					+ "an integer number of bins of size " + binSize_s + " s.");
		}
		this.binSize_s = binSize_s;
	}

	public int getBinSize_s() {
		return binSize_s;
	}

	/**
	 * empty constructor, only for serialization
	 */
	public SumoCalibrator() {
		this(null, 1, 24 * 3600);
	}

	// -------------------- CONSTRUCTION & INITIALIZATION --------------------

	SumoCalibrator(final Random rnd, final double lambda, final int binSize_s) {
		super(rnd, lambda);
		this.setBinSize_s(binSize_s);
	}

	void loadMeasurements(final String measFile, final double stddev_veh_h) {
		SumoFlowLoader.loadMeasurements(measFile, this, stddev_veh_h, this
				.getBinSize_s());
	}

	// -------------------- CHOICE IMPLEMENTATION --------------------

	static void writeAttr(final String name, final String value,
			final BufferedWriter writer) throws IOException {
		writer.write(name);
		writer.write("=\"");
		writer.write(value);
		writer.write("\" ");
	}

	static void writeAttr(final String name, final int value,
			final BufferedWriter writer) throws IOException {
		writeAttr(name, Integer.toString(value), writer);
	}

	static void writeAttr(final String name, final double value,
			final BufferedWriter writer) throws IOException {
		writeAttr(name, Double.toString(value), writer);
	}

	void choice(final String popFile, final String choiceFile)
			throws IOException {

		final SumoDemandLoader demandLoader = new SumoDemandLoader();
		final SumoPopulation population = demandLoader.load(popFile);
		final BufferedWriter writer = new BufferedWriter(new FileWriter(
				choiceFile));

		writer.write("<route-alternatives>");
		writer.newLine();

		for (SumoAgent agent : population.getAgents()) {

			final SumoPlan choice = this.selectPlan(agent);
			this.addToDemand(choice.getPlan());

			final StringBuffer edges = new StringBuffer();
			final StringBuffer exits = new StringBuffer();
			final Iterator<PlanStep<String>> it = choice.getPlan().iterator();
			if (it.hasNext()) {
				it.next();
				while (it.hasNext()) {
					final PlanStep<String> step = it.next();
					edges.append(step.getFrom());
					edges.append(" ");
					exits.append(step.getTime_s());
					exits.append(" ");
				}
			}

			writer.write("  <");
			writer.write(SumoDemandLoader.VEHICLE_ELEM);
			writer.write(" ");
			writeAttr(SumoDemandLoader.VEHICLEID_ATTR, agent.getId(), writer);
			writeAttr(SumoDemandLoader.DEPART_ATTR, agent.getDptTime_s(),
					writer);
			writer.write(">");
			writer.newLine();

			writer.write("    <");
			writer.write(SumoDemandLoader.ROUTE_ELEM);
			writer.write(" ");
//			SumoCalibrator.writeAttr(SumoDemandLoader.ROUTEID_ATTR, choice
//					.getId(), writer);
			SumoCalibrator.writeAttr(SumoDemandLoader.EDGES_ATTR, edges
					.toString(), writer);
			SumoCalibrator.writeAttr(SumoDemandLoader.EXITTIMES_ATTR, exits
					.toString(), writer);
			SumoCalibrator.writeAttr(SumoDemandLoader.CHOICEPROB_ATTR, "1",
					writer);
			writer.write("/>");
			writer.newLine();

			writer.write("  </");
			writer.write(SumoDemandLoader.VEHICLE_ELEM);
			writer.write(">");
			writer.newLine();
		}

		writer.write("</route-alternatives>");
		writer.newLine();

		writer.flush();
		writer.close();
	}

	private SumoPlan selectPlan(final SumoAgent agent) {
		final List<SumoPlan> plans = agent.getPlans();
		if (plans == null || plans.size() == 0) {
			this.printMessage("Agent " + agent.getId()
					+ " has an empty choice set.");
			return null;
		}

		final List<Double> scaledProbs = new ArrayList<Double>(plans.size());
		double scaledProbSum = 0;
		for (SumoPlan plan : plans) {
			final double scaledProb = plan.getProb()
					* Math.exp(this.calcLinearPlanEffect(plan.getPlan()));
			scaledProbs.add(scaledProb);
			scaledProbSum += scaledProb;
		}

		final double x = super.getRandom().nextDouble() * scaledProbSum;
		scaledProbSum = 0;
		for (int i = 0; i < plans.size(); i++) {
			scaledProbSum += scaledProbs.get(i);
			if (x <= scaledProbSum) {
				return plans.get(i);
			}
		}
		return plans.get(plans.size() - 1);
	}

	// -------------------- UPDATE IMPLEMENTATION --------------------

	void update(final String netCondFile) {
		final SimResultsContainer<String> simResults = SumoFlowLoader
				.loadSimResults(netCondFile, this.binSize_s);
		this.afterNetworkLoading(simResults);
	}
}