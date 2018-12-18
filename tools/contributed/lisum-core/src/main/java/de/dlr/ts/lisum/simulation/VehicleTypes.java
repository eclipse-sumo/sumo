/** ************************************************************************* */
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2016-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/** ************************************************************************* */
/// @file    Constants.java
/// @author  Maximiliano Bottazzi
/// @date    2016
/// @version $Id$
///
//
/** ************************************************************************* */
package de.dlr.ts.lisum.simulation;

import de.dlr.ts.commons.logger.DLRLogger;
import de.dlr.ts.utils.xmladmin2.Attributes;
import de.dlr.ts.utils.xmladmin2.XMLAdmin2;
import de.dlr.ts.utils.xmladmin2.XMLNode;
import de.dlr.ts.utils.xmladmin2.MalformedKeyOrNameException;
import de.dlr.ts.utils.xmladmin2.XMLNodeNotFoundException;
import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.xml.sax.SAXException;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano
 * Bottazzi</a>
 */
public class VehicleTypes {

    private final Map<String, VehicleType> types = new HashMap<>();

    /**
     *
     */
    public VehicleTypes() {
    }

    /**
     *
     * @param name
     * @return
     */
    public VehicleType getVehicleType(String name) {
        return types.get(name);
    }

    /**
     *
     * @param configFile
     */
    public void load(File configFile) {
        try
        {
            DLRLogger.config(this, "Loading file " + configFile);
            
            XMLAdmin2 x = new XMLAdmin2().load(configFile);
            
            int nodesCount;
            try {
                nodesCount = x.getNodesCount("vType");
            } catch (XMLNodeNotFoundException ex) {
                DLRLogger.severe(this, "Fatal error: Node <vType> in " + configFile.getName() + " not found.");
                return;
            }
            
            for (int i = 0; i < nodesCount; i++)
            {
                XMLNode node = x.getNode("vType", i);
                
                Attributes att = node.getAttributes();
                if(att == null) {
                    DLRLogger.severe(this, "Fatal error: Invalid attributes in node <vType> in " + 
                            configFile.getName() + " not found, 'id' and 'length' expected.");
                    return;
                }
                
                String id = att.get("id").getValue();
                double length = att.get("length").getValue(0d);
                
                types.put(id, new VehicleType(id, length));
            }
        }
        catch (SAXException ex)
        {
            ex.printStackTrace(System.out);
        } catch (IOException ex) {
            DLRLogger.config(this, "Fatal error: File " + configFile.getName() + " not found.");
        } catch (MalformedKeyOrNameException ex) {
            Logger.getLogger(VehicleTypes.class.getName()).log(Level.SEVERE, null, ex);
        } catch (XMLNodeNotFoundException ex) {
            Logger.getLogger(VehicleTypes.class.getName()).log(Level.SEVERE, null, ex);
        } 
    }

    /**
     *
     */
    public static class VehicleType {

        private final String name;
        private final double length;

        /**
         *
         * @param name
         * @param length
         */
        public VehicleType(String name, double length) {
            this.name = name;
            this.length = length;
        }

        public String getName() {
            return name;
        }

        /**
         *
         * @return
         */
        public double getLength() {
            return length;
        }
    }
}
