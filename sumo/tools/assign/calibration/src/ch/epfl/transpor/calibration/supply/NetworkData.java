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
import java.util.HashMap;
import java.util.Map;

/**
 * 
 * @author Gunnar Flötteröd
 * 
 * @param <L>
 */
public class NetworkData<L> implements Serializable {

	// -------------------- SERIALIZABLE IMPLEMENTATION --------------------

	private static final long serialVersionUID = 1L;

	/**
	 * data start time
	 */
	private int startTime_s;

	public void setStartTime_s(int startTime_s) {
		this.startTime_s = startTime_s;
	}

	public int getStartTime_s() {
		return this.startTime_s;
	}

	/**
	 * length of a time bin
	 */
	private int timePeriod_s;

	public void setTimePeriod_s(int timePeriod_s) {
		this.timePeriod_s = timePeriod_s;
	}

	public int getTimePeriod_s() {
		return this.timePeriod_s;
	}

	/**
	 * number of time bins
	 */
	private int binCnt;

	public void setBinCnt(int binCnt) {
		this.binCnt = binCnt;
	}

	public int getBinCnt() {
		return this.binCnt;
	}

	/**
	 * the data stored in this container
	 */
	private Map<L, double[]> data;

	public void setData(Map<L, double[]> data) {
		this.data = data;
	}

	public Map<L, double[]> getData() {
		return this.data;
	}

	/**
	 * empty constructor, only for serialization
	 */
	public NetworkData() {
		this(0, 0, 0);
	}

	// -------------------- CONSTRUCTION --------------------

	public NetworkData(final int startTime_s, final int timePeriod_s,
			final int binCnt) {
		this.startTime_s = startTime_s;
		this.timePeriod_s = timePeriod_s;
		this.binCnt = binCnt;
		this.data = new HashMap<L, double[]>();
	}

	// -------------------- CONTENT ACCESS --------------------

	public int bin(int time_s) {
		return (time_s - this.startTime_s) / this.timePeriod_s;
	}

	public int binStart_s(int bin) {
		return this.startTime_s + bin * this.timePeriod_s;
	}

	public int binEnd_s(int bin) {
		return binStart_s(bin + 1) - 1;
	}

	public void put(L link, int bin, double value) {
		double[] dataArray = this.getData().get(link);
		if (dataArray == null) {
			dataArray = new double[this.getBinCnt()];
			this.getData().put(link, dataArray);
		}
		dataArray[bin] = value;
	}

	// TODO add "add" function and use it in Demand class

	public double get(L link, int startTime_s, int endTime_s) {

		final double[] demandSequence = this.data.get(link);
		if (demandSequence == null)
			return 0;

		final int startBin = Math.max(bin(startTime_s), 0);
		final int endBin = Math.min(bin(endTime_s), this.binCnt - 1);

		double result = 0;
		for (int bin = startBin; bin <= endBin; bin++) {
			final double weight = (Math.min(binEnd_s(bin), endTime_s)
					- Math.max(binStart_s(bin), startTime_s) + 1.0)
					/ this.timePeriod_s;
			result += weight * demandSequence[bin];
		}
		return result;
	}

	public double[] getTotal() {
		final double[] result = new double[this.binCnt];
		for (double[] addend : this.data.values())
			for (int i = 0; i < this.binCnt; i++)
				result[i] += addend[i];
		return result;
	}

	public void clear() {
		this.data.clear();
	}
}

// ------------------------------------------------------------
// ------------------------------------------------------------
// ------------------------------------------------------------
//
// private static final String DATA_ELEM = "data";
//
// private static final String START_ATTR = "startTime_s";
//
// private static final String PERIOD_ATTR = "timePeriod_s";
//
// private static final String BINS_ATTR = "binCnt";
//
// private static final String ENTRY_ELEM = "entry";
//
// private static final String LINK_ATTR = "link";
//
// private static final String VALUES_ATTR = "values";
//
// public NetworkData(final String fileName,
// LinkInstanceProvider<L> linkInstanceProvider) {
// final SimResultsHandler<L> handler = new SimResultsHandler<L>(
// linkInstanceProvider);
// try {
// final SAXParserFactory factory = SAXParserFactory.newInstance();
// final SAXParser saxParser = factory.newSAXParser();
// saxParser.parse(fileName, handler);
// } catch (Exception e) {
// e.printStackTrace();
// }
// this.startTime_s = handler.startTime_s;
// this.timePeriod_s = handler.timePeriod_s;
// this.binCnt = handler.binCnt;
// this.data = handler.data;
// }
//
// private static class SimResultsHandler<L> extends DefaultHandler {
//
// private final LinkInstanceProvider<L> linkInstanceProvider;
//
// private int startTime_s;
//
// private int timePeriod_s;
//
// private int binCnt;
//
// private Map<L, double[]> data = new HashMap<L, double[]>();
//
// private SimResultsHandler(
// final LinkInstanceProvider<L> linkInstanceProvider) {
// this.linkInstanceProvider = linkInstanceProvider;
// }
//
// public void startElement(String namespaceURI, String sName,
// String qName, Attributes attrs) {
// if (DATA_ELEM.equals(qName)) {
// this.startTime_s = Integer.parseInt(attrs.getValue(START_ATTR));
// this.timePeriod_s = Integer.parseInt(attrs
// .getValue(PERIOD_ATTR));
// this.binCnt = Integer.parseInt(attrs.getValue(BINS_ATTR));
// } else if (ENTRY_ELEM.equals(qName)) {
// final L link = this.linkInstanceProvider.getLink(attrs
// .getValue(LINK_ATTR));
// final String[] entriesStr = attrs.getValue(ENTRY_ELEM).split(
// "\\s");
// final double[] entries = new double[entriesStr.length];
// for (int i = 0; i < entries.length; i++) {
// entries[i] = Integer.parseInt(entriesStr[i]);
// }
// this.data.put(link, entries);
// }
// }
// }
//
// protected void writeAttr(final String name, final String value,
// final BufferedWriter writer) throws IOException {
// writer.write(name);
// writer.write("=\"");
// writer.write(value);
// writer.write("\" ");
// }
//
// protected void writeAdditionalAttributes() {
// }
//
// protected void writeAdditionalElements() {
// }
//
// public void toXMLFile(final String file) throws IOException {
// final BufferedWriter writer = new BufferedWriter(new FileWriter(file));
// writer.write("<");
// writer.write(DATA_ELEM);
// writer.write(" ");
// writeAttr(START_ATTR, Integer.toString(this.startTime_s), writer);
// writeAttr(PERIOD_ATTR, Integer.toString(this.timePeriod_s), writer);
// writeAttr(BINS_ATTR, Integer.toString(this.binCnt), writer);
// this.writeAdditionalAttributes();
// writer.write(">");
// writer.newLine();
// this.writeAdditionalElements();
// for (Map.Entry<L, double[]> entry : this.data.entrySet()) {
// if (entry.getValue() != null) {
// writer.write("  <");
// writer.write(ENTRY_ELEM);
// writer.write(" ");
// writeAttr(LINK_ATTR, entry.getKey().toString(), writer);
// final StringBuffer values = new StringBuffer();
// for (Double value : entry.getValue()) {
// values.append(value);
// values.append(" ");
// }
// writeAttr(VALUES_ATTR, values.toString(), writer);
// writer.write("/>)");
// }
// }
// writer.write("</");
// writer.write(DATA_ELEM);
// writer.write(">");
// writer.newLine();
// writer.flush();
// writer.close();
// }