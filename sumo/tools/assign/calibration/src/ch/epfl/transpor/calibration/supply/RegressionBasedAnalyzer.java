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
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import ch.epfl.transpor.calibration.demand.Demand;
import ch.epfl.transpor.calibration.demand.Plan;
import ch.epfl.transpor.calibration.demand.PlanStep;

/**
 * 
 * @author Gunnar Flötteröd
 * 
 */
public class RegressionBasedAnalyzer<L, P extends Iterable<PlanStep<L>>>
		implements Serializable {

	// -------------------- CONSTANTS --------------------

	public static final int MIN_TIME_S = 0;

	public static final int MAX_TIME_S = 24 * 3600 - 1;

	// -------------------- SERIALIZABLE IMPLEMENTATION --------------------

	private static final long serialVersionUID = 1L;

	/**
	 * the real world measurements
	 */
	private Map<L, List<Measurement<L>>> measurements;

	public void setMeasurements(Map<L, List<Measurement<L>>> measurements) {
		this.measurements = measurements;
	}

	public Map<L, List<Measurement<L>>> getMeasurements() {
		return this.measurements;
	}

	/**
	 * the simulated demand
	 */
	private Demand<L> demand;

	public void setDemand(Demand<L> demand) {
		this.demand = demand;
	}

	public Demand<L> getDemand() {
		return this.demand;
	}

	/**
	 * the forgetting factor of the underlying recursive regressions
	 */
	private double lambda;

	public void setLambda(double lambda) {
		this.lambda = lambda;
	}

	public double getLambda() {
		return this.lambda;
	}

	/**
	 * the average log-likelihood of the last iteration
	 */
	private transient double avgLL = 0;

	/**
	 * the average log-likelihood prediction error of the last iteration
	 */
	private transient double avgLLPredErr = 0;

	/**
	 * empty constructor, only for serialization
	 */
	public RegressionBasedAnalyzer() {
		this(0, 0, 0, 0);
	}

	// -------------------- CONSTRUCTION --------------------

	public RegressionBasedAnalyzer(int startTime_s, int timePeriod_s,
			int binCnt, double lambda) {
		this.demand = new Demand<L>(startTime_s, timePeriod_s, binCnt);
		this.measurements = new LinkedHashMap<L, List<Measurement<L>>>();
		this.lambda = lambda;
	}

	// -------------------- ANALYSIS FUNCTIONALITY --------------------

	public void freeze() {
		for (List<Measurement<L>> measList : this.measurements.values()) {
			for (Measurement<L> meas : measList) {
				meas.freeze();
			}
		}
	}

	public double calcPlanLambda(P plan) {
		double result = 0;
		for (PlanStep<L> step : plan) {
			final List<Measurement<L>> measList = this.measurements.get(step
					.getTo());
			if (measList != null) {
				for (Measurement<L> meas : measList) {
					result += meas.getLambda(step);
				}
			}
		}
		return result;
	}

	public void addFlowMeasurement(L link, int start_s, int end_s,
			double val_veh_h, double stddev_veh_h) {

		// CHECK

		if (link == null)
			throw new IllegalArgumentException("link is null");

		if (val_veh_h < 0 || Double.isNaN(val_veh_h)
				|| Double.isInfinite(val_veh_h))
			throw new IllegalArgumentException("impossible flow value: "
					+ val_veh_h);

		if (stddev_veh_h <= 0 || Double.isNaN(stddev_veh_h)
				|| Double.isInfinite(stddev_veh_h))
			throw new IllegalArgumentException("impossible stddev value: "
					+ stddev_veh_h);

		if (start_s >= end_s)
			throw new IllegalArgumentException("start time " + start_s
					+ " s is not strictly before end time " + end_s + " s");

		if (start_s < MIN_TIME_S || end_s > MAX_TIME_S)
			throw new IllegalArgumentException(
					"allowed measurment time interval is " + MIN_TIME_S
							+ " s -- " + MAX_TIME_S + " s");

		// CONTINUE

		List<Measurement<L>> measList = this.measurements.get(link);
		if (measList == null) {
			measList = new ArrayList<Measurement<L>>();
			this.measurements.put(link, measList);
		}

		// if (this.statistics != null) {
		// final AnalysisMeasurement<L> meas = new AnalysisMeasurement<L>(
		// val_veh_h, stddev_veh_h * stddev_veh_h, start_s, end_s,
		// link, this.lambda);
		// this.statistics.addFlowMeasurement(meas);
		// measList.add(meas);
		// } else {
		final Measurement<L> meas = new Measurement<L>(val_veh_h, stddev_veh_h
				* stddev_veh_h, start_s, end_s, link, this.lambda);
		measList.add(meas);
		// }
	}

	public void addToDemand(Plan<L> plan) {
		for (PlanStep<L> planStep : plan) {
			if (planStep.getTime_s() >= MIN_TIME_S
					&& planStep.getTime_s() <= MAX_TIME_S) {
				this.addToDemand(planStep);
			}
		}
	}

	public void addToDemand(PlanStep<L> step) {
		if (this.measurements.keySet().contains(step.getTo()))
			this.demand.add(step);
	}

	public void afterNetworkLoading(SimResultsContainer<L> simResults) {

		this.avgLL = 0;
		this.avgLLPredErr = 0;
		int measCnt = 0;

		for (List<Measurement<L>> measList : measurements.values()) {
			for (Measurement<L> meas : measList) {
				final double sim_veh_h = simResults.getAvgFlow_veh_h(meas
						.getLink(), meas.getStartTime_s(), meas.getEndTime_s());
				meas.update(this.demand, sim_veh_h);
				this.avgLL += meas.getLastLL();
				this.avgLLPredErr += Math.abs(meas.getLastLLPredErr());
				measCnt++;
			}
		}

		this.avgLL /= measCnt;
		this.avgLLPredErr /= measCnt;

		this.demand.clear();
	}

	public String getInfoString() {
		return " avg. log-likelihood: " + this.avgLL + " +/- "
				+ this.avgLLPredErr;
	}
}
