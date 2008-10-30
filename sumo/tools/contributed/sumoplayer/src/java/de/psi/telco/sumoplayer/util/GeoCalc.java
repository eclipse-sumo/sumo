package de.psi.telco.sumoplayer.util;


public class GeoCalc {
	
	public static double KM_PER_LAT = 111.12;
	public static final double LATITUDE_PER_KM = 1.0/111.12;
	public static final int EARTH_RADIUS_KM = 6371;
	
	public static double distance(Point a, Point b){
		return Math.sqrt( Math.pow(Math.abs(a.getX()-b.getX()),2) + Math.pow(Math.abs(a.getY()-b.getY()),2) );
	}
	public static double geoDistanceKm(Point a, Point b){
		return Math.acos(Math.sin(Math.toRadians(a.getY()))*Math.sin(Math.toRadians(b.getY())) + 
                Math.cos(Math.toRadians(a.getY()))*Math.cos(Math.toRadians(b.getY())) *
                Math.cos(Math.toRadians(b.getX())-Math.toRadians(a.getX()))) * (double)EARTH_RADIUS_KM;
	}
	public static double geoDistanceM(Point a, Point b){
		return (geoDistanceKm(a, b)*1000);
	}
	
	/**
	 * This function returns the minimum distance from Poit p to Line l.
	 * It uses SUNs implementation which uses a scalarproduct projection length
	 * to determin the minimal distance.
	 * @param p
	 * @param l
	 * @return
	 */
	public static double distance(Point p, Line l) {
		double x1 = l.getA().getX();
		double x2 = l.getB().getX();
		double y1 = l.getA().getY();
		double y2 = l.getB().getY();
		double px = p.getX();
		double py = p.getX();
		
		// Adjust vectors relative to x1,y1
		// x2,y2 becomes relative vector from x1,y1 to end of segment
		x2 -= x1;
		y2 -= y1;
		// px,py becomes relative vector from x1,y1 to test point
		px -= x1;
		py -= y1;
		double dotprod = px * x2 + py * y2;
		// dotprod is the length of the px,py vector
		// projected on the x1,y1=>x2,y2 vector times the
		// length of the x1,y1=>x2,y2 vector
		double projlenSq = dotprod * dotprod / (x2 * x2 + y2 * y2);
		// Distance to line is now the length of the relative point
		// vector minus the length of its projection onto the line
		double lenSq = px * px + py * py - projlenSq;
		if (lenSq < 0) {
		    lenSq = 0;
		}
		return Math.sqrt(lenSq);
	}
	
	public static double getLonOffset(double lon, double lat, int meters){
		double latDelta = (1.0/(KM_PER_LAT*1000))*meters;
		double lonFact = Math.cos(Math.toRadians(lat));
		return latDelta*lonFact;
	}
	
	public static double getLatOffset(double lon, double lat, int meters){
		return (1.0/(KM_PER_LAT*1000))*meters;
	}
}
