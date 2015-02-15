/*   
    Copyright (C) 2015 Mario Krumnow, Dresden University of Technology

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

package de.tudresden.sumo.util;

import it.polito.appeal.traci.TraCIException.UnexpectedData;
import it.polito.appeal.traci.protocol.Command;
import it.polito.appeal.traci.protocol.ResponseContainer;

import java.io.IOException;
import java.net.Socket;

import de.tudresden.sumo.config.Constants;
import de.tudresden.ws.container.SumoBoundingBox;
import de.tudresden.ws.container.SumoColor;
import de.tudresden.ws.container.SumoGeometry;
import de.tudresden.ws.container.SumoLink;
import de.tudresden.ws.container.SumoLinkList;
import de.tudresden.ws.container.SumoPosition2D;
import de.tudresden.ws.container.SumoPosition3D;
import de.tudresden.ws.container.SumoStringList;
import de.tudresden.ws.container.SumoTLSLogic;
import de.tudresden.ws.container.SumoTLSPhase;

/**
 * 
 * @author Mario Krumnow
 *
 */

public class CommandProcessor extends Query{
 
	int temp;
	public CommandProcessor(Socket sock) throws IOException {
		super(sock);
	}
	
	public synchronized void do_job_set(SumoCommand sc) throws IOException{
		queryAndVerifySingle(sc.cmd);
	}
	
