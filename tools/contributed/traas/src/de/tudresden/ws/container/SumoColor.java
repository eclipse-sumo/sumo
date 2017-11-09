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

public class SumoColor implements SumoObject {

	public byte r;
	public byte g;
	public byte b;
	public byte a;
	
	public SumoColor(){
		this.r = (byte) 100;
		this.g = (byte) 100;
		this.b = (byte) 100;
		this.a = (byte) 100;
	}
	
	public SumoColor(int r, int g, int b, int a){
		this.r = (byte) r;
		this.g = (byte) g;
		this.b = (byte) b;
		this.a = (byte) a;
	}
	
	public String toString(){
		return r+"#"+g+"#"+b+"#"+a;
	}
	
	
}
