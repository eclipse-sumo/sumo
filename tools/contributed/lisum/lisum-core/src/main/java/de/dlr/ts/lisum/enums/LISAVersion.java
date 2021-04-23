/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
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
/// @file    LISAVersion.java
/// @author  Philippe Käufer
/// @date    2021
///
//
/****************************************************************************/
package de.dlr.ts.lisum.enums;

/**
 *
 * @author @author <a href="mailto:sumo@philhil.de">Philippe Käufer</a>
 */
public enum LISAVersion {
    gt0(0, "0"),
    gt7_2(72, "7.2");

    private final int versionCode;
    private final String versionString;

    /**
     *
     * @param versionCode
     * @param versionString
     */
    private LISAVersion(int versionCode, String versionString) {
        this.versionCode = versionCode;
        this.versionString = versionString;
    }

    /**
     *
     * @return
     */
    public int getVersionCode() {
        return versionCode;
    }


    /**
     *
     * @return
     */
    public String getVersionString() { return versionString; }

    /**
     *
     * @return
     */
    public static LISAVersion getDefault() { return values()[values().length-1]; }

    /**
     *
     * @return
     */
    public static String getVersionsList() {
        String versions = "";
        for (LISAVersion lv : values())
            versions += lv.getVersionString() + ";";

        return versions;
    }

    /**
     *
     * @param code
     * @return
     */
    public static LISAVersion getPerVersionCode(int code) {
        for (LISAVersion lv : values())
            if (lv.versionCode == code) {
                return lv;
            }

        return null;
    }

    /**
     *
     * @param code
     * @return
     */
    public static LISAVersion getPerVersionString(String code) {
        for (LISAVersion lv : values())
            if (lv.versionString == code) {
                return lv;
            }

        return null;
    }
}
