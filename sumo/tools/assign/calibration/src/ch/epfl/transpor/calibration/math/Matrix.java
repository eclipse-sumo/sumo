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

/**
 * 
 * @author Gunnar Flötteröd
 *
 */
public class Matrix implements Serializable {

	// -------------------- SERIALIZABLE IMPLEMENTATION --------------------

	private static final long serialVersionUID = 1L;

	/**
	 * the data contained by this matrix
	 */
	private Vector[] rows;

	public void setRows(Vector[] rows) {
		this.rows = rows;
	}

	public Vector[] getRows() {
		return this.rows;
	}

	/**
	 * empty constructor, only for serialization
	 */
	public Matrix() {
		this(0, 0);
	}

	// -------------------- CONSTRUCTION --------------------

	public Matrix(int rowCnt, int colCnt) {
		this.rows = new Vector[rowCnt];
		for (int i = 0; i < rowCnt; i++)
			this.rows[i] = new Vector(colCnt);
	}

	public static Matrix newDiagonal(int dim, double val) {
		final Matrix result = new Matrix(dim, dim);
		for (int i = 0; i < dim; i++)
			result.getRow(i).set(i, val);
		return result;
	}

	// -------------------- IMPLEMENTATION --------------------

	public int rowSize() {
		return rows.length;
	}

	public Vector getRow(int i) {
		return rows[i];
	}

	public double frobeniusNorm() {
		double result = 0;
		for (Vector row : rows)
			result += row.innerProd(row);
		return Math.sqrt(result);
	}

	public void mult(double value) {
		for (Vector row : rows)
			row.mult(value);
	}

	public void addOuterProduct(Vector other1, Vector other2, double weight) {
		for (int i = 0; i < other1.size(); i++)
			this.getRow(i).add(other2, other1.get(i) * weight);
	}

	public Vector timesVectorFromLeft(Vector other, Vector result) {
		for (int l = 0; l < other.size(); l++)
			result.add(this.getRow(l), other.get(l));
		return result;
	}

	public Vector timesVectorFromLeft(Vector other) {
		return timesVectorFromLeft(other, new Vector(other.size()));
	}

	// TODO check this
	// public Matrix times(Matrix other, Matrix result) {
	// for (int i = 0; i < this.rowSize(); i++)
	// other.timesVectorFromLeft(this.getRow(i), result.getRow(i));
	// return result;
	// }

	public String toString() {
		final StringBuffer result = new StringBuffer();
		for (Vector row : rows) {
			result.append(row.toString());
			result.append("\n");
		}
		return result.toString();
	}

	// -------------------- TESTING --------------------

	private static void testSerialization() throws IOException {
		Matrix m = new Matrix(2, 3);
		m.getRow(0).set(0, 7);
		m.getRow(0).set(1, 12);
		m.getRow(0).set(2, 1976);
		m.getRow(1).set(0, 1);
		m.getRow(1).set(1, 1);
		m.getRow(1).set(2, 2009);

		XMLEncoder e = new XMLEncoder(new BufferedOutputStream(
				new FileOutputStream("test.xml")));
		e.writeObject(m);
		e.close();

		XMLDecoder d = new XMLDecoder(new FileInputStream("test.xml"));
		m = (Matrix) d.readObject();
		System.out.println(m);
	}

	public static void main(String[] args) throws IOException {
		testSerialization();
	}
}

// ------------------------------------------------------------
// ------------------------------------------------------------
// ------------------------------------------------------------
//
// IMPLEMENTATION OF XMLStreamable
//
// private static final String MATRIX_ELEM = "matrix";
//
// public void writeXML(final BufferedWriter writer, final String prefix)
// throws IOException {
// writer.write(prefix);
// writer.write("<");
// writer.write(MATRIX_ELEM);
// writer.write("/>");
// for (Vector row : rows) {
// row.writeXML(writer, prefix + "  ");
// }
// writer.write("</");
// writer.write(MATRIX_ELEM);
// writer.write(">");
// writer.newLine();
// }
//
// public SimpleHandler<Matrix> newHandler() {
// return new MatrixHandler();
// }
//
// private static class MatrixHandler implements SimpleHandler<Matrix> {
// private SimpleHandler<Vector> vectorHandler = null;
//
// @Override
// public void startElement(String name, Attributes attrs) {
// if (this.vectorHandler != null) {
// this.vectorHandler.startElement(name, attrs);
// }
// if (Vector.VECTOR_ELEM.equals(name)) {
// this.vectorHandler = (new Vector()).newHandler();
// this.vectorHandler.startElement(name, attrs);
// }
// }
//
// @Override
// public void endElement(String name) {
// if (Vector.VECTOR_ELEM.equals(name)) {
//
// }
// }
//
// public Matrix result() {
// return null; // TODO
// }
// }
