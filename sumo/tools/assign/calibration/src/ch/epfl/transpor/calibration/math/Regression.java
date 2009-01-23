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
package ch.epfl.transpor.calibration.math;

import java.beans.XMLDecoder;
import java.beans.XMLEncoder;
import java.io.BufferedOutputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.Serializable;
import java.util.Random;

/**
 * 
 * @author Gunnar Flötteröd
 * 
 * @see http://en.wikipedia.org/wiki/Recursive_least_squares
 * 
 */
public class Regression implements Serializable {

	// -------------------- SERIALIZABLE IMPLEMENTATION --------------------

	private static final long serialVersionUID = 1L;

	/**
	 * the estimated coefficients
	 */
	private Vector coefficients;

	public void setCoefficients(Vector coefficients) {
		this.coefficients = coefficients;
	}

	public Vector getCoefficients() {
		return this.coefficients;
	}

	/**
	 * the precision matrix of the estimated coefficients
	 */
	private Matrix precisionMatrix;

	public void setPrecisionMatrix(Matrix precisionMatrix) {
		this.precisionMatrix = precisionMatrix;
	}

	public Matrix getPrecisionMatrix() {
		return this.precisionMatrix;
	}

	/**
	 * the forgetting factor
	 */
	private double lambda;

	public void setLambda(double lambda) {
		if (lambda <= 0 || lambda > 1) {
			throw new IllegalArgumentException("lambda must be in (0,1]");
		}
		this.lambda = lambda;
	}

	public double getLambda() {
		return this.lambda;
	}

	/**
	 * empty constructor, only for serialization
	 */
	public Regression() {
		this(1, null, null);
	}

	// -------------------- CONSTRUCTION --------------------

	private Regression(double lambda, Vector a, Matrix precisionMatrix) {
		this.setLambda(lambda);
		this.setCoefficients(a);
		this.setPrecisionMatrix(precisionMatrix);
	}

	public Regression(double lambda, int dim) {
		this(lambda, new Vector(dim), Matrix.newDiagonal(dim, 1e6));
	}

	public Regression(Vector a, double var) {
		this(1.0, a.copy(), Matrix.newDiagonal(a.size(), var));
	}

	public Regression(int dim) {
		this(1.0, new Vector(dim), Matrix.newDiagonal(dim, 1e6));
	}

	// -------------------- IMPLEMENTATION --------------------

	public int getDimension() {
		return getCoefficients().size();
	}

	public double predict(Vector x) {
		return x.innerProd(this.getCoefficients()); // TODO use this in
		// update(..)
	}

	public void update(Vector x, double y) {
		final Vector xP = this.precisionMatrix.timesVectorFromLeft(x); // P is
		// symmetric
		final double gScale = 1.0 / (this.lambda + xP.innerProd(x));

		this.getCoefficients().add(xP,
				(y - x.innerProd(this.getCoefficients())) * gScale);

		this.precisionMatrix.addOuterProduct(xP, xP, -gScale);
		this.precisionMatrix.mult(1.0 / this.lambda);
	}

	// -------------------- TESTING --------------------

	private static double rnd() {
		return (new Random()).nextGaussian();
	}

	private static Vector newRndVect(int dim) {
		final Vector result = new Vector(dim);
		for (int i = 0; i < dim - 1; i++)
			result.set(i, rnd());
		result.set(dim - 1, 1); // constant
		return result;
	}

	static void test10dimRandom() {
		final int dim = 10;
		final int its = 25;
		final int runs = 1;

		for (int run = 0; run < runs; run++) {
			final Vector a = newRndVect(dim);
			final Regression r = new Regression(new Vector(dim), 1e6);

			for (int it = 0; it < its; it++) {
				Vector x = newRndVect(dim);
				final double y = x.innerProd(a);
				final double e = y - x.innerProd(r.getCoefficients());

				// ----------------------------------------
				x = new Vector(dim);
				x.set(0, 1); // constant explanatory variable
				x.set(dim - 1, 1); // _the_ constant
				// ----------------------------------------

				System.out.println("it = " + it + "\terr = " + e * e
						+ "\tnorm = " + r.getPrecisionMatrix().frobeniusNorm());
				System.out.println(r.getCoefficients());
				System.out.println(r.getPrecisionMatrix());
				System.out.println();

				r.update(x, y);
			}
			System.out.println();
		}
	}

	static void testConstantInput() {
		final Regression r = new Regression(new Vector(2), 1e6);
		for (int it = 0; it < 20; it++) {
			final Vector x = new Vector(it == 10 ? 1e0 : 1e-6, 1.0);
			final double y = 2.0 + Math.random();
			final double e = y - x.innerProd(r.getCoefficients());
			System.out.println("it = " + it + ", err = " + e * e + ", norm = "
					+ r.getPrecisionMatrix().frobeniusNorm());
			System.out.println(r.getCoefficients());
			System.out.println(r.getPrecisionMatrix());
			r.update(x, y);
		}
	}

	private static void testSerialization() throws IOException {
		Regression r = new Regression(2);
		r.update(new Vector(0.0, 0.0), 1.0 * 0.0 + 0.0);
		r.update(new Vector(1.0, 1.0), 1.0 * 1.0 + 1.0);
		System.out.println(r.getCoefficients());
		System.out.println(r.getPrecisionMatrix());

		XMLEncoder e = new XMLEncoder(new BufferedOutputStream(
				new FileOutputStream("test.xml")));
		e.writeObject(r);
		e.close();

		XMLDecoder d = new XMLDecoder(new FileInputStream("test.xml"));
		r = (Regression) d.readObject();
		System.out.println(r.getCoefficients());
		System.out.println(r.getPrecisionMatrix());
	}

	public static void main(String[] args) throws IOException {
		// testConstantInput();
		testSerialization();
	}
}
