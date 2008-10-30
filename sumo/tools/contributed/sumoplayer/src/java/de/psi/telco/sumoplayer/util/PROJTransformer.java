package de.psi.telco.sumoplayer.util;

import java.awt.geom.Point2D.Double;

import com.jhlabs.map.proj.Projection;
import com.jhlabs.map.proj.ProjectionFactory;

public class PROJTransformer {
	private String projString = null;
	private Vector offset = null;
	private Projection proj = null;
	
	public PROJTransformer(String projString, Vector offset){
		this.projString = projString;
		this.offset = offset;
		if (this.projString != null){
			this.proj = ProjectionFactory.fromPROJ4Specification(this.projString.split(" "));
		}
	}
	
	public PROJTransformer(String projString){
		this.projString = projString;
		this.offset = null;
		if (this.projString != null){
			this.proj = ProjectionFactory.fromPROJ4Specification(this.projString.split(" "));
		}
	}
	
	public Point transformInvers(Point input){
		PointImpl output = new PointImpl(input.getX(),input.getY());
		if (offset != null){	// correct offset it required
			output = new PointImpl(output.getX()-offset.getX(),output.getY()-offset.getY());
		}
		if (proj != null){
			// magic
			Double projout = new Double();
			proj.inverseTransform(new Double(output.getX(),output.getY()), projout);
			output = new PointImpl(projout.x,projout.y);
		}
		return output;
	}
	
	public Point transform(Point input){
		PointImpl output = new PointImpl(input.getX(),input.getY());
		if (proj != null){
			// magic
			Double projout = new Double();
			proj.transform(new Double(output.getX(),output.getY()), projout);
			output = new PointImpl(projout.x,projout.y);
		}
		if (offset != null){	// correct offset it required
			output = new PointImpl(output.getX()+offset.getX(),output.getY()+offset.getY());
		}
		return output;
	}
	
	public Point transform(double lon, double lat){
		PointImpl output = new PointImpl(lon, lat);
		if (proj != null){
			// magic
			Double projout = new Double();
			proj.transform(new Double(lon,lat), projout);
			output = new PointImpl(projout.x,projout.y);
		}
		if (offset != null){	// correct offset it required
			output = new PointImpl(output.getX()+offset.getX(),output.getY()+offset.getY());
		}
		return output;
	}
}
