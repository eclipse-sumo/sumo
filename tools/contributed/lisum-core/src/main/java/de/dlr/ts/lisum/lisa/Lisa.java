/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2016-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Lisa.java
/// @author  Maximiliano Bottazzi
/// @date    2016
/// @version $Id$
///
//
/****************************************************************************/
package de.dlr.ts.lisum.lisa;

import de.dlr.ts.commons.logger.DLRLogger;
import de.dlr.ts.lisum.exceptions.LisaRESTfulServerNotFoundException;
import de.dlr.ts.lisum.interfaces.CityInterface;
import de.dlr.ts.lisum.interfaces.ControlUnitInterface;
import de.dlr.ts.lisum.simulation.LisumSimulation;
import java.io.File;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano
 * Bottazzi</a>
 */
public class Lisa implements CityInterface {

    private final LisaCommands lisaCommands;
    private final LisaControlUnits lisaControlUnits = new LisaControlUnits();
    private File lisaDirectory;

    /**
     *
     * @param simulation
     */
    private Lisa(String lisaServerAddress, int lisaServerPort) {
        lisaCommands = new LisaCommands(lisaServerAddress, lisaServerPort);
    }

    @Override
    public ControlUnitInterface[] getControlUnits() {
        return lisaControlUnits.getControlUnits();
    }

    /**
     *
     * @param lisaServerAddress
     * @param lisaServerPort
     * @return
     */
    public static CityInterface create(String lisaServerAddress, int lisaServerPort) {
        return new Lisa(lisaServerAddress, lisaServerPort);
    }

    /**
     * Looks for xml files in the Lisa directory. Each of those files represent
     * Control units and contain information related to them. This method loads
     * this information and communicates the Lisa server the data directory.
     *
     * @param lisaDirectory
     */
    @Override
    public void load(File lisaDirectory) {
        DLRLogger.config(this, "Loading directory " + lisaDirectory);

        if (!lisaDirectory.exists()) {
            DLRLogger.severe(this, lisaDirectory + " could not be found");
            return;
        }

        this.lisaDirectory = lisaDirectory;

        LisaConfigurationFiles configurationFiles = new LisaConfigurationFiles();
        configurationFiles.load(lisaDirectory);

        lisaControlUnits.load(configurationFiles, lisaCommands);
    }

    /**
     *
     * @return
     */
    @Override
    public LisumSimulation.InitBeforePlayResponse initBeforePlay() {

        if (!LisaCommands.TestRESTfulServer()) {
            return LisumSimulation.InitBeforePlayResponse.LisaRESTfulServerNotFound;
        }

        try {
            lisaCommands.setDataDir(lisaDirectory);
            lisaControlUnits.initBeforePlay();
        } catch (LisaRESTfulServerNotFoundException ex) {
            Logger.getLogger(Lisa.class.getName()).log(Level.SEVERE, null, ex);
        }

        return LisumSimulation.InitBeforePlayResponse.OK;
    }

    /**
     *
     * @param simulationTime
     */
    @Override
    public void executeSimulationStep(long simulationTime) {

        try {
            lisaControlUnits.executeSimulationStep(simulationTime);
        } catch (LisaRESTfulServerNotFoundException ex) {
            ex.printStackTrace(System.out);
        }

    }

    /**
     *
     * @param name
     * @return
     */
    @Override
    public ControlUnitInterface getControlUnit(String name) {
        return this.lisaControlUnits.getControlUnit(name);
    }

}
