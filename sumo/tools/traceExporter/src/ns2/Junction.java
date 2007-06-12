package ns2;

/**
 * model for junctions
 * @author Thimor Bohn <bohn@itm.uni-luebeck.de>
 *
 */
public class Junction {
	public String id;
	public float x;
	public float y;

	/**
	 * constructor
	 * @param id unique id of junction
	 * @param x x-coordinate of junction
	 * @param y y-coordinate of junction
	 */
	public Junction(String id, float x, float y)
	{
		this.id = id;
		this.x  = x;
		this.y  = y;
	}
}
