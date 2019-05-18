/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package de.dlr.ts.commons.utils.print;

/**
 *
 * @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public enum Color {
    NONE("", ""),
    BLANK("30", "40"),
    RED("31", "41"),
    GREEN("32", "42"),
    YELLOW("33", "43"),
    BLUE("34", "44"),
    MAGENTA("35", "45"),
    CYAN("36", "46"),
    WHITE("37", "47"),
    ORANGE("38;5;214", "48;5;208"),
    REDYELLOW("38;5;214", "48;5;208"),
    OFF("37", "47");

    private final String fore;
    private final String back;

    private Color(String fore, String back) {
        this.fore = fore;
        this.back = back;
    }

    public String getForegroundCode() {
        return fore;
    }

    public String getBackgroundCode() {
        return back;
    }
}
