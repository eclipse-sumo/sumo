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