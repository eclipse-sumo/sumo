package de.psi.telco.sumoplayer;

/**
 * This interface can be used to customize SUMOPlayer's output in a simple and powerful way.
 * SUMOPlayer will call this interface whenever it gets to know about a new vehilce position.
 * 
 * E.g. It can be used to implement HTTP requests for SUMO position updates (if needed).
 * 
 * @author michael willigens <michael@willigens.de>
 *
 */
public interface SUMOLocationListener {
	public void LocationUpdated(String vehicleId, int timestep, double lon, double lat, double speed);
}
