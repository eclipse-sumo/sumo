/*   
    Copyright (C) 2016 Mario Krumnow, Dresden University of Technology

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

package de.tudresden.ws;

/**
 * 
 * @author Mario Krumnow
 *
 */

public class ShutdownHook  extends Thread
{

	private boolean shutdown;
	
	/**
	 * Instantiates a new shutdown hook.
	 */
	public ShutdownHook(){setshutdown(false);}
	
    public void run() 
    {
        System.out.println("Shutdown in progress...");
        
        setshutdown(true);
        
		try
		{Thread.sleep(3000);} catch (Exception e){}

        System.out.println("Shutdown finsihed");

    }


	public void setshutdown(boolean shutdown) {this.shutdown = shutdown;}

	public boolean isshutdown() {return shutdown;}
	
	
}
