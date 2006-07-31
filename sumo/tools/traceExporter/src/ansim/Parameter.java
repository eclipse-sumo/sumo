package ansim;

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
	 * @param out name of out file
	 */
	public Parameter(String net, String trace, String out) {
		this.net   = net;
		this.trace = trace;
		this.out   = out;
	}

	public String net;
	public String trace;
	public String out;
}
