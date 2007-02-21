package ns2;

/**
 * model for command line parameters
 * @author Thimor Bohn <bohn@itm.uni-luebeck.de>
 *
 */
public class Parameter {
	/**
	 * constructor
	 * @param net name of netfile
	 * @param trace name of tracefile
	 * @param activity name of activity file
	 * @param mobility name of mobility file
	 * @param config name of config file
	 * @param penetration value in [0,1] of penetration factor
	 * @param seed value for random number generator
	 */
	public Parameter(String net, String trace, String activity, String mobility, String config, double penetration, long seed) {
		this.net         = net;
		this.trace       = trace;
		this.activity    = activity;
		this.mobility    = mobility;
		this.config      = config;
		this.penetration = penetration;
		this.seed        = seed;
	}

	public String net;
	public String trace;
	public String activity;
	public String mobility;
	public String config;
	public double penetration;
	public long seed;
}
