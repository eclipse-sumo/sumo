package ns2;

/**
 * model for lanes
 * @author Thimor Bohn <bohn@itm.uni-luebeck.de>
 *
 */
public class Lane {
	public String id;
	public float xfrom;
	public float xto;
	public float yfrom;
	public float yto;

	/**
	 * constructor
	 * @param id unique (in edge) id of lane
	 * @param xfrom x-coordinate at which vehicles start on the edge
	 * @param xto y-coordinate at which vehicles start on the edge
	 * @param yfrom x-coordinate at which vehicles leave the edge
	 * @param yto y-coordinate at which vehicles leave the edge
	 */
	public Lane(String id, float xfrom, float xto, float yfrom, float yto) {
		this.id    = id;
		this.xfrom = xfrom;
		this.xto   = xto;
		this.yfrom = yfrom;
		this.yto   = yto;
	}
}
