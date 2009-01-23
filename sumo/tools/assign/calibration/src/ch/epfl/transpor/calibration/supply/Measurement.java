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
package ch.epfl.transpor.calibration.supply;

import java.io.Serializable;

import ch.epfl.transpor.calibration.demand.Demand;
import ch.epfl.transpor.calibration.demand.PlanStep;
import ch.epfl.transpor.calibration.math.Regression;
import ch.epfl.transpor.calibration.math.Vector;

/**
 * 
 * @author Gunnar Flötteröd
 * 
 */
public class Measurement<L> implements Serializable {

	// -------------------- CONSTANTS --------------------

	private static final int DEMAND_INDEX = 0;

	private static final int OFFSET_INDEX = 1;

	// -------------------- SERIALIZABLE IMPLEMENTATION --------------------

	private static final long serialVersionUID = 1L;

	/**
	 * the measured value
	 */
	private double meas_veh_h;

	public void setMeas_veh_h(double meas_veh_h) {
		this.meas_veh_h = meas_veh_h;
	}

	public double getMeas_veh_h() {
		return this.meas_veh_h;
	}

	/**
	 * the variance of this measurement
	 */
	private double var_veh2_h2;

	public void setVar_veh2_h2(double var_veh2_h2) {
		this.var_veh2_h2 = var_veh2_h2;
	}

	public double getVar_veh2_h2() {
		return this.var_veh2_h2;
	}

	/**
	 * the time at which this measurement starts
	 */
	private int startTime_s;

	public void setStartTime_s(int startTime_s) {
		this.startTime_s = startTime_s;
	}

	public int getStartTime_s() {
		return this.startTime_s;
	}

	/**
	 * the time at which this measurement ends
	 */
	private int endTime_s;

	public void setEndTime_s(int endTime_s) {
		this.endTime_s = endTime_s;
	}

	public int getEndTime_s() {
		return this.endTime_s;
	}

	/**
	 * the link this measurement refers to
	 */
	private L link;

	public void setLink(L link) {
		this.link = link;
	}

	public L getLink() {
		return this.link;
	}

	/**
	 * regression of log-likelihood against link demand
	 */
	private Regression regression;

	public void setRegression(Regression regression) {
		this.regression = regression;
	}

	public Regression getRegression() {
		return this.regression;
	}

	/**
	 * last log-likelihood prediction error
	 */
	private transient double lastLLPredErr = 0;

	/**
	 * last log-likelihood value
	 */
	private transient double lastLL = 0;

	/**
	 * buffer for the input data in the regression update
	 */
	private transient final Vector defaultInput = new Vector(2);

	/**
	 * empty constructor, only for serialization
	 */
	public Measurement() {
		this(0, 0, 0, 0, null, 1);
	}

	// -------------------- CONSTRUCTION --------------------

	Measurement(double meas_veh_h, double var_veh2_h2, int startTime_s,
			int endTime_s, L link, double lambda) {

		this.meas_veh_h = meas_veh_h;
		this.var_veh2_h2 = var_veh2_h2;
		this.startTime_s = startTime_s;
		this.endTime_s = endTime_s;
		this.link = link;

		this.regression = new Regression(lambda, 2);
	}

	// -------------------- SIMPLE FUNCTIONALITY --------------------

	public double getStdDev_veh_h() {
		return Math.sqrt(this.getVar_veh2_h2());
	}

	public void freeze() {
		this.regression.setLambda(1.0);
	}

	// -------------------- MEASUREMENT FUNCTIONALITY --------------------

	protected double ll(double sim_veh_h) {
		final double e_veh_h = sim_veh_h - this.getMeas_veh_h();
		// if (e_veh_h <= (-1.0) * this.getStdDev_veh_h()) {
		// return e_veh_h / this.getStdDev_veh_h() + 0.5;
		// } else if (e_veh_h >= this.getStdDev_veh_h()) {
		// return (-1.0) * e_veh_h / this.getStdDev_veh_h() + 0.5;
		// } else {
		return (-1.0) * e_veh_h * e_veh_h / 2.0 / this.getVar_veh2_h2();
		// }
	}

	private Vector newInput(double demand_veh) {
		this.defaultInput.set(DEMAND_INDEX, demand_veh);
		this.defaultInput.set(OFFSET_INDEX, 1.0);
		return this.defaultInput;
	}

	public void update(Demand<L> demand, double sim_veh_h) {
		final double demand_veh = demand.get(this.getLink(), this
				.getStartTime_s(), this.getEndTime_s());
		final Vector input = this.newInput(demand_veh);
		this.lastLL = this.ll(sim_veh_h);
		this.lastLLPredErr = this.regression.predict(input) - this.lastLL;
		this.regression.update(input, this.lastLL);
	}

	public double getLambda(PlanStep<L> step) {
		if (step.getTime_s() < this.getStartTime_s()
				|| step.getTime_s() > this.getEndTime_s()
				|| !this.getLink().equals(step.getTo())) {
			return 0;
		}
		return this.regression.getCoefficients().get(DEMAND_INDEX);
	}

	protected double getLastLL() {
		return this.lastLL;
	}

	protected double getLastLLPredErr() {
		return this.lastLLPredErr;
	}

	public double getCoeff() {
		return this.regression.getCoefficients().get(DEMAND_INDEX);
	}

	public double getOffset() {
		return this.regression.getCoefficients().get(OFFSET_INDEX);
	}
}
