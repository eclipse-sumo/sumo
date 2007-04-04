package ns2;

import java.util.List;

/**
 * class for translating sumo net
 * needed for avoiding negative coordinates in ns2
 * @author Thimor Bohn <bohn@itm.uni-luebeck.de>
 *
 */
public class NetTranslater {
	/**
	 * method fro translating sumo net
	 * needed for avoiding negative coordinates in ns2
	 * @param edges contains sumo net
	 */
	public static void translate(List<Edge> edges) {
		// scan coordinates
        float xmin = edges.get(0).xfrom;
        xmin = Math.min(xmin, edges.get(0).xto);
        float ymin = edges.get(0).yfrom;
        ymin = Math.min(ymin, edges.get(0).yto);
		for (Edge edge: edges) {
			xmin = Math.min(xmin, edge.xfrom);
			xmin = Math.min(xmin, edge.xto);
			ymin = Math.min(ymin, edge.yfrom);
			ymin = Math.min(ymin, edge.yto);
		}
		// translate if needed
		if (xmin < 0 || ymin < 0) {
			for (Edge edge: edges) {
				edge.xfrom -= xmin;
				edge.xto   -= xmin;
				edge.yfrom -= ymin;
				edge.yto   -= ymin;
			}			
		}
	}
}
