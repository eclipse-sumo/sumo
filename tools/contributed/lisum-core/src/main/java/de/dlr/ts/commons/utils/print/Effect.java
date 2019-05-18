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
public enum Effect {
    NONE(0), BOLD(1), ITALICS(3), UNDERLINE(4);

    //underline = 4

    int code;

    private Effect(int code) {
        this.code = code;
    }

    public int getCode() {
        return code;
    }


}
