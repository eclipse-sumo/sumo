/*   
    Copyright (C) 2017 Mario Krumnow, Dresden University of Technology

    This file is part of TraaS.

    TraaS is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    TraaS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with TraaS.  If not, see <http://www.gnu.org/licenses/>.
    
*/

package de.tudresden.ws.container;

/**
 * 
 * @author Mario Krumnow
 *
 */

public class SumoBoundingBox implements SumoObject {
	
	public double x_min;
	public double y_min;
	public double x_max;
	public double y_max;

	
	public SumoBoundingBox(){
		this.x_min = 0;
		this.y_min = 0;
		this.x_max = 0;
		this.y_max = 0;
	}
	
	public SumoBoundingBox( double x_min, double y_min, double x_max, double y_max){
		
		this.x_min = x_min;
		this.y_min = y_min;
		this.x_max = x_max;
		this.y_max = y_max;
		
	}
	
	public String toString(){
		return this.x_min +"#"+this.y_min+"#"+this.x_max+"#"+this.y_max;
	}
	
	
}
