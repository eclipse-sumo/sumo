/*   
    Copyright (C) 2011 ApPeAL Group, Politecnico di Torino

    This file is part of TraCI4J.

    TraCI4J is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    TraCI4J is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with TraCI4J.  If not, see <http://www.gnu.org/licenses/>.
*/

package de.tudresden.sumo.util;

import it.polito.appeal.traci.TraCIException;
import it.polito.appeal.traci.TraCIException.UnexpectedData;
import it.polito.appeal.traci.protocol.Command;
import it.polito.appeal.traci.protocol.RequestMessage;
import it.polito.appeal.traci.protocol.ResponseMessage;
import it.polito.appeal.traci.protocol.ResponseContainer;
import it.polito.appeal.traci.protocol.StatusResponse;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.util.List;

import de.tudresden.sumo.config.Constants;

public abstract class Query {
	private final DataOutputStream outStream; 
	private final DataInputStream inStream; 
	
	public Query(Socket sock) throws IOException {
		outStream = new DataOutputStream(sock.getOutputStream());
		inStream = new DataInputStream(sock.getInputStream());
	}

	/**
	 * Sends a request message to SUMO and returns a response message. 
	 * @param msg
	 * @throws IOException
	 */
	protected ResponseMessage doQuery(RequestMessage msg) throws IOException {
		msg.writeTo(getOutStream());
		return new ResponseMessage(inStream);
	}

	/**
	 * Like {@link #doQuery(RequestMessage)}; in addition, verifies that all
	 * responses are successful and and the statuses match the requests.
	 * @param reqMsg
	 * @return the verified response message
	 * @throws IOException
	 * @see #doQuery(RequestMessage)
	 */
	protected ResponseMessage queryAndVerify(RequestMessage reqMsg) throws IOException {
		
		reqMsg.writeTo(getOutStream());
		
		ResponseMessage respMsg = new ResponseMessage(inStream);

		List<Command> commands = reqMsg.commands();
		List<ResponseContainer> responses = respMsg.responses();
		if (commands.size() > responses.size())
			throw new TraCIException("not enough responses received");
		
		for (int i=0; i<commands.size(); i++) {
			Command cmd = commands.get(i);
			ResponseContainer responsePair = responses.get(i);
			StatusResponse statusResp = responsePair.getStatus();
			verify("command and status IDs match", cmd.id(), statusResp.id());
			if (statusResp.result() != Constants.RTYPE_OK)
				throw new TraCIException("SUMO error for command "
						+ statusResp.id() + ": " + statusResp.description());
		}
		
		return respMsg;
	}

	/**
	 * Like {@link #doQuery(RequestMessage)}, but good for one-command/
	 * one-response queries.
	 * 
	 * @param request
	 * @throws IOException
	 */
	protected ResponseContainer doQuerySingle(Command request) throws IOException {
		RequestMessage msg = new RequestMessage();
		msg.append(request);
		ResponseMessage resp = doQuery(msg);
		return resp.responses().iterator().next();
	}

	/**
	 * Like {@link #queryAndVerify(RequestMessage)}, but good for one-command/
	 * one-response queries.
	 * 
	 * @param request
	 * @return the response container for the specified request
	 * @throws IOException
	 */
	protected ResponseContainer queryAndVerifySingle(Command request) throws IOException {
		RequestMessage msg = new RequestMessage();
		msg.append(request);
		ResponseMessage resp = queryAndVerify(msg);
		return resp.responses().iterator().next();
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

	protected static void verify(String description, Object expected, Object actual) throws UnexpectedData {
		if(!actual.equals(expected))
			throw new UnexpectedData(description, expected, actual);
	}

	protected static void verify(String description, int expected, short actual) throws UnexpectedData {
		verify(description, expected, (int)actual);
	}

	protected static void verify(String description, int expected, byte actual) throws UnexpectedData {
		verify(description, expected, (int)actual);
	}

	public DataOutputStream getOutStream() {
		return outStream;
	}

}
