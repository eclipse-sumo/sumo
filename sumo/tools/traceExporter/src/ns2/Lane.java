package ns2;

/**
 * model for lanes
 * @author Thimor Bohn <bohn@itm.uni-luebeck.de>
 *
 */
public class Lane {
	/**
	 * constructor
	 * @param id
	 * @param xfrom
	 * @param xto
	 * @param yfrom
	 * @param yto
	 */
	public Lane(String id, float xfrom, float xto, float yfrom, float yto) {
		this.id    = id;
		this.xfrom = xfrom;
		this.xto   = xto;
		this.yfrom = yfrom;
		this.yto   = yto;
	}
	
	public String id;
	
	public float xfrom;
	
	public float xto;
	
	public float yfrom;
	
	public float yto;
}
