package ansim;

import ansim.Parameter;

/**
 * class for parsing command line arguments
 * @author Thimor Bohn <bohn@itm.uni-luebeck.de>
 *
 */
public class Parser {
	/**
	 * method for parsing command line arguments
	 * @param args command line arguments
	 * @return parsed arguments
	 * @throws IllegalArgumentException
	 */
	public static Parameter parse(String[] args) throws IllegalArgumentException {
		String net   = "";
		String trace = "";
		String out   = "";
		
		boolean hasNet   = false;
		boolean hasTrace = false;
		boolean hasOut   = false;
		
		for (int i=0; i<args.length; i++) {
			if ("ns2".equals(args[i].toLowerCase())) {
			}
			
			if ("-n".equals(args[i])) {
				// net file
				if (++i<args.length) {
					if (!args[i].startsWith("-")) {
						net = args[i];
						hasNet = true;
					}
				}
			}
			
			if ("-t".equals(args[i])) {
				// trace file
				if (++i<args.length) {
					if (!args[i].startsWith("-")) {
						trace = args[i];
						hasTrace = true;
					}
				}
			}
			
			if ("-o".equals(args[i])) {
				// out file
				if (++i<args.length) {
					if (!args[i].startsWith("-")) {
						out = args[i];
						hasOut = true;
					}
				}
			}
		}
		
		// must have net file argument
		if (!hasNet) {
			throw new IllegalArgumentException("no netfile specified!");
		}
		
		// must have trace file argument
		if (!hasTrace) {
			throw new IllegalArgumentException("no tracefile specified!");
		}
		
		// must have out file argument
		if (!hasOut) {
			throw new IllegalArgumentException("no outfile specified!");
		}
				
		return new Parameter(net, trace, out);
	}
}
