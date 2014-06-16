package de.tudresden.ws.container;

/**
 * 
 * @author Mario Krumnow
 * @author Anja Liebscher
 *
 */

public enum SumoVehicleSignalState {
	
	VEH_SIGNAL_BLINKER_RIGHT (0),
	VEH_SIGNAL_BLINKER_LEFT (1),
	VEH_SIGNAL_BLINKER_EMERGENCY (2),
	VEH_SIGNAL_BRAKELIGHT(3),
	VEH_SIGNAL_FRONTLIGHT(4),
	VEH_SIGNAL_FOGLIGHT(5),
	VEH_SIGNAL_HIGHBEAM(6),
	VEH_SIGNAL_BACKDRIVE(7),
	VEH_SIGNAL_WIPER(8),
	VEH_SIGNAL_DOOR_OPEN_LEFT(9),
	VEH_SIGNAL_DOOR_OPEN_RIGHT(10),
	VEH_SIGNAL_EMERGENCY_BLUE(11),
	VEH_SIGNAL_EMERGENCY_RED(12),
	VEH_SIGNAL_EMERGENCY_YELLOW(13); 
	
	private final int pos;
	
	SumoVehicleSignalState(int pos){this.pos = pos;}

	public int getPos(){return this.pos;}
	
}
