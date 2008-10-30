package de.psi.telco.sumoplayer.util;

public class PointImpl implements Point, Vector{
	
	double x;
	double y;
	
	public PointImpl(double x, double y) {
		this.x = x;
		this.y = y;
	}
	
	public double distance(Point b) {
		return GeoCalc.distance(this,b);
	}
	public double distance(Line l) {
		return GeoCalc.distance(this,l);
	}

	public double getX() {
		return x;
	}

	public double getY() {
		return y;
	}
	
	public String toString(){
		return "<Point x=\""+x+"\" y="+y+"\"/>";
	}
	
	public Vector add(Vector b) {
		return new PointImpl(x+b.getX(),y+b.getY());
	}

	public Vector sub(Vector b) {
		return new PointImpl(x-b.getX(),y-b.getY());
	}

	public Vector scale(double scalar) {
		return new PointImpl(x*scalar,y*scalar);
	}
}
