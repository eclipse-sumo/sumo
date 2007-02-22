package ns2;

import java.util.LinkedList;
import java.util.List;
import java.util.Random;

/**
 * class for randomizing ids
 * a sumo id maps to a ns2 id. selecting the mapping is done by randomizing
 * @author Thimor Bohn <bohn@itm.uni-luebeck.de>
 *
 */
public class IdRandomizer {
	/**
	 * working method
	 * @param wantedVehicle list of vehicles to be selected for ns2
	 * @param vehicleNewId list of vehicle ids for ns2
	 */
	public static void randomize(List<String> wantedVehicle, List<String> vehicleNewId, long seed) {
		// set of new ids = {0, 1, ..., N-1}
		int N = wantedVehicle.size();
		List<String> tmpId = new LinkedList<String>();
		for (int i=0; i<N; i++) {
			tmpId.add("" + i);
		}
		Random rnd = new Random(seed);
		for (int i=0; i<N; i++) {
			int num = rnd.nextInt(tmpId.size());
			String val = tmpId.remove(num);
			vehicleNewId.add(val);
		}
	}
}
