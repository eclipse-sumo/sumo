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
package ch.epfl.transpor.calibration.demand;

/**
 * 
 * @author Gunnar Flötteröd
 *
 * @param <L>
 */
public class PlanFactory<L> {

	private enum Status {
		VALID, INVALID
	};

	private final String RESET_THIS = "reset() this, previous entries were invalid";

	// -------------------- MEMBERS --------------------

	private Plan<L> result;

	private Status status;

	private L lastLink;

	// -------------------- CONSTRUCTION --------------------

	public PlanFactory() {
		reset();
	}

	// -------------------- PLAN BUILDING PROCEDURE --------------------

	public void reset() {
		this.result = new Plan<L>();
		this.status = Status.VALID;
		this.lastLink = null;
	}

	public void addDiscreteParameter(final String label, final Object value) {
		this.result.addDiscreteParameter(label, value);
	}

	public void addContinousParameter(final String label, final double value) {
		this.result.addContinuousParameter(label, value);
	}

	public void addEntry(L entryLink, int time_s) {

		// CHECK

		if (this.status == Status.INVALID) {
			throw new RuntimeException(this.RESET_THIS);
		} else if (this.lastLink != null) {
			this.status = Status.INVALID;
			throw new RuntimeException("last link is not null "
					+ "(currently not outside of network)");
		}

		// CONTINUE

		this.result.addStep(new PlanStep<L>(null, entryLink, time_s));
		this.lastLink = entryLink;
	}

	public void addTurn(L toLink, int time_s) {

		// CHECK

		if (this.status == Status.INVALID) {
			throw new RuntimeException(this.RESET_THIS);
		} else if (this.lastLink == null) {
			this.status = Status.INVALID;
			throw new RuntimeException("last link is null "
					+ "(currently not inside the network)");
		}

		// CONTINUE

		this.result.addStep(new PlanStep<L>(this.lastLink, toLink, time_s));
		this.lastLink = toLink;
	}

	public void addExit(int time_s) {

		// CHECK

		if (this.status == Status.INVALID) {
			throw new RuntimeException(RESET_THIS);
		} else if (this.lastLink == null) {
			this.status = Status.INVALID;
			throw new RuntimeException("last link is null "
					+ "(currently not inside the network)");
		}

		// CONTINUE

		this.result.addStep(new PlanStep<L>(this.lastLink, null, time_s));
		this.lastLink = null;
	}

	public Plan<L> getResult() {
		return (this.status != Status.INVALID) ? this.result : null;
	}

}
