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

import java.io.DataOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.zip.Checksum;


import de.uniluebeck.itm.tcpip.Storage;

/**
 * Represents a TraCI messages used to send {@link Command}s from the client to
 * server (i.e. requests). After construction, the commands are appended one
 * after one with {@link #append(Command)}. The whole message, along with its
 * length header, can be then retrieved in a serialized form via the
 * {@link #writeTo(DataOutputStream)} method.
 * 
 * @author Enrico Gueli &lt;enrico.gueli@polito.it&gt;
 * @see <a
 *      href="https://sourceforge.net/apps/mediawiki/sumo/index.php?title=TraCI/Protocol#Messages">https://sourceforge.net/apps/mediawiki/sumo/index.php?title=TraCI/Protocol#Messages</a>
 */
public class RequestMessage {



	private final List<Command> commands = new ArrayList<Command>();

	/**
	 * Adds a command to the tail of this message.
	 * @param c c
	 * @throws NullPointerException if the command is <code>null</code>.
	 */
	public void append(Command c) {
		if (c == null)
			throw new NullPointerException("the command can't be null");
		commands.add(c);
	}

	/**
	 * Writes the commands to the specified {@link DataOutputStream} object, in
	 * the same order as the calls of {@link #append(Command)}.
	 * @param dos data output stream
	 * @throws IOException Exception
	 */
	public void writeTo(DataOutputStream dos) throws IOException {
		int totalLen = Integer.SIZE / 8; // the length header

		for (Command cmd : commands) {
			totalLen += cmd.rawSize();
		}

		Checksum checksum = null;
		dos.writeInt(totalLen);


		for (Command cmd : commands) {
			Storage s = new Storage();
			cmd.writeRawTo(s);
			writeStorage(s, dos, checksum);
		}

	}

	/**
	 * @param storage storage
	 * @param os os
	 * @param checksum checksum
	 * @throws IOException Exception
	 */
	private void writeStorage(Storage storage, OutputStream os, Checksum checksum)
			throws IOException {
		
		byte[] buf = new byte[storage.getStorageList().size()];
		int n = 0;
		for (Byte b : storage.getStorageList()) {
			if (checksum != null)
				checksum.update(b);
			
			buf[n] = b;
			n++;
		}
		os.write(buf);
	}

	public List<Command> commands() {
		return Collections.unmodifiableList(commands);
	}
}
