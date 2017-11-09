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

package it.polito.appeal.traci;

import it.polito.appeal.traci.protocol.Command;

import java.io.IOException;
import java.net.Socket;

import de.tudresden.sumo.config.Constants;
import de.tudresden.sumo.util.Query;


public class CloseQuery extends Query {

	public CloseQuery(Socket sock) throws IOException {
		super(sock);
	}

	public void doCommand() throws IOException {
		Command req = new Command(Constants.CMD_CLOSE);
		queryAndVerifySingle(req);
	}
}
