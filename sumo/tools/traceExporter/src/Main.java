/**
 * Main class of application
 * @author Thimor Bohn <bohn@itm.uni-luebeck.de>
 *
 */
public class Main {

	/**
	 * Entry point of application
	 * @param args
	 */
	public static void main(String[] args) {
		boolean isNs2 = false;
		boolean isAnsim = false;
		// choose one of two possible actions
		for (String arg: args) {
			// convert to ns2
			if ("ns2".equals(arg.toLowerCase())) {
				isNs2    = true;
			}
			// convert to ansim
			if ("ansim".equals(arg.toLowerCase())) {
				isAnsim = true;
			}
		}
		
		if ( (isNs2 || isAnsim) == false ) {
			// no target selected
			System.err.println("Which target wanted? => [ns2|ansim]");
			System.exit(1);
		}
		
		if ( (isNs2 && isAnsim) == true) {
			// two targets selected
			System.err.println("Which target wanted? => [ns2|ansim]");
			System.exit(1);			
		}
		
		if (isNs2) {
			// convert!
			ns2.Converter.convert(args);
		} else if (isAnsim) {
			ansim.Converter.convert(args);
		}
	}
}
