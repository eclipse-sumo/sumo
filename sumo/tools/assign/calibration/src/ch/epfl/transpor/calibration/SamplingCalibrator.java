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
package ch.epfl.transpor.calibration;

import java.util.HashMap;
import java.util.Map;
import java.util.Random;


/**
 * 
 * This is the basic calibration class. It is abstract because the most
 * efficient mechanics of plan selection are likely to differ among DTA
 * simulators. Consequently, a subclass that suits the particular application's
 * needs should be instantiated. The set of suchlike subclasses is expected to
 * grow with the different applications of this tool.
 * 
 * @param L
 *            the network link type
 * @param S
 *            the type of the concrete individual-level plan sampler
 * 
 * @author Gunnar Flötteröd
 * 
 */
public abstract class SamplingCalibrator<L, S> extends Calibrator<L, S> {

	// -------------------- MEMBERS --------------------

	// CONSTANTS

	private final Map<Object, S> samplers;

	private int maxDraws = 20;

	// -------------------- CONSTRUCTION --------------------

	/**
	 * Standard constructor. Instantiate one Calibrator subclass for an entire
	 * calibration process.
	 * 
	 * @param rnd
	 *            provides control over reproducible random number generation
	 * @param lambda
	 *            forgetting factor of the recursive regression log-likelihood
	 *            approximation
	 * 
	 */
	public SamplingCalibrator(final Random rnd, final double lambda) {
		super(rnd, lambda);
		this.samplers = new HashMap<Object, S>();
		this.setIteration(0);
		// this.iteration = 0;
	}

	/**
	 * Same as the standard constructor with a newly generated Random instance
	 * and a lambda forgetting factor of 0.95.
	 */
	public SamplingCalibrator() {
		this(new Random(), 0.95);
	}

	// -------------------- SIMPLE FUNCTIONALITY --------------------

	/**
	 * @param maxDraws
	 *            the maximum number of draws that is requested from a
	 *            replanning agent until a plan is accepted. Must be at least 3.
	 */
	public void setMaxDraws(final int maxDraws) {
		if (maxDraws < 3) {
			throw new IllegalArgumentException(
					"maximum number of draws must be at least 3");
		}
		this.maxDraws = maxDraws;
	}

	public int getMaxDraws() {
		return this.maxDraws;
	}

	/**
	 * @return subclasses should return an instance of the particular sampler
	 *         type S that is compatible with the considered demand simulator
	 */
	protected abstract S newSampler();

	// -------------------- CALIBRATION LOGIC INTERFACE --------------------

	/**
	 * 
	 * @param agent
	 *            the agent reference for which replanning is to be conducted
	 * @return a sampler instance that is specified by the concrete subclass of
	 *         this Calibrator
	 */
	public S getSampler(final Object agent) {
		// CHECK
		if (agent == null)
			throw new IllegalArgumentException(
					"reference agent must not be null;");
		// CONTINUE
		S sampler = this.samplers.get(agent);
		if (sampler == null) {
			sampler = this.newSampler();
			this.samplers.put(agent, sampler);
		}
		return sampler;
	}
}
