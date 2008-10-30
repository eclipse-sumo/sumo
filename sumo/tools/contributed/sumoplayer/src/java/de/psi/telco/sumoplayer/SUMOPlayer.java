/**
 * This software is under the 
 * 
 * GNU GENERAL PUBLIC LICENSE
 *    Version 2, June 1991
 * 
 * see attached file LICENSE
 */
package de.psi.telco.sumoplayer;

import java.io.File;
import java.io.IOException;

import javax.xml.parsers.ParserConfigurationException;
import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;

import org.xml.sax.SAXException;

/**
 * This class runs a SUMO simulated netword dumpfile.
 * It needs to know of the used xumo.net.xml network file
 * in order to look up the coordinates correctly. 
 *  
 * @author Michael Willigens <michael@willigens.de>
 *
 */
public class SUMOPlayer extends Thread{

	private String ndumpFile = "sumo.ndump.xml";
	private String networkFile = "sumo.net.xml";
	private double trackingPropability = 0.01;
	private int jitter = 0;	// in meters
	private String listenerClassname = "de.psi.telco.sumoplayer.SUMOPipedLocationListener";
	
	private SAXParser saxParser;
	private SUMOSAXHandler saxHandler;
	
	/**
	 * the main function to call it from command line
	 * @param args
	 */
	public static void main(String[] args) {
		String nDumpFilename = "sumo.ndump.xml";
		String netFilename = "sumo.net.xml";
		String listenerClassname = "de.psi.telco.sumoplayer.SUMOPipedLocationListener";
		double propability = 0.01;
		int jitter = 0;
		
    	for (int i = 0; i<args.length;i++){
    		if (args[i].equals("-n") && args.length > i+1){
    			netFilename = args[i+1];
    			i++;
    		}
    		if (args[i].equals("-d") && args.length > i+1){
    			nDumpFilename = args[i+1];
    			i++;
    		}
    		if (args[i].equals("-l") && args.length > i+1){
    			listenerClassname = args[i+1];
    			i++;
    		}
    		if (args[i].equals("-e") && args.length > i+1){
    			propability = Double.parseDouble(args[i+1]);
    			i++;
    		}
    		if (args[i].equals("-j") && args.length > i+1){
    			jitter = Integer.parseInt(args[i+1]);
    			i++;
    		}
    	}
		
    	File net = new File(netFilename);
    	if (!net.exists()){
    		System.out.println("SUMO Network file does not exist! set this with \"-n sumo.net.xml\". aborting");
    		System.exit(1);
    	}
    	File ndump = new File(nDumpFilename);
    	if (!ndump.exists()){
    		System.out.println("SUMO ndump file does not exist! set this with \"-d sumo.ndump.xml\". aborting");
    		System.exit(1);
    	}
    	
		SUMOPlayer player = new SUMOPlayer(nDumpFilename,netFilename,propability, jitter, listenerClassname);
		
		player.run();
	}
	
	/**
	 * This function initializes a SUMO player.
	 * It mainly reads the network file which it
	 * will need to perform a simualtion.
	 * 
	 */
	private void init() {
        SAXParserFactory saxFactory = SAXParserFactory.newInstance();
        try {
			saxParser = saxFactory.newSAXParser();
		} catch (ParserConfigurationException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (SAXException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		saxHandler = new SUMOSAXHandler(networkFile);
		saxHandler.setCar4carEquippedPropability(this.trackingPropability);
		saxHandler.setJitter(jitter);
		
		SUMOLocationListener listener = null;
		try {
			listener = (SUMOLocationListener) Class.forName(listenerClassname).newInstance();
		} catch (Exception e){
			e.printStackTrace();
			System.exit(1);
		}
		saxHandler.setListener(listener);
	}
	
	/**
	 * This function Runs the simulation.
	 *
	 */
	public void run() {
		this.init();	// read network first
		
		System.out.println("Performing Simulation...");
        try {
			saxParser.parse(new File(ndumpFile), saxHandler);
		} catch (SAXException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
        System.out.println("done.");
	}

	/**
	 * This constructor creates a SUMO player using a ndump file.
	 * @param ndumpFile
	 * @param networkFile
	 * @param trackingPropability
	 */
	public SUMOPlayer(String ndumpFile, String networkFile, double trackingPropability, int jitter, String listenerClassname){
		
		this.ndumpFile = ndumpFile;
		this.networkFile = networkFile;
		if (trackingPropability > 0) this.trackingPropability = trackingPropability;
		this.jitter = jitter;
		
		if (listenerClassname != null && listenerClassname.length() > 0){
			this.listenerClassname = listenerClassname;
		}
	}

}
