package de.tudresden.ws.container;

public enum SumoTrafficLightType {

	    STATIC(0),
	    RAIL_SIGNAL(1),
	    RAIL_CROSSING(2),
	    ACTUATED(3),
	    DELAYBASED(4),
	    SOTL_PHASE(5),
	    SOTL_PLATOON(6),
	    SOTL_REQUEST(7),
	    SOTL_WAVE(8),
	    SOTL_MARCHING(9),
	    SWARM_BASED(10),
	    HILVL_DETERMINISTIC(11),
	    OFF(12),
	    INVALID(13);
	
	int index;
	
	SumoTrafficLightType(int index){this.index = index;}
	
	public int getIndex() {return this.index;}
	
}
