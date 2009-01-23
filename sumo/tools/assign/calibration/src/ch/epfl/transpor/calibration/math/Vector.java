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
public class Vector implements Serializable {

	// -------------------- SERIALIZABLE IMPLEMENTATION --------------------

	private static final long serialVersionUID = 1L;

	/**
	 * the data contained by this vector
	 */
	private double[] data;

	public void setData(double[] data) {
		this.data = data;
	}

	public double[] getData() {
		return this.data;
	}

	/**
	 * empty constructor, only for serialization
	 */
	public Vector() {
		this(0);
	}

	// -------------------- CONSTRUCTION --------------------

	public Vector(int dim) {
		this.data = new double[dim];
	}

	public Vector(double... data) {
		this.data = data;
	}

	public Vector copy() {
		final Vector result = new Vector(this.data.length);
		System.arraycopy(this.data, 0, result.data, 0, this.data.length);
		return result;
	}

	// -------------------- IMPLEMENTATION --------------------

	public int size() {
		return this.data.length;
	}

	public double get(int pos) {
		return this.data[pos];
	}

	public void set(int pos, double value) {
		this.data[pos] = value;
	}

	public void add(int pos, double value) {
		this.data[pos] += value;
	}

	public void mult(int pos, double value) {
		this.data[pos] *= value;
	}

	public void clear() {
		for (int i = 0; i < this.size(); i++)
			set(i, 0.0);
	}

	public void mult(double val) {
		for (int i = 0; i < size(); i++)
			mult(i, val);
	}

	public double innerProd(Vector other) {
		double result = 0;
		for (int i = 0; i < this.size(); i++)
			result += this.get(i) * other.get(i);
		return result;
	}

	public void add(Vector other, double weight) {
		for (int i = 0; i < this.size(); i++)
			this.add(i, weight * other.get(i));
	}

	public String toString() {
		final StringBuffer result = new StringBuffer();
		result.append("[ ");
		for (int i = 0; i < size(); i++)
			result.append(get(i) + " ");
		result.append("]");
		return result.toString();
	}

	// -------------------- TESTING --------------------

	private static void testSerialization() throws IOException {
		Vector v = new Vector(3);
		v.set(0, 7);
		v.set(1, 12);
		v.set(2, 1976);
		
		XMLEncoder e = new XMLEncoder(new BufferedOutputStream(
				new FileOutputStream("test.xml")));
		e.writeObject(v);
		e.close();

		XMLDecoder d = new XMLDecoder(new FileInputStream("test.xml"));
		v = (Vector) d.readObject();
		System.out.println(v);
	}

	public static void main(String[] args) throws IOException {
		testSerialization();
	}

}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
//
// IMPLEMENTATION of XMLStreamable
//
// public static final String VECTOR_ELEM = "vector";
//
// private static final String DATA_ATTR = "data";
//
// public void writeXML(final BufferedWriter writer, final String prefix)
// throws IOException {
// writer.write(prefix);
// writer.write("<");
// writer.write(VECTOR_ELEM);
// writer.write(" ");
// writer.write(DATA_ATTR);
// writer.write("=\"");
// for (int i = 0; i < data.length; i++) {
// writer.write(Double.toString(data[i]));
// writer.write(" ");
// }
// writer.write("\">");
// writer.newLine();
// }
//
// public SimpleHandler<Vector> newHandler() {
// return new Handler();
// }
//
// private static class Handler implements SimpleHandler<Vector> {
// public void startElement(String name, Attributes attrs) {
// if (Vector.VECTOR_ELEM.equals(name)) {
// // TODO
// }
// }
//
// public void endElement(String name) {
// }
//
// public Vector result() {
// return null; // TODO
// }
// }
