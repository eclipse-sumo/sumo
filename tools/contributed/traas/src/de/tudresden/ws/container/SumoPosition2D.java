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

public class SumoPosition2D implements SumoObject {

	public double x;
	public double y;
	
	public SumoPosition2D(){
		this.x = 0;
		this.y = 0;
	}
	
	public SumoPosition2D(double x, double y){
		this.x = x;
		this.y = y;
	}
	
	public String toString(){
		return this.x + "," + this.y;
	}
	
}
