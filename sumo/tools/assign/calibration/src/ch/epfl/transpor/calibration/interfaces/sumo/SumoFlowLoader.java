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
package ch.epfl.transpor.calibration.interfaces.sumo;

import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;

import org.xml.sax.Attributes;
import org.xml.sax.helpers.DefaultHandler;

import ch.epfl.transpor.calibration.supply.SimResults;
import ch.epfl.transpor.calibration.supply.SimResultsContainer;

/**
 * 
 * @author Gunnar Flötteröd
 *
 */
class SumoFlowLoader {

	private static final int DEFAULT_STARTTIME_S = 0;

	static final String INTERVAL_ELEM = "interval";
	static final String START_ATTR = "begin";
	static final String END_ATTR = "end";

	static final String EDGE_ELEM = "edge";
	static final String ID_ATTR = "id";
	static final String ENTERED_ATTR = "entered";

	// -------------------- HELPERS --------------------

	private static void checkBin(final int start_s, final int end_s,
			final int binSize_s) {
		final boolean ok = (start_s % binSize_s == 0 && end_s - start_s == binSize_s - 1);
		if (!ok) {
			throw new RuntimeException("File contains a bin from " + start_s
					+ " s to " + end_s + " s, which is inconsistent with the"
					+ " bin size of " + binSize_s + " s.");
		}
	}

	// -------------------- MEASUREMENT LOADING --------------------

	static void loadMeasurements(final String fileName,
			final SumoCalibrator calibrator, final double stddev_veh_h,
			final int binSize_s) {
		final MeasurementsHandler handler = new MeasurementsHandler(calibrator,
				stddev_veh_h, binSize_s);
		try {
			final SAXParserFactory factory = SAXParserFactory.newInstance();
			final SAXParser saxParser = factory.newSAXParser();
			saxParser.parse(fileName, handler);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	private static class MeasurementsHandler extends DefaultHandler {
		private final SumoCalibrator calibrator;
		private final double stddev_veh_h;
		private final int binSize_s;
		private int start_s;
		private int end_s;

		private MeasurementsHandler(final SumoCalibrator calibrator,
				final double stddev_veh_h, final int binSize_s) {
			this.calibrator = calibrator;
			this.stddev_veh_h = stddev_veh_h;
			this.binSize_s = binSize_s;
		}

		public void startElement(String namespaceURI, String sName,
				String qName, Attributes attrs) {
			if (INTERVAL_ELEM.equals(qName)) {
				this.start_s = Integer.parseInt(attrs.getValue(START_ATTR));
				this.end_s = Integer.parseInt(attrs.getValue(END_ATTR));
				checkBin(this.start_s, this.end_s, this.binSize_s);
			} else if (EDGE_ELEM.equals(qName)) {
				final String linkId = attrs.getValue(ID_ATTR);
				final double entered_veh = Integer.parseInt(attrs
						.getValue(ENTERED_ATTR));
				final double flow_veh_h = entered_veh * 3600.0
						/ (this.end_s - this.start_s + 1);
				this.calibrator.addFlowMeasurement(linkId, this.start_s,
						this.end_s, flow_veh_h, this.stddev_veh_h);
			}
		}
	}

	// -------------------- NETWORK CONDITION LOADING --------------------

	static SimResultsContainer<String> loadSimResults(final String fileName,
			final int binSize_s) {
		final SimResultsHandler handler = new SimResultsHandler(binSize_s);
		try {
			final SAXParserFactory factory = SAXParserFactory.newInstance();
			final SAXParser saxParser = factory.newSAXParser();
			saxParser.parse(fileName, handler);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return handler.result;
	}

	private static class SimResultsHandler extends DefaultHandler {

		private final SimResults<String> result;
		private int bin;

		private SimResultsHandler(final int binSize_s) {
			this.result = new SimResults<String>(DEFAULT_STARTTIME_S,
					binSize_s, 24 * 3600 / binSize_s);
		}

		public void startElement(String namespaceURI, String sName,
				String qName, Attributes attrs) {
			if (INTERVAL_ELEM.equals(qName)) {
				final int startTime_s = Integer.parseInt(attrs
						.getValue(START_ATTR));
				final int endTime_s = Integer
						.parseInt(attrs.getValue(END_ATTR));
//				checkBin(startTime_s, endTime_s, this.result.getTimePeriod_s());
				this.bin = this.result.bin(startTime_s);
			} else if (EDGE_ELEM.equals(qName)) {
				final String linkId = attrs.getValue(ID_ATTR);
				final double entered_veh = Double.parseDouble(attrs
						.getValue(ENTERED_ATTR));
				this.result.put(linkId, this.bin, entered_veh);
			}
		}
	}
}