/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2023 German Aerospace Center (DLR) and others.
// TraCI4J module
// Copyright (C) 2011 ApPeAL Group, Politecnico di Torino
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    SumoTraciConnection.java
/// @author  Enrico Gueli
/// @author  Mario Krumnow
/// @date    2011
///
//
/****************************************************************************/
package it.polito.appeal.traci;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.ConnectException;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketAddress;
import java.util.ArrayList;
import java.util.List;

import de.tudresden.sumo.subscription.Subscription;
import de.tudresden.sumo.util.CommandProcessor;
import de.tudresden.sumo.util.Observer;
import de.tudresden.sumo.util.SumoCommand;

/**
 * Models a TCP/IP connection to a local or remote SUMO server via the TraCI
 * protocol.
 *
 * @author Enrico Gueli &lt;enrico.gueli@gmail.com&gt;
 * @author Mario Krumnow
 *
 */
public class SumoTraciConnection {

    /**
     * Reads an InputStream object and logs each row in the containing class's
     * logger.
     *
     * @author Enrico
     *
     */

    static boolean stdout = false;
    
    /** The stderr. */
    static boolean stderr = true;

    /**
     * The Class StreamLogger.
     */
    private static class StreamLogger implements Runnable {
        
        /** The stream. */
        final InputStream stream;
        
        /** The prefix. */
        @SuppressWarnings("unused")
        final String prefix;

        /**
         * Instantiates a new stream logger.
         *
         * @param stream the stream
         * @param prefix the prefix
         */
        public StreamLogger(InputStream stream, String prefix) {
            this.stream = stream;
            this.prefix = prefix;
        }