	public synchronized Object do_job_get(SumoCommand sc) throws IOException{
		
		Object output = null;
		ResponseContainer rc = queryAndVerifySingle(sc.cmd);
		Command resp = rc.getResponse();
	
		verifyGetVarResponse(resp, sc.response, sc.input2, sc.input3);
		verify("", sc.output_type, (int)resp.content().readUnsignedByte());
		
		if(sc.output_type == Constants.TYPE_INTEGER){output = resp.content().readInt();
		}else if(sc.output_type == Constants.TYPE_DOUBLE){output = resp.content().readDouble();
		}else if(sc.output_type == Constants.TYPE_STRING){output = resp.content().readStringUTF8();
		}else if(sc.output_type == Constants.POSITION_2D){
			double x = resp.content().readDouble();
			double y = resp.content().readDouble();
			output = new SumoPosition2D(x,y);
		}else if(sc.output_type == Constants.POSITION_3D){
			double x = resp.content().readDouble();
			double y = resp.content().readDouble();
			double z = resp.content().readDouble();
			output = new SumoPosition3D(x,y,z);
		}else if(sc.output_type == Constants.TYPE_STRINGLIST){
			
			SumoStringList ssl = new SumoStringList();
			int laenge = resp.content().readInt();
			for(int i=0; i<laenge; i++){
				ssl.add(resp.content().readStringASCII());
			}
			output = ssl;
		
		}else if(sc.output_type == Constants.TYPE_BOUNDINGBOX){
			
			double min_x = resp.content().readDouble();
			double min_y = resp.content().readDouble();
			double max_x = resp.content().readDouble();
			double max_y = resp.content().readDouble();
			
			output = new SumoBoundingBox(min_x, min_y, max_x, max_y);
			
		}else if(sc.output_type == Constants.TYPE_COMPOUND){
			
			Object[] obj = null;
			
			//decision making
			if(sc.input2 == Constants.TL_CONTROLLED_LINKS){
				
				SumoLinkList sll = new SumoLinkList();
				
				//read length
				resp.content().readUnsignedByte();
				resp.content().readInt();
				
				int laenge = resp.content().readInt();
				obj = new StringList[laenge];
				
				for(int i=0; i<laenge; i++){
				
					resp.content().readUnsignedByte();
					int anzahl = resp.content().readInt();
					
					for(int i1=0; i1<anzahl; i1++){
						
						resp.content().readUnsignedByte();
						resp.content().readInt(); //length
						
						String from = resp.content().readStringASCII();
						String to = resp.content().readStringASCII();
						String over = resp.content().readStringASCII();
						sll.add(new SumoLink(from, to, over));
						
					}
					
				}
				
				output = sll;
			
			}else if(sc.input2 == Constants.TL_COMPLETE_DEFINITION_RYG){
				
				resp.content().readUnsignedByte();
				resp.content().readInt();
				
				int length = resp.content().readInt();
				for(int i=0; i<length; i++){
					
					resp.content().readUnsignedByte();
					String subID = resp.content().readStringASCII();
					
					resp.content().readUnsignedByte();
					int type = resp.content().readInt();
					
					resp.content().readUnsignedByte();
					int subParameter = resp.content().readInt();
					
					resp.content().readUnsignedByte();
					int currentPhaseIndex = resp.content().readInt();
					
					SumoTLSLogic stl = new SumoTLSLogic(subID, type, subParameter, currentPhaseIndex);
					
					resp.content().readUnsignedByte();
					int nbPhases = resp.content().readInt();
					
					for(int i1=0; i1<nbPhases; i1++){
						
						resp.content().readUnsignedByte();
						int duration = resp.content().readInt();
						
						resp.content().readUnsignedByte();
						int duration1 = resp.content().readInt();
						
						resp.content().readUnsignedByte();
						int duration2 = resp.content().readInt();
						
						resp.content().readUnsignedByte();
						String phaseDef = resp.content().readStringASCII();
						
						stl.add(new  SumoTLSPhase(duration, duration1, duration2, phaseDef));
						
					}

					output = stl;
					
				}

				
			}else if(sc.input2 == Constants.LANE_LINKS)
			{
			
				resp.content().readUnsignedByte();
				resp.content().readInt();
				
				//number of links
				int length = resp.content().readInt();
				SumoLinkList links = new SumoLinkList();
				for(int i=0; i<length; i++){
					
					resp.content().readUnsignedByte();
					String notInternalLane = resp.content().readStringASCII();
					
					resp.content().readUnsignedByte();
					String internalLane = resp.content().readStringASCII();
					
					resp.content().readUnsignedByte();
					byte hasPriority = (byte)resp.content().readUnsignedByte();
					
					resp.content().readUnsignedByte();
					byte isOpened = (byte)resp.content().readUnsignedByte();
					
					resp.content().readUnsignedByte();
					byte hasFoes = (byte)resp.content().readUnsignedByte();
					
					//not implemented
					resp.content().readUnsignedByte();
					String state = resp.content().readStringASCII();
					
					resp.content().readUnsignedByte();
					String direction = resp.content().readStringASCII();
					
					resp.content().readUnsignedByte();
					double laneLength = resp.content().readDouble();
					
					
					links.add(new SumoLink(notInternalLane,internalLane,hasPriority,isOpened,hasFoes,laneLength, state, direction));
				}
				output = links;
			}
			else{
				
				int laenge = resp.content().readInt();
				obj = new Object[laenge];
				
				for(int i=0; i<laenge; i++){
					
					int k = resp.content().readUnsignedByte();
					obj[i] = this.get_value(k, resp);
					
				}
				
				output = obj;
			}
			
			
		}
		else if(sc.output_type == Constants.TYPE_POLYGON){
			
			int laenge = resp.content().readUnsignedByte();
			
			SumoGeometry sg = new SumoGeometry();
			for(int i=0; i<laenge; i++){
				double x =  (Double) this.get_value(Constants.TYPE_DOUBLE, resp);
				double y = (Double) this.get_value(Constants.TYPE_DOUBLE, resp);
				sg.add(new SumoPosition2D(x,y));
			}
			
			output = sg;
		
		}
		else if(sc.output_type == Constants.TYPE_COLOR){
			
			int r = resp.content().readUnsignedByte();
			int g = resp.content().readUnsignedByte();
			int b = resp.content().readUnsignedByte();
			int a = resp.content().readUnsignedByte();
			
			output = new SumoColor(r, g, b, a);
		
		}else if(sc.output_type == Constants.TYPE_UBYTE){
			
			output = resp.content().readUnsignedByte();
			
		}
		
		
			
		return output;
	}

	private Object get_value(int code, Command resp){
		
		Object obj = -1;
		
		if(code == Constants.TYPE_STRING) {
			obj = resp.content().readStringASCII();		
		}else if (code == Constants.TYPE_INTEGER) {
			obj = resp.content().readInt();
		}else if (code == Constants.TYPE_UBYTE) {
			obj = resp.content().readUnsignedByte();
		}else if (code == Constants.TYPE_DOUBLE) {
			obj = resp.content().readDouble();
		}else {
			System.out.println("unknown: " + code);
		}
		
		return obj;
	}
	
	protected static String verifyGetVarResponse(Command resp, int commandID, int variable, String objectID) throws UnexpectedData {
		verify("response code", commandID, resp.id());
		verify("variable ID", variable, (int)resp.content().readUnsignedByte());
		String respObjectID = resp.content().readStringASCII();
		if (objectID != null) {
			verify("object ID", objectID, respObjectID);
		}
		return respObjectID;
	}

	
}
