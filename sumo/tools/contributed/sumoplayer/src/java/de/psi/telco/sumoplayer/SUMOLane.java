package de.psi.telco.sumoplayer;

import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

import de.psi.telco.sumoplayer.util.GeoCalc;
import de.psi.telco.sumoplayer.util.Point;
import de.psi.telco.sumoplayer.util.PointImpl;
import de.psi.telco.sumoplayer.util.Vector;

public class SUMOLane {

	public String id;
	public List<Point> points = new LinkedList<Point>();
	
	private double length = -1;	// not known
	
	public SUMOLane(String id){
		this.id=id;
	}
	
	public double getLength(){
		if (length == -1){
			length = 0;
			Iterator i = points.iterator();
			Point a = null;
			Point b = null;
			while (i.hasNext()){
				if (a == null){
					a = (Point)i.next();
					continue;
				}
				b = (Point)i.next();

				length += GeoCalc.distance(a, b);

				a = b;
			}
			return length;
		}else{
			return length;
		}
	}
	
	public Point getPositionWithin(double pos){
		//System.out.println("GetPosWithin: laneId:"+this.id+" - req pos: "+pos+" - cal len:"+this.getLength());
		if (this.getLength()<=pos){	// near end. returning last point	
			return points.get(points.size()-1);		// just return last point
		}
		try{
			int idx = 0;
			double len = 0;
			Iterator i = points.iterator();
			Point a = null;
			Point b = null;
			boolean foundWithin = false;	
			while (i.hasNext()){
				if (a == null){
					a = (Point)i.next();
					continue;
				}
				
				b = (Point)i.next();
				if (len+GeoCalc.distance(a,b)<pos){
					len += GeoCalc.distance(a,b);
					idx++;
				}else{
					foundWithin = true;	
					break;
				}
				
				a = b;
			}
			
			// sometimes requested positions lies a bit outside the lane. dont know why.
			if (foundWithin){	// TODO: check if this is just a sumo mistake
				PointImpl startPos = new PointImpl(points.get(idx).getX(),points.get(idx).getY());
				PointImpl endPos = new PointImpl(points.get(idx+1).getX(),points.get(idx+1).getY());
				double partialLen = startPos.distance(endPos);
				double rest = pos-len;
				double fact = rest/partialLen;
				
				Vector out = startPos.add((endPos.sub(startPos).scale(fact)));
				return new PointImpl(out.getX(),out.getY());
			}else{
				return points.get(points.size()-1);		// just return last point
			}
		
		}catch(Exception e){
			e.printStackTrace();
			return new PointImpl(0,0);
		}
	}
}