        /**
         * Run.
         */
        public void run() {

            BufferedReader br = new BufferedReader(new InputStreamReader(stream));
            try {

                String strLine;
                while ((strLine = br.readLine()) != null)   {

                    if (strLine.contains("Error:") && !strLine.contains("peer shutdown")) {
                        if (stderr) {
                            System.err.println(strLine);
                        }
                    } else {
                        if (stdout) {
                            System.out.println(strLine);
                        }
                    }

                }

            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    /** The config file. */
    private String configFile;
    
    /** The random seed. */
    private int randomSeed = -1;
    
    /** The remote port. */
    private int remotePort;
    
    /** The socket. */
    private Socket socket;

    /** The net file. */
    //new
    private String net_file;
    
    /** The route file. */
    private String route_file;
    
    /** The additional file. */
    private String additional_file;
    
    /** The gui settings. */
    private String gui_settings;

    /** The sumo EXE. */
    String sumoEXE = "/opt/sumo/sumo-1.1.0/bin/sumo";
    
    /** The cp. */
    private CommandProcessor cp;

    /** The sumo process. */
    private Process sumoProcess;
    
    /** The Constant CONNECT_RETRIES. */
    private static final int CONNECT_RETRIES = 3;
    
    /** The close query. */
    @SuppressWarnings("unused")
    private CloseQuery closeQuery;
    
    /** The args. */
    private List<String> args = new ArrayList<String>();

    /** The remote. */
    private boolean remote = false;

    /**
     * Instantiates a new sumo traci connection.
     *
     * @param sumo_bin the sumo bin
     */
    public SumoTraciConnection(String sumo_bin) {
        this.sumoEXE = sumo_bin;
    }

    /**
     * Instantiates a new sumo traci connection.
     *
     * @param sumo_bin the sumo bin
     * @param net_file the net file
     * @param route_file the route file
     */
    public SumoTraciConnection(String sumo_bin, String net_file, String route_file) {
        this.sumoEXE = sumo_bin;
        this.net_file = net_file;
        this.route_file = route_file;
    }

    /**
     * Instantiates a new sumo traci connection.
     *
     * @param sumo_bin the sumo bin
     * @param net_file the net file
     * @param route_file the route file
     * @param additional_file the additional file
     */
    public SumoTraciConnection(String sumo_bin, String net_file, String route_file, String additional_file) {
        this.sumoEXE = sumo_bin;
        this.net_file = net_file;
        this.route_file = route_file;
        this.additional_file = additional_file;
    }

    /**
     * Instantiates a new sumo traci connection.
     *
     * @param sumo_bin the sumo bin
     * @param net_file the net file
     * @param route_file the route file
     * @param additional_file the additional file
     * @param gui_settings the gui settings
     */
    public SumoTraciConnection(String sumo_bin, String net_file, String route_file, String additional_file, String gui_settings) {
        this.sumoEXE = sumo_bin;
        this.net_file = net_file;
        this.route_file = route_file;
        this.additional_file = additional_file;
        this.gui_settings = gui_settings;
    }

    /**
     * Instantiates a new sumo traci connection.
     *
     * @param sumo_bin the sumo bin
     * @param configFile the config file
     */
    public SumoTraciConnection(String sumo_bin, String configFile) {
        this.sumoEXE = sumo_bin;
        this.configFile = configFile;
    }

    /**
     * Instantiates a new sumo traci connection.
     *
     * @param configFile the config file
     * @param randomSeed the random seed
     * @param useGeoOffset the use geo offset
     */
    public SumoTraciConnection(String configFile, int randomSeed, boolean useGeoOffset) {
        this.randomSeed = randomSeed;
        this.configFile = configFile;
    }

    /**
     * Instantiates a new sumo traci connection.
     *
     * @param remotePort the remote port
     * @throws IOException Signals that an I/O exception has occurred.
     * @throws InterruptedException the interrupted exception
     */
    public SumoTraciConnection(int remotePort) throws IOException, InterruptedException {
        this(new InetSocketAddress("127.0.0.1", remotePort));
    }

    /**
     * Instantiates a new sumo traci connection.
     *
     * @param sockAddr the sock addr
     * @throws IOException Signals that an I/O exception has occurred.
     * @throws InterruptedException the interrupted exception
     */
    public SumoTraciConnection(SocketAddress sockAddr) throws IOException,
        InterruptedException {

        this.remote = true;
        socket = new Socket();
        socket.setTcpNoDelay(true);

        int waitTime = 500; // milliseconds
        for (int i = 0; i < CONNECT_RETRIES; i++) {

            try {
                socket.connect(sockAddr);
                break;
            } catch (ConnectException ce) {
                Thread.sleep(waitTime);
                waitTime *= 2;
            }
        }

        if (!socket.isConnected()) {
            throw new IOException("can't connect to SUMO server");
        } else {
            this.cp = new CommandProcessor(socket);
        }

    }

    /**
     * Adds a custom option to the SUMO command line before executing it.
     *
     * @param option
     *            the option name, in long form (e.g. &quot;no-warnings&quot;
     *            instead of &quot;W&quot;) and without initial dashes
     * @param value
     *            the option value, or <code>null</code> if the option has no
     *            value
     */
    public void addOption(String option, String value) {
        args.add("--" + option);
        if (value != null) {
            args.add(value);
        }
    }

    /**
     * Runs a SUMO instance and tries to connect at it.
     *
     * @throws IOException
     *             if something wrong occurs while starting SUMO or connecting
     *             at it.
     */
    public void runServer() throws IOException {
        runServer(0);
    }

    /**
     * Runs a SUMO instance and tries to connect at it.
     *
     * @param _remotePort the remote port
     * @throws IOException             if something wrong occurs while starting SUMO or connecting
     *             at it.
     */
    public void runServer(int _remotePort) throws IOException {



        if (!this.remote) {

            remotePort = _remotePort;
            if (remotePort == 0) {
                findAvailablePort();
            }

            runSUMO();

            int waitTime = 500; // milliseconds
            try {
                for (int i = 0; i < CONNECT_RETRIES; i++) {


                    socket = new Socket();
                    socket.setTcpNoDelay(true);

                    try {
                        socket.connect(new InetSocketAddress("127.0.0.1", remotePort));
                        break;
                    } catch (ConnectException ce) {
                        Thread.sleep(waitTime);
                        waitTime *= 2;
                    }




                }

                if (!socket.isConnected()) {
                    throw new IOException("can't connect to SUMO server at " + remotePort);
                } else {
                    this.cp = new CommandProcessor(socket);
                }

            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            closeQuery = new CloseQuery(socket);

        }

    }

    /**
     * Run SUMO.
     *
     * @throws IOException Signals that an I/O exception has occurred.
     */
    private void runSUMO() throws IOException {

        args.add(0, sumoEXE);

        if (this.configFile != null) {
            args.add("-c");
            args.add(configFile);
        } else if (this.net_file != null && this.route_file != null) {

            args.add("--net-file");
            args.add(this.net_file);
            args.add("--route-files");
            args.add(this.route_file);

            if (this.additional_file != null) {
                args.add("--additional-files");
                args.add(this.additional_file);
            }

            if (this.gui_settings != null) {
                args.add("--gui-settings-file");
                args.add(this.gui_settings);
            }

        } else if (this.net_file != null) {
            args.add("--net-file");
            args.add(this.net_file);
        }

        args.add("--remote-port");
        args.add(Integer.toString(remotePort));

        if (randomSeed != -1) {
            args.add("--seed");
            args.add(Integer.toString(randomSeed));
        }

        String[] argsArray = new String[args.size()];
        args.toArray(argsArray);


        sumoProcess = Runtime.getRuntime().exec(argsArray);



        StreamLogger errStreamLogger = new StreamLogger(sumoProcess.getErrorStream(), "SUMO-err:");
        StreamLogger outStreamLogger = new StreamLogger(sumoProcess.getInputStream(), "SUMO-out:");
        new Thread(errStreamLogger, "StreamLogger-SUMO-err").start();
        new Thread(outStreamLogger, "StreamLogger-SUMO-out").start();
    }

    /**
     * Find available port.
     *
     * @throws IOException Signals that an I/O exception has occurred.
     */
    private void findAvailablePort() throws IOException {
        ServerSocket testSock = new ServerSocket(0);
        remotePort = testSock.getLocalPort();
        testSock.close();
        testSock = null;
    }

    /**
     * Closes the connection, quits the simulator, frees any stale
     * resource and makes all Vehicle instances inactive.
     *
     */

    public void close() {
        try {
            if (isClosed()) {
                throw new IllegalStateException("connection is closed");
            }
            this.cp.do_close();
            socket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * Closes the connection, eating the InterruptedException it may
     * throw, hoping that Murphy's Law doesn't notice all this ugly thing.
     */
    private void closeAndDontCareAboutInterruptedException() {
        close();
    }

    /**
     * Returns <code>true</code> if the connection was closed by the user, or if
     * an {@link IOException} was thrown after the connection was made.
     *
     * @return boolean
     * @see #close()
     */
    public boolean isClosed() {
        return socket == null || socket.isClosed();
    }


    /**
     * Do job set.
     *
     * @param cmd the cmd
     * @throws Exception the exception
     */
    public synchronized void do_job_set(SumoCommand cmd) throws Exception {

        if (isClosed()) {
            throw new IllegalStateException("connection is closed");
        }

        try {
            this.cp.do_job_set(cmd);
        } catch (Exception e) {
            closeAndDontCareAboutInterruptedException();
            throw e;
        }

    }

    /**
     * Do job get.
     *
     * @param cmd the cmd
     * @return the object
     * @throws Exception the exception
     */
    public synchronized Object do_job_get(SumoCommand cmd) throws Exception {

        Object output = null;
        if (isClosed()) {
            throw new IllegalStateException("connection is closed");
        }

        try {
            output = this.cp.do_job_get(cmd);
        } catch (Exception e) {
            closeAndDontCareAboutInterruptedException();
            throw e;
        }

        return output;
    }


    /**
     * Sets the order.
     *
     * @param index the new order
     * @throws Exception the exception
     */
    public synchronized void setOrder(int index) throws Exception {

        if (isClosed()) {
            throw new IllegalStateException("connection is closed");
        }

        try {
            this.cp.do_setOrder(index);
        } catch (Exception e) {
            closeAndDontCareAboutInterruptedException();
            throw e;
        }

    }

    /**
     * Do timestep.
     *
     * @throws Exception the exception
     */
    public synchronized void do_timestep() throws Exception {
        this.do_timestep(0);
    }

    /**
     * Do timestep.
     *
     * @param targetTime the target time
     * @throws Exception the exception
     */
    public synchronized void do_timestep(double targetTime) throws Exception {

        if (isClosed()) {
            throw new IllegalStateException("connection is closed");
        }

        try {
            this.cp.do_SimulationStep(targetTime);
        } catch (Exception e) {
            closeAndDontCareAboutInterruptedException();
            throw e;
        }

    }

    /**
     * Adds the observer.
     *
     * @param o the o
     */
    public synchronized void addObserver(Observer o) {
        this.cp.addObserver(o);
    }

    /**
     * Do subscription.
     *
     * @param cs the cs
     * @throws Exception the exception
     */
    public synchronized void do_subscription(Subscription cs) throws Exception {

        if (isClosed()) {
            throw new IllegalStateException("connection is closed");
        }

        try {
            this.cp.do_subscription(cs);
        } catch (Exception e) {
            closeAndDontCareAboutInterruptedException();
            throw e;
        }

    }

    /**
     * Prints the sumo output.
     *
     * @param b the b
     */
    public void printSumoOutput(boolean b) {
        stdout = b;
    }

    /**
     * Prints the sumo error.
     *
     * @param b the b
     */
    public void printSumoError(boolean b) {
        stderr = b;
    }

}

