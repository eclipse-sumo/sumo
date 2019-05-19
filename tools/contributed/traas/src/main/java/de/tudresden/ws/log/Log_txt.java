/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2019 German Aerospace Center (DLR) and others.
// TraaS module
// Copyright (C) 2016-2017 Dresden University of Technology
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Log_txt.java
/// @author  Mario Krumnow
/// @author  Evamarie Wiessner
/// @date    2016
/// @version $Id$
///
//
/****************************************************************************/
package de.tudresden.ws.log;

import java.io.FileWriter;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

public class Log_txt implements Log {

    private boolean txt_output = false;

    @Override
    public void write(String input, int priority) {


        if (priority == 1) {
            System.out.println(input);
        }

        if (txt_output) {

            try {
                FileWriter fw = new FileWriter("output.txt", true);
                fw.write(get_message(input));
                fw.flush();
                fw.close();
            } catch (Exception e) {
                System.err.println(e);
            }

        }


    }

    public void write(StackTraceElement[] el) {

        for (int i = el.length - 1; i >= 0; i--) {
            System.err.println(el[i].toString());
        }

    }


    public void txt_output(boolean txt_output) {
        this.txt_output = txt_output;
    }

    private String get_message(String input) {

        DateFormat df = new SimpleDateFormat("hh:mm:ss dd.MM.yyyy");
        return df.format(new Date()) + " - " + input + "\n";

    }

}
