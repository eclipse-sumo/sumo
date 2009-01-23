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

import java.util.Random;

import ch.epfl.transpor.calibration.SamplingCalibrator;
import ch.epfl.transpor.calibration.demand.Plan;

/**
 * 
 * @author Gunnar Flötteröd
 *
 * @param <L>
 */
public class MatsimCalibrator<L> extends
		SamplingCalibrator<L, MatsimSampler<L>> {

	// -------------------- CONSTRUCTION --------------------

	/**
	 * Adds nothing new to the according Calibrator interface.
	 */
	public MatsimCalibrator(Random rnd, double lambda) {
		super(rnd, lambda);
	}

	/**
	 * Adds nothing to the according Calibrator interface.
	 */
	public MatsimCalibrator() {
		super();
	}

	// -------------------- IMPLEMENTATION OF Calibrator --------------------

	protected MatsimSampler<L> newSampler() {
		return new MatsimSampler<L>(this);
	}

	// -------------------- OVERRIDING OF Calibrator --------------------

//	@Override
	// to make this package visible
//	protected Random getRandom() {
//		return super.getRandom();
//	}

	@Override
	// to make this package visible
	protected void addToDemand(Plan<L> plan) {
		super.addToDemand(plan);
	}

	@Override
	// to make this package visible
	protected double calcLinearPlanEffect(final Plan<L> plan) {
		return super.calcLinearPlanEffect(plan);
	}

}
