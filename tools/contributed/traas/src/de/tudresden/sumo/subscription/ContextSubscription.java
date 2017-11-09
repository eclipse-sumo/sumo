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

import java.util.LinkedList;

import de.tudresden.sumo.config.Constants;
import it.polito.appeal.traci.protocol.Command;

public class ContextSubscription implements Subscription  {

	public SubscriptionContext context;
	public int start;
	public int stop;
	public String objectID;
	public SumoDomain contextDomain;
	public double contextRange;
	public LinkedList<Integer> commands;
	
	public ContextSubscription(){
		
		//demo
		this.context = SubscriptionContext.lane;
		this.start = 0;
		this.stop = 100000 * 60;
		this.objectID = "gneE0_0";
		this.contextDomain = SumoDomain.vehicles;
		this.contextRange = 100;
		
		this.commands = new LinkedList<Integer>();
		this.commands.add(Constants.VAR_POSITION);
		this.commands.add(Constants.VAR_SPEED);
		
	}
	
	public ContextSubscription(SubscriptionContext context, int start, int stop, String objectID, SumoDomain contextDomain, double contextRange){
		
		this.context = context;
		this.start = start;
		this.stop = stop;
		this.objectID = objectID;
		this.contextDomain = contextDomain;
		this.contextRange = contextRange;
		this.commands = new LinkedList<Integer>();
	}
	
	public void addCommand(int cmd){this.commands.add(cmd);}
	
	public Command getCommand() {
		
		Command cmd = new Command(this.context.id);
		cmd.content().writeInt(this.start);
		cmd.content().writeInt(this.stop);
		cmd.content().writeStringASCII(this.objectID);
		cmd.content().writeUnsignedByte(this.contextDomain.id);
		cmd.content().writeDouble(this.contextRange);
		cmd.content().writeUnsignedByte(this.commands.size());
		for(Integer ix : this.commands){cmd.content().writeUnsignedByte(ix);}
		
		return cmd;
	}
	
}