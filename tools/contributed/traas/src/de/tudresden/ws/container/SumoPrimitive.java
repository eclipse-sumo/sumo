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

public class SumoPrimitive implements SumoObject {

	public Object val;
	public String type;
	
	public SumoPrimitive(Object o){
		
		this.val = o;
		if(o.getClass().equals(Integer.class)){
			type = "integer";
		}else if(o.getClass().equals(Double.class)){
			type = "double";
		}else if(o.getClass().equals(String.class)){
			type = "string";
		}else{
			System.err.println("unsupported format " + o.getClass());
		}
		
	}
	
}
