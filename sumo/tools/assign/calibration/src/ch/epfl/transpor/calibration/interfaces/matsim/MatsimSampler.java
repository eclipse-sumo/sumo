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

package ch.epfl.transpor.calibration.interfaces.matsim;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import ch.epfl.transpor.calibration.demand.Plan;

/**
 * Implements a scaling of the prior plan choice distribution that is based on
 * "progressive sampling importance resampling (progressive SIR)". While SIR is
 * a well-known procedure to draw from a scaled distribution, the "progressive"
 * aspect is an add-on that allows to approximately draw in a SIR-like manner
 * while at the same time not needing to memorize more than one draw.
 * 
 * @author Gunnar Flötteröd
 * 
 */
public class MatsimSampler<L> {

	// -------------------- MEMBERS --------------------

	// CONSTANTS

	private final Map<Plan<L>, Double> plan2linEffect = new HashMap<Plan<L>, Double>();

	// TODO not necessary for a naive implementation
	private final List<Double> likelihoods = new LinkedList<Double>();

	private final MatsimCalibrator<L> calibrator;

	// RUNTIME PARAMETERS

	private int draws = 0;

	private double likelihoodSum = 0;

	// -------------------- CONSTRUCTION AND INITIALIZATION --------------------

	MatsimSampler(final MatsimCalibrator<L> calibrator) {
		// CHECK
		if (calibrator == null)
			throw new IllegalArgumentException("calibrator is null");
		// CONTINUE
		this.calibrator = calibrator;
		this.init();
	}

	private void init() {
		this.plan2linEffect.clear();
		this.likelihoods.clear();
		this.draws = 0;
		this.likelihoodSum = 0;
	}

	// -------------------- ACCEPT/REJECT FUNCTIONALITY --------------------

	private double likelihood(final Plan<L> plan) {
		Double linEffect = this.plan2linEffect.get(plan);
		if (linEffect == null) {
			linEffect = this.calibrator.calcLinearPlanEffect(plan);
			this.plan2linEffect.put(plan, linEffect);
		}
		return Math.exp(linEffect);
	}

	/**
	 * Whenever an agent is about to make a choice, draw plans according to this
	 * agent's behavioral model (i.e., this agents prior choice distribution)
	 * until this function returns an "accept". The first accepted plan can be
	 * considered a draw from the agent's behavioral posterior distribution.
	 * <em>It is of greatest importance that the
	 * agent does indeed implement the first accepted plan!</em>
	 * 
	 * @param plan
	 *            the plan under consideration, must be a draw from the
	 *            behavioral prior distribution
	 * @return if the plan is accepted
	 */
	public boolean isAccepted(final Plan<L> plan) {

		this.draws++;
		final double likelihood = this.likelihood(plan);

		final double pAccept;
		if (Double.isInfinite(likelihood)) {
			pAccept = 1.0;
			this.calibrator.printMessage("problem: infinite likelihood");
		} else {
			this.likelihoods.add(likelihood);
			this.likelihoodSum += likelihood;
			if (this.likelihoods.size() > this.calibrator.getMaxDraws()) {
				this.likelihoodSum -= this.likelihoods.remove(0);
				this.likelihoodSum = Math.max(0, this.likelihoodSum);
			}
			pAccept = likelihood
					/ (likelihood + (this.calibrator.getMaxDraws() - this.draws)
							* this.likelihoodSum / this.likelihoods.size());
		}

		// -------------------------------------------------------------------------
		// final boolean isAccepted = (this.draws == this.maxDraws || this.rnd
		// .nextDouble() < pAccept); // TODO original
		final boolean isAccepted = (this.draws > this.calibrator.getMaxDraws() / 2)
				&& (this.draws == this.calibrator.getMaxDraws() || this.calibrator
						.getRandom().nextDouble() < pAccept);
		// -------------------------------------------------------------------------

		if (isAccepted) {
			this.calibrator.addToDemand(plan);
			this.init();
		}
		return isAccepted;
	}
}

// -------------------- MAIN-FUNCTION, ONLY FOR TESTING --------------------

// public static void main(String[] args) {
// final int bins = 10;
// final int maxDraws = 20;
// final double[] cnt = new double[bins];
// final ProgressiveSIR<Double> sir = new ProgressiveSIR<Double>(
// new LikelihoodFunction<Double>() {
// public double likelihood(Double proposal) {
// return proposal;
// }
// }, new Random(), maxDraws);
// double sum = 0;
// double noDraws = 0;
// System.out.println("running...");
// for (int i = 0; i < 1000 * bins; i++) {
// sir.init();
// boolean accept = false;
// int draw = 0;
// while (!accept) {
// final double rnd = Math.random();
// accept = sir.accept(rnd);
// if (accept) {
// cnt[(int) (rnd * bins)] += 1.0 / (1000 * bins);
// sum += 1.0 / (1000 * bins);
// }
// draw++;
// }
// noDraws += draw / (1000.0 * bins * maxDraws);
// }
// System.out.println("draws " + noDraws);
// for (int bin = 0; bin < bins; bin++) {
// System.out.print(bin + "\t");
// for (int i = 0; i < cnt[bin] * 100; i++)
// System.out.print("#");
// System.out.println();
// }
// }
