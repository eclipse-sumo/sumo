package de.psi.telco.sumoplayer;

/**
 * This location listener implementation is SUMOPlayers default.
 * It pipes ouput in CVS style to STDOUT
 * @author will
 *
 */
public class SUMOPipedLocationListener implements SUMOLocationListener {

	public static char separator = '|';
	
	public void LocationUpdated(String vehicleId, int timestep, double lon,
			double lat, double speed) {
		System.out.println(vehicleId+separator+timestep+separator+lon+separator+lat+separator+speed);
	}

}
