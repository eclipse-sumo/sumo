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

package it.polito.appeal.traci.protocol;

import java.util.List;

/**
 * Represents all the information related to a given request. It contains a
 * mandatory status response, an optional response command and an optional list
 * of sub-responses.
 * 
 * @author Enrico Gueli &lt;enrico.gueli@polito.it&gt;
 * 
 */
public class ResponseContainer {
	private final StatusResponse status;
	private final Command response;
	private final List<Command> subResponses;

	/**
	 * Constructor for a response container with status response, response
	 * command and zero or more sub-responses.
	 * 
	 * @param status
	 * @param response
	 * @param subResponses
	 */
	public ResponseContainer(StatusResponse status, Command response, 
			List<Command> subResponses) {
		this.status = status;
		this.response = response;
		this.subResponses = subResponses;
	}

	/**
	 * Constructor for a response container with status response and a response
	 * command.
	 * 
	 * @param status
	 * @param response
	 */
	public ResponseContainer(StatusResponse status, Command response) {
		this(status, response, null);
	}
	
	/**
	 * Returns the status response
	 * @return the status
	 */
	public StatusResponse getStatus() {
		return status;
	}

	/**
	 * @return the response command. It may return <code>null</code> if there is
	 *         no response to such command (i.e. a command that changes
	 *         something in the simulation state or the close command)
	 */
	public Command getResponse() {
		return response;
	}

	/**
	 * @return the sub-responses of this response container. It may return
	 *         <code>null</code> if there are no sub-responses to such command
	 *         (i.e. commands different than CMD_SIMSTEP2)
	 */
	public List<Command> getSubResponses() {
		return subResponses;
	}
	
	
}
