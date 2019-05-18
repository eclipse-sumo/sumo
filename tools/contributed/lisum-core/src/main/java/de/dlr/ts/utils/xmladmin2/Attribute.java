/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package de.dlr.ts.utils.xmladmin2;

import java.util.Objects;

/**
 *
 * @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class Attribute extends Value implements Comparable<Attribute> {
    private String name = "";



    /**
     *
     */
    public Attribute() {
    }

    /**
     *
     * @param name
     */
    public void setName(String name) {
        this.name = name;
    }

    @Override
    public String toString() {
        return name + "=" + value;
    }

    /**
     *
     * @return
     */
    public String getName() {
        return name;
    }

    /**
     * Attribute
     *
     * @param name
     * @param value
     */
    public Attribute(String name, String value) {
        this.name = name;
        this.value = value;
    }

    /**
     *
     * @param o
     * @return
     */
    @Override
    public int compareTo(Attribute o) {
        if (this.name.equals(o.name) && this.value.equals(o.value)) {
            return 0;
        }

        return 1;
    }

    @Override
    public boolean equals(Object obj) {
        if (obj == this) {
            return true;
        }
        if (obj == null || obj.getClass() != this.getClass()) {
            return false;
        }

        Attribute guest = (Attribute) obj;

        return this.hashCode() == guest.hashCode();
    }

    @Override
    public int hashCode() {
        int hash = 7;
        hash = 97 * hash + Objects.hashCode(this.name);
        return hash;
    }
}
