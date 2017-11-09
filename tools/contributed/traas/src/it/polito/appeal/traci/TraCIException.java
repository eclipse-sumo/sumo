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

import java.io.IOException;

@SuppressWarnings("serial")
public class TraCIException extends IOException {
	
	public TraCIException() {
		super();
	}

	public TraCIException(String msg) {
		super(msg);
	}

	public static class UnexpectedData extends TraCIException {
		public UnexpectedData(String what, Object expected, Object got) {
			super("Unexpected " + what + ": expected " + expected + ", got " + got);
		}
	}
	
	public static class UnexpectedDatatype extends UnexpectedData {
		public UnexpectedDatatype(int expected, int got) {
			super("datatype", expected, got);
		}
	}

	public static class UnexpectedResponse extends UnexpectedData {
		public UnexpectedResponse(int expected, int got) {
			super("response", expected, got);
		}
	}

}
