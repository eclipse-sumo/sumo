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

public class VariableSubscription implements Subscription  {

	SubscribtionVariable sv;
	public int start;
	public int stop;
	public String objectID;
	public LinkedList<Integer> commands;
	
	public VariableSubscription(){
		
		//demo
		this.sv = SubscribtionVariable.lane;
		this.start = 0;
		this.stop = 100000 * 60;
		this.objectID = "gneE0_0";
		this.commands = new LinkedList<Integer>();
		this.commands.add(Constants.LAST_STEP_VEHICLE_NUMBER);
		
	}
	
	public VariableSubscription(SubscribtionVariable sv, int start, int stop, String objectID){
		
		this.sv = sv;
		this.start = start;
		this.stop = stop;
		this.objectID = objectID;
		this.commands = new LinkedList<Integer>();
	}
	
	public void addCommand(int cmd){this.commands.add(cmd);}
	
	public Command getCommand() {
		
		Command cmd = new Command(this.sv.id);
		cmd.content().writeInt(this.start);
		cmd.content().writeInt(this.stop);
		cmd.content().writeStringASCII(this.objectID);
		cmd.content().writeUnsignedByte(this.commands.size());
		for(Integer ix : this.commands){cmd.content().writeUnsignedByte(ix);}
		
		return cmd;
	}
	
}