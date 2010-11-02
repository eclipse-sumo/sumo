package ns2;

/**
 * model for lanes
 * @author Thimor Bohn <bohn@itm.uni-luebeck.de>
 * @author Matthias Röckl <matthias.roeckl@dlr.de>
 *
 */
public class Lane {
	public String id;
	public float[] x;
	public float[] y;
	public float length;

	/**
	 * constructor
	 * @param id unique (in edge) id of lane
	 * @param x array of x-coordinates ( for each point on the shape starting with the starting point)
	 * @param y array of y-coordinates ( for each point on the shape starting with the starting point)
	 * @param length length of lane
	 */
	public Lane(String id, float[] x, float[] y, float length) {
		this.id       = id;
		this.x = x;
		this.y = y;
		this.length   = length;
	}
	
	/**
	 * Returns a position, i.e. an array of floats [x,y], with a given distance from the start point of this lane.
	 *
	 * @param distance  distance from the start point of this lane
	 * @return position as array of floats [x,y]
	 */
	public float[] getPositionOnLane(float distance) {
		float[] xAndY = new float[] {this.x[0],this.y[0]};   // create return value and initialize it with the start position of the lane
		float accSegmentLength = 0;  // accumulated lane segment length
		float segmentLength = 0;   // the length of the current segment
		int s=0;   // index for lane segments
		
		// Iterate over the segments until the correct segment is found. The id of the endpoint of the right segment is stored in the s variable.
		do {
			segmentLength = (float)Math.sqrt((x[s+1]-x[s])*(x[s+1]-x[s]) + (y[s+1]-y[s])*(y[s+1]-y[s]));
			accSegmentLength += segmentLength;
			s++;
		}
		while (distance > accSegmentLength && s < x.length-1);   // s has to be limited manually since the final accSegmentLength is somtimes smaller than x.length (must be a different calculation method)
		
		float distanceOnCurrentSegment = distance - (accSegmentLength - segmentLength);
		xAndY[0] = x[s-1] + distanceOnCurrentSegment * (x[s] - x[s-1]) / segmentLength;     // segmentLength is the length of the current segment, x[s] - x[s-1] is the length of the current segment along the x axis
		xAndY[1] = y[s-1] + distanceOnCurrentSegment * (y[s] - y[s-1]) / segmentLength;
		
		return xAndY;
	}
}
