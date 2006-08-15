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
	 * @param penetration value in [0,1] of penetration factor
	 */
	public Parameter(String net, String trace, String activity, String mobility, double penetration) {
		this.net         = net;
		this.trace       = trace;
		this.activity    = activity;
		this.mobility    = mobility;
		this.penetration = penetration;
	}

	public String net;
	public String trace;
	public String activity;
	public String mobility;
	public double penetration;
}
