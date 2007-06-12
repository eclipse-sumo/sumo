package ns2;

import java.util.List;
import java.util.Map;

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
	public static void translate(List<Edge> edges, Map<String, Junction> junctions) {
		// scan coordinates
		float xmin = 0;
		float ymin = 0;
		boolean first = true;
		for (Junction junction: junctions.values())
		{
			if (first)
			{
				first = false;
				xmin  = junction.x;
				ymin  = junction.y;
			}
			else
			{
				xmin = Math.min(xmin, junction.x);
				ymin = Math.min(ymin, junction.y);
			}
		}
		// translate if needed
		if (xmin < 0 || ymin < 0) {
			for (Junction junction: junctions.values())
			{
				junction.x -= xmin;
				junction.y -= ymin;
			}
			for (Edge edge: edges) {
				for (Lane lane: edge.lanes.values())
				{
					lane.xfrom -= xmin;
					lane.xto   -= xmin;
					lane.yfrom -= ymin;
					lane.yto   -= ymin;
				}
			}			
		}
	}
}
