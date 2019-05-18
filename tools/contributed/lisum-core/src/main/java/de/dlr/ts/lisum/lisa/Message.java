/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2016-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Message.java
/// @author  Maximiliano Bottazzi
/// @date    2016
/// @version $Id$
///
//
/****************************************************************************/
package de.dlr.ts.lisum.lisa;


/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
class Message {
    public enum Type {Init, Run}

    private int taskId = 0;
    public long createTime = 0;
    public int intervall = 60;
    public int vdBuildId = 0;
    public long time = 0;



    /**
     *
     * @param taskId
     */
    public Message(int taskId) {
        this.taskId = taskId;
    }

    /**
     *
     * @param type
     * @param wunschVector
     * @return
     */
    public String getCommand(Type type, WunschVector wunschVector, String detectorsString, long simulationTime) {
        createTime = simulationTime;
        time = simulationTime;

        StringBuilder sb = new StringBuilder();
        sb.append(taskId).append(" ");
        sb.append(createTime).append(" ");
        sb.append(intervall).append(" ");
        sb.append(vdBuildId).append(":");
        sb.append(time);
        sb.append("{\"").append(type).append("\"}");
        sb.append("{").append(wunschVector.getVector()).append("}");

        if (type == Type.Init) {
            sb.append("{}{}");
        } else {
            sb.append("{").append(detectorsString).append("}{}");
            sb.append("{}{}");
        }

        return sb.toString();
    }
}
