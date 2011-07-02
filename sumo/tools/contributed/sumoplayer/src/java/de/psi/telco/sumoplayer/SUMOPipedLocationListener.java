package de.psi.telco.sumoplayer;
/**
 * This location listener implementation is SUMOPlayers default.
 * It pipes ouput in CVS style to STDOUT
 * @author will
 *
 */
public class SUMOPipedLocationListener implements SUMOLocationListener {

	public static char separator = ' ';/* a space is the seperator*/
	
	/*speed is in meters , lon lat are just names actual value is
	 in cartesian coordinates , with offsets mentioned in the 
	 SUMOnetwork file. To convert the number back to lon lat use
	 pyproj (look for code convert_sumoplayer_op_2_lonlat.py )*/

	public void LocationUpdated(String vehicleId, int timestep, double lon,
			double lat, double speed) {
		
		System.out.println(timestep+(separator+vehicleId)+separator+lon+separator+lat+separator+(speed/3.6));
		
	}

}
