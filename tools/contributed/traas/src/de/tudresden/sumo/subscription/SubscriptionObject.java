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

package de.tudresden.sumo.subscription;

import de.tudresden.ws.container.SumoObject;

public class SubscriptionObject {

	public String id;
	public ResponseType response;
	public int domain;
	public String name;
	public int variable;
	public int status;
	public int return_type;
	public SumoObject object;
	
	
		//context
		public SubscriptionObject(String id, ResponseType response, int domain, String name, int variable, int status, int return_type, SumoObject object){
			this.id = id;
			this.response = response;
			this.domain = domain;
			this.name = name;
			this.variable = variable;
			this.status = status;
			this.return_type = return_type;
			this.object = object;
		}
	
		//variable
		public SubscriptionObject(String id, ResponseType response, int variable, int status, int return_type, SumoObject object){
			this.id = id;
			this.response = response;
			this.variable = variable;
			this.status = status;
			this.return_type = return_type;
			this.object = object;
		}
	
}
