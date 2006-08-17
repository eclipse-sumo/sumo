package ns2;

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
		String net         = "";
		String trace       = "";
		String activity    = "";
		String mobility    = "";
		String config      = "";
		double penetration = 0;
		
		boolean hasNet         = false;
		boolean hasTrace       = false;
		boolean hasActivity    = false;
		boolean hasMobility    = false;
		boolean hasPenetration = false;
		boolean hasConfig      = false;
		
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
			
			if ("-a".equals(args[i])) {
				// activity file
				if (++i<args.length) {
					if (!args[i].startsWith("-")) {
						activity = args[i];
						hasActivity = true;
					}
				}
			}
			
			if ("-m".equals(args[i])) {
				// mobility file
				if (++i<args.length) {
					if (!args[i].startsWith("-")) {
						mobility = args[i];
						hasMobility = true;
					}
				}
			}
			
			if ("-c".equals(args[i])) {
				// config file
				if (++i<args.length) {
					if (!args[i].startsWith("-")) {
						config = args[i];
						hasConfig = true;
					}
				}
			}
			
			if ("-p".equals(args[i])) {
				// penetration factor
				if (++i<args.length) {
					if (!args[i].startsWith("-")) {
						penetration = Double.parseDouble(args[i]);
						if (penetration>=0 && penetration<=1) {
							hasPenetration = true;
						} else {
							penetration = 0;
						}
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
		
		// must have activity file argument
		if (!hasActivity) {
			throw new IllegalArgumentException("no activity specified!");
		}
		
		// must have mobility file argument
		if (!hasMobility) {
			throw new IllegalArgumentException("no mobility specified!");
		}
		
		// must have config file argument
		if (!hasConfig) {
			throw new IllegalArgumentException("no config specified!");
		}
		
		// must have penetration argument
		if (!hasPenetration) {
			throw new IllegalArgumentException("no penetration specified!");
		}
		
		return new Parameter(net, trace, activity, mobility, config, penetration);
	}
}
