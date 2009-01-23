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
public class TurningMove<L> {

	// -------------------- MEMBERS --------------------

	private final L from;

	private final L to;

	private final int hashCode; // since this object is immutable

	// -------------------- CONSTRUCTION --------------------

	public TurningMove(L from, L to) {
		this.from = from;
		this.to = to;

		int hashCode = 1;
		hashCode = 31 * hashCode + (from == null ? 0 : from.hashCode());
		hashCode = 31 * hashCode + (to == null ? 0 : to.hashCode());
		this.hashCode = hashCode;
	}

	// -------------------- SIMPLE FUNCTIONALITY --------------------

	public L getFrom() {
		return this.from;
	}

	public L getTo() {
		return this.to;
	}

	// -------------------- OVERRIDE Object --------------------

	private boolean equal(L l1, L l2) {
		return l1 == null ? l2 == null : l1.equals(l2);
	}

	public boolean hasEqualLinks(TurningMove<L> other) {
		return equal(this.getFrom(), other.getFrom())
				&& equal(this.getTo(), other.getTo());
	}

	public boolean equals(Object other) {
		if (this == other)
			return true;
		if (other == null || other.getClass() != this.getClass())
			return false;
		return hasEqualLinks((TurningMove<L>) other);
	}

	public int hashCode() {
		return this.hashCode;
	}

	public String toString() {
		return this.getFrom() + " -> " + this.getTo();
	}
}
