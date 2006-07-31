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
	 */
	public Parameter(String net, String trace, String activity, String mobility) {
		this.net      = net;
		this.trace    = trace;
		this.activity = activity;
		this.mobility = mobility;
	}

	public String net;
	public String trace;
	public String activity;
	public String mobility;
}
