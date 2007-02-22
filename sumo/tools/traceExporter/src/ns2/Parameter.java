package ns2;

/**
 * model for command line parameters
 * @author Thimor Bohn <bohn@itm.uni-luebeck.de>
 *
 */
public class Parameter {
	public String net;
	public String trace;
	public String activity;
	public String mobility;
	public String config;
	public double begin;
	public double end;
	public double penetration;
	public long seed;
	public boolean hasPenetration;

	/**
	 * constructor
	 * @param net name of sumo net file
	 * @param trace name of sumo trace file
	 * @param activity name of ns2 activity file
	 * @param mobility name of ns2 mobility file
	 * @param config name of ns2 config file
	 */
	public Parameter(String net, String trace, String activity, String mobility, String config, double begin, double end, double penetration, long seed, boolean hasPenetration) {
		this.net         = net;
		this.trace       = trace;
		this.activity    = activity;
		this.mobility    = mobility;
		this.config      = config;
		this.begin       = begin;
		this.end         = end;
		this.penetration = penetration;
		this.seed        = seed;
		this.hasPenetration = hasPenetration;
	}
}
