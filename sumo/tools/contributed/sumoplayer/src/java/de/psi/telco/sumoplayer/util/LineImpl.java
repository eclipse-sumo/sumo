package de.psi.telco.sumoplayer.util;

public class LineImpl implements Line {
	private Point a;
	private Point b;
	
	public LineImpl(double ax, double ay, double bx, double by){
		this.a = new PointImpl(ax,ay);
		this.b = new PointImpl(bx,by);
	}
	public LineImpl(Point a, Point b){
		this.a = a;
		this.b = b;
	}
	
	/* (non-Javadoc)
	 * @see de.psi.telco.car4car.dto.geo.Line#getA()
	 */
	public Point getA(){
		return a;
	}
	/* (non-Javadoc)
	 * @see de.psi.telco.car4car.dto.geo.Line#getB()
	 */
	public Point getB(){
		return b;
	}
}
