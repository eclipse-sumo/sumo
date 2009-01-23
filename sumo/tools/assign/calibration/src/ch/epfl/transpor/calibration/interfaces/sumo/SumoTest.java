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
import java.util.Iterator;
import java.util.Random;

import ch.epfl.transpor.calibration.demand.PlanStep;

/**
 * 
 * @author Gunnar Flötteröd
 *
 */
public class SumoTest {

	static final int N = 100;

	// INITIALIZATION PARAMETERS

	static final String MEAS_FILE = "testdata/sumo/meas.xml";
	static final String STDDEV_VEH_H = "100";
	static final String RANDOM_SEED = "071276";
	static final String LAMBDA = "0.95";
	static final String PREP_ITS = "5";
	static final String FREEZE_ITS = "20";
	static final String BINSIZE_S = "300";
	static final String[] INIT_PARAMS = new String[] { SumoControler.INIT,
			MEAS_FILE, STDDEV_VEH_H, RANDOM_SEED, LAMBDA, PREP_ITS, FREEZE_ITS,
			BINSIZE_S };

	// CHOICE PARAMETERS

	static final String CHOICE_SET_FILE = "testdata/sumo/choiceset.xml";
	static final String CHOICE_FILE = "testdata/sumo/choices.xml";
	static final String[] CHOICE_PARAMS = new String[] { SumoControler.CHOICE,
			CHOICE_SET_FILE, CHOICE_FILE };

	// UPDATE PARAMETERS

	static final String SIM_RESULTS_FILE = "testdata/sumo/simresults.xml";
	static final String[] UPDATE_PARAMS = new String[] { SumoControler.UPDATE,
			SIM_RESULTS_FILE };

	// TEST IMPLEMENTATION

	static void writeChoiceSet() throws IOException {
		final Random rnd = new Random();
		final BufferedWriter writer = new BufferedWriter(new FileWriter(
				CHOICE_SET_FILE));

		writer.write("<route-alternatives>");
		writer.newLine();

		for (int n = 0; n < N; n++) {
			final double startTime_s = rnd.nextInt(300);

			writer.write("  <");
			writer.write(SumoDemandLoader.VEHICLE_ELEM);
			writer.write(" ");
			SumoCalibrator.writeAttr(SumoDemandLoader.VEHICLEID_ATTR, Integer
					.toString(n), writer);
			SumoCalibrator.writeAttr(SumoDemandLoader.DEPART_ATTR, Double
					.toString(startTime_s), writer);
			writer.write(">");
			writer.newLine();

			// TODO this is new: empty alternative!
			writer.write("    <");
			writer.write(SumoDemandLoader.ROUTE_ELEM);
			writer.write(" ");
			SumoCalibrator
					.writeAttr(SumoDemandLoader.ROUTEID_ATTR, "0", writer);
			SumoCalibrator.writeAttr(SumoDemandLoader.EDGES_ATTR, "", writer);
			SumoCalibrator.writeAttr(SumoDemandLoader.EXITTIMES_ATTR, "",
					writer);
			SumoCalibrator.writeAttr(SumoDemandLoader.CHOICEPROB_ATTR, "0.34",
					writer);
			writer.write("/>");
			writer.newLine();

			writer.write("    <");
			writer.write(SumoDemandLoader.ROUTE_ELEM);
			writer.write(" ");
			SumoCalibrator
					.writeAttr(SumoDemandLoader.ROUTEID_ATTR, "1", writer);
			SumoCalibrator
					.writeAttr(SumoDemandLoader.EDGES_ATTR, "0 1", writer);
			SumoCalibrator.writeAttr(SumoDemandLoader.EXITTIMES_ATTR,
					(startTime_s + 60) + " " + (startTime_s + 120), writer);
			SumoCalibrator.writeAttr(SumoDemandLoader.CHOICEPROB_ATTR, "0.33",
					writer);
			writer.write("/>");
			writer.newLine();

			writer.write("    <");
			writer.write(SumoDemandLoader.ROUTE_ELEM);
			writer.write(" ");
			SumoCalibrator
					.writeAttr(SumoDemandLoader.ROUTEID_ATTR, "2", writer);
			SumoCalibrator
					.writeAttr(SumoDemandLoader.EDGES_ATTR, "0 2", writer);
			SumoCalibrator.writeAttr(SumoDemandLoader.EXITTIMES_ATTR,
					(startTime_s + 60) + " " + (startTime_s + 120), writer);
			SumoCalibrator.writeAttr(SumoDemandLoader.CHOICEPROB_ATTR, "0.33",
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

	static void writeNetworkConditions() throws IOException {
		final SumoDemandLoader loader = new SumoDemandLoader();
		final SumoPopulation demand = loader.load(CHOICE_FILE);

		int cnt1 = 0;
		int cnt2 = 0;
		for (SumoAgent agent : demand.getAgents()) {
			final Iterator<PlanStep<String>> it = agent.getPlans().get(0)
					.getPlan().iterator();
			if (it.hasNext()) {
				it.next();
				final PlanStep<String> step = it.next();
				if ("1".equals(step.getTo())) {
					cnt1++;
				} else if ("2".equals(step.getTo())) {
					cnt2++;
				}
			}
		}

		final BufferedWriter writer = new BufferedWriter(new FileWriter(
				SIM_RESULTS_FILE));

		// TODO use constants from SumoFlowLoader here

		writer.append("<netstats>");
		writer.newLine();

		writer.append("  <");
		writer.append(SumoFlowLoader.INTERVAL_ELEM);
		writer.append(" ");
		SumoCalibrator.writeAttr(SumoFlowLoader.START_ATTR, "0", writer);
		SumoCalibrator.writeAttr(SumoFlowLoader.END_ATTR, "299", writer);
		writer.append(">");
		writer.newLine();

		writer.append("    <");
		writer.append(SumoFlowLoader.EDGE_ELEM);
		writer.append(" ");
		SumoCalibrator.writeAttr(SumoFlowLoader.ID_ATTR, "1", writer);
		SumoCalibrator.writeAttr(SumoFlowLoader.ENTERED_ATTR, cnt1, writer);
		writer.append("/>");
		writer.newLine();

		writer.append("    <");
		writer.append(SumoFlowLoader.EDGE_ELEM);
		writer.append(" ");
		SumoCalibrator.writeAttr(SumoFlowLoader.ID_ATTR, "2", writer);
		SumoCalibrator.writeAttr(SumoFlowLoader.ENTERED_ATTR, cnt2, writer);
		writer.append("/>");
		writer.newLine();

		writer.append("  </");
		writer.append(SumoFlowLoader.INTERVAL_ELEM);
		writer.append(">");
		writer.newLine();

		writer.append("</netstats>");
		writer.newLine();

		writer.flush();
		writer.close();
	}

	public static void main(String[] args) throws IOException {

		// System.out.println("RUNNING INIT");
		SumoControler.main(INIT_PARAMS);

		for (int it = 0; it < 50; it++) {

			// System.out.println("RUNNING CHOICE #" + (it + 1));
			writeChoiceSet();
			SumoControler.main(CHOICE_PARAMS);

			// System.out.println("RUNNING UPDATE #" + (it + 1));
			writeNetworkConditions();
			SumoControler.main(UPDATE_PARAMS);
		}

	}

}
