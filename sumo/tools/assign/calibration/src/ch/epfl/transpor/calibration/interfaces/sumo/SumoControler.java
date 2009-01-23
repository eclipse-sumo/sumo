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

import java.beans.XMLDecoder;
import java.beans.XMLEncoder;
import java.io.BufferedOutputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Random;

/**
 * 
 * @author Gunnar Flötteröd
 * 
 */
public class SumoControler {

	// -------------------- CONSTANTS --------------------

	static final String INIT = "INIT";

	static final String CHOICE = "CHOICE";

	static final String UPDATE = "UPDATE";

	// -------------------- SERIALIZATION OF CALIBRATOR --------------------

	private static final String SERIALIZED_FILE = "serialized.xml";

	private static void serialize(final SumoCalibrator calibrator)
			throws IOException {
		final XMLEncoder encoder = new XMLEncoder(new BufferedOutputStream(
				new FileOutputStream(SERIALIZED_FILE)));
		encoder.writeObject(calibrator);
		encoder.flush();
		encoder.close();
	}

	private static SumoCalibrator deserialize() throws IOException {
		final XMLDecoder decoder = new XMLDecoder(new FileInputStream(
				SERIALIZED_FILE));
		final SumoCalibrator result = (SumoCalibrator) decoder.readObject();
		return result;
	}

	// -------------------- MAIN OPERATIONS --------------------

	private static void unknown(final String[] args) {
		System.err.print("Unknown parameter list: ");
		if (args != null) {
			for (String arg : args) {
				System.err.print(arg);
				System.err.print(" ");
			}
		}
		System.err.println();
		System.exit(-1);
	}

	private static void init(final String[] args) throws IOException {
		if (args.length != 8) {
			System.err.println("Initialization needs these parameters"
					+ " after the " + INIT + " keyword:");
			System.err.println("(1) file that contains the measurements");
			System.err.println("(2) measurement standard deviation (in veh/h)");
			System.err.println("(3) random seed");
			System.err.println("(4) memory coefficient of recursive "
					+ "regression");
			System.err.println("(5) number of preparatory iterations");
			System.err.println("(6) number of iterations until system freezes");
			System.err.println("(7) network data bin size (in seconds)");
			unknown(args);
		}
		final String measurementsFile = args[1];
		final double stddev_veh_h = Double.parseDouble(args[2]);
		final long randomSeed = Long.parseLong(args[3]);
		final double lambda = Double.parseDouble(args[4]);
		final int prepIterations = Integer.parseInt(args[5]);
		final int freezeIteration = Integer.parseInt(args[6]);
		final int binSize_s = Integer.parseInt(args[7]);

		final SumoCalibrator calibrator = new SumoCalibrator(new Random(
				randomSeed), lambda, binSize_s);
		calibrator.setPreparatoryIterations(prepIterations);
		calibrator.setFreezeIteration(freezeIteration);
		calibrator.loadMeasurements(measurementsFile, stddev_veh_h);
		serialize(calibrator);
	}

	private static void select(final String[] args) throws IOException {
		if (args.length != 3) {
			System.err.println("calibrated selection needs these parameters"
					+ " after the " + CHOICE + " keyword:");
			System.err.println("(1) file that contains the choice sets");
			System.err.println("(2) file where choices are to be written");
			unknown(args);
		}
		final String popFile = args[1];
		final String choiceFile = args[2];

		final SumoCalibrator calibrator = deserialize();
		calibrator.choice(popFile, choiceFile);
		serialize(calibrator);
	}

	private static void update(final String[] args) throws IOException {
		if (args.length != 2) {
			System.err.println("internal update needs these parameters"
					+ " after the " + UPDATE + " keyword:");
			System.err.println("(1) file that contains the network conditions");
			unknown(args);
		}
		final String netCondFile = args[1];

		final SumoCalibrator calibrator = deserialize();
		calibrator.update(netCondFile);
		serialize(calibrator);
	}

	// -------------------- MAIN FUNCTION --------------------

	public static void main(String[] args) throws IOException {
		if (args == null || args.length == 0) {
			unknown(args);
		}
		final String action = args[0];

		if (INIT.equalsIgnoreCase(action)) {
			init(args);
		} else if (CHOICE.equalsIgnoreCase(action)) {
			select(args);
		} else if (UPDATE.equalsIgnoreCase(action)) {
			update(args);
		} else {
			unknown(args);
		}
	}
}
