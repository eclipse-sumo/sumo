/*   
    Copyright (C) 2013 Mario Krumnow, Dresden University of Technology

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

public class SumoLink {

	public String from;
	public String to;
	public String over;
	
	public SumoLink(){
		
		from = "unknown";
		to = "unknown";
		over = "unknown";
	}
	
	public SumoLink(String from, String to,	String over){
		this.from = from;
		this.to = to;
		this.over = over;
	}
	
	
}
