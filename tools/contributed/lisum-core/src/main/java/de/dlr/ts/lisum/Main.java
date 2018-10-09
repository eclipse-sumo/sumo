/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.dlr.ts.lisum;

import de.dlr.ts.commons.logger.DLRLogger;
import de.dlr.ts.commons.logger.LogLevel;
import de.dlr.ts.commons.tools.FileTools;
import de.dlr.ts.lisum.simulation.LisumSimulation;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author bott_ma
 */
public class Main 
{   
    String sumoExec = "sumo";
    String sumoConfig = null;
    int sumoPort = 9100;
    String lisaRestFulServerDir = "localhost";
    int lisaPort = 9091;    
    String lisumFile = "";
    private String loggingLevel = "INFO";        
    
    /**
     * 
     * @param args
     * @throws Exception 
     */
    public static void main(String[] args) throws Exception 
    {        
        SplashScreen sc = new SplashScreen("LiSuM 1.0.1");
        sc.setYear(2018);
        
        sc.addRuntimeOption("-log            ", "logging level", "INFO");
        sc.addRuntimeOption("-f or -file     ", "lisum_xml Project file", "");
        sc.addRuntimeOption("-c or -conf     ", "config file", "");        
        sc.addRuntimeOption("-s or -sumoexec ", "Sumo executable", "");
        sc.addRuntimeOption("-S or -sumocfg  ", "Sumo configuration", "");
        sc.addRuntimeOption("-l or -lisa     ", "lisa RESTful server address", "localhost");
        sc.showSplashScreen();
        
        /*
        if(args.length == 0) {            
            
            String aa = "-f D:\\Tmp\\Lisum\\Lisum_Victor_20181002\\lisum.xml -c culo.txt";
            //String aa = "-c culo.txt";
            new Main().start(aa.split(" "));
            
            return;
        }
        */
        
        new Main().start(args);
    }    
    

    private String[] clean(String[] args) {
        List<String> tmp = new ArrayList<>();
        
        for (String arg : args)
            if(!arg.trim().isEmpty())
                tmp.add(arg.trim());        
        
        return tmp.toArray(new String[tmp.size()]);
    }
    
    private void readConfFile(String file) {
        
        DLRLogger.info("Reading config file: " + file.trim());
        
        try {
            List<String> f = FileTools.readSmallTextFile(file);
            
            StringBuilder sb = new StringBuilder();
            
            for (String string : f) {
                if(!string.trim().startsWith("#"))                
                    sb.append(string + " ");            
            }
                
            
            readConf(sb.toString().split(" "));
            
        } catch (IOException ex) {
            Logger.getLogger(Main.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    private void readConf(String[] args) 
    {        
        args = clean(args);                
        
        for (int i = 0; i < args.length; i++) 
        {            
            if(args[i].equals("-conf") || args[i].equals("-c")) 
                readConfFile(args[i+1]);            
            
            if(args[i].equals("-sumoexec") || args[i].equals("-s")) {
                sumoExec = args[i+1];
                DLRLogger.info("Setting sumoExec: " + sumoExec);
                
                if(!new File(sumoExec).exists()) {
                    DLRLogger.severe(String.format("Error: Couldn't find Sumo exec file (%s). Quitting.", sumoExec));
                    System.exit(0);
                }
            }                
            if(args[i].equals("-sumocfg") || args[i].equals("-S")) {
                sumoConfig = args[i+1];
                DLRLogger.info("Setting sumoConfig: " + sumoConfig);
                
                if(!new File(sumoConfig).exists()) {
                    DLRLogger.severe(String.format("Error: Couldn't find Sumo coniguration file (%s). Quitting.", sumoConfig));
                    System.exit(0);
                }
            }                
            
            if(args[i].equals("-lisa") || args[i].equals("-l")) {
                lisaRestFulServerDir = args[i+1];
                DLRLogger.info("Setting Lisa Server: " + lisaRestFulServerDir);
                
                if(!new File(lisaRestFulServerDir).exists()) {
                    DLRLogger.severe(String.format("Error: Couldn't find Lisa RESTful directory (%s). Quitting.", lisaRestFulServerDir));
                    System.exit(0);
                }
            }                
            
            if(args[i].equals("-log")) {                
                loggingLevel = args[i+1];
                DLRLogger.info("Setting logging level: " + loggingLevel.toUpperCase());
            }                
            
            if(args[i].equals("-file") || args[i].equals("-f")) {
                lisumFile = args[i+1];
                DLRLogger.info("Project file: " + lisumFile);
                
                if(!new File(lisumFile).exists()) {
                    DLRLogger.severe(String.format("Error: Couldn't find projec file (%s). Quitting.", lisumFile));
                    System.exit(0);
                }
            }                                        
        } 
    }
    
    /**
     * 
     * @param args
     * @throws Exception 
     */
    public void start(String[] args) throws Exception 
    {
        readConf(args);        
        
        DLRLogger.setLevel(LogLevel.valueOf(loggingLevel.toUpperCase()));
        
        if(lisumFile.isEmpty()) {
            System.err.println("Error. No Lisum file received. Use -f <path_to_file/lisum.xml>. Quitting.");
            return;
        }
        
        LisumSimulation ls = new LisumSimulation(sumoExec, sumoConfig, sumoPort, lisaRestFulServerDir, lisaPort);        
        ls.load(new File(lisumFile));
     
        ls.initBeforePlay();
        
        new Thread(ls.getRunnable()).start();
    }
}
