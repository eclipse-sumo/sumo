/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2016-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Attributes.java
/// @author  Maximiliano Bottazzi
/// @date    2016
/// @version $Id$
///
//
/****************************************************************************/
package de.dlr.ts.utils.xmladmin2;

import java.util.ArrayList;
import java.util.List;

/**
 *
 * @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class Attributes implements Comparable<Attributes> {
    private List<Attribute> attributes = new ArrayList<>();



    /**
     *
     * @param name
     * @param value
     * @return
     */
    public boolean hasAttribute(String name, String value) {
        return hasAttribute(new Attribute(name, value));
    }

    /**
     *
     * @return
     */
    public String getXML() {
        StringBuilder sb = new StringBuilder(" ");

        for (Attribute ka : attributes) {
            sb.append(ka.getName());
            sb.append("=\"");
            sb.append(ka.getValue());
            sb.append("\" ");
        }

        return sb.toString();
    }

    @Override
    public String toString() {
        String tmp = "";

        for (Attribute ka : attributes) {
            tmp = tmp + " " + ka;
        }

        return tmp.trim();
    }

    /**
     *
     * @param name
     * @param value
     */
    public void add(String name, String value) {
        attributes.add(new Attribute(name, value));
    }

    /**
     *
     * @param ka
     */
    public void add(Attribute ka) {
        attributes.add(ka);
    }

    /**
     *
     * @return
     */
    public boolean isEmpty() {
        return attributes.isEmpty();
    }

    /**
     *
     * @return
     */
    public int size() {
        return attributes.size();
    }

    /**
     *
     * @param index
     * @return
     */
    public Attribute get(int index) {
        return attributes.get(index);
    }

    /**
     *
     * @param attributeName
     * @return
     */
    public Attribute get(String attributeName) {
        for (Attribute att : attributes)
            if (att.getName().equals(attributeName)) {
                return att;
            }

        return null;
    }

    @Override
    public int compareTo(Attributes o) {
        if (o.attributes.size() != attributes.size()) {
            return 1;
        }

        for (Attribute ka : o.attributes)
            if (!this.hasAttribute(ka)) {
                return 2;
            }

        return 0;
    }

    /**
     *
     * @param ka
     * @return
     */
    public boolean hasAttribute(Attribute ka) {
        for (Attribute o : attributes)
            if (ka.compareTo(o) == 0) {
                return true;
            }

        return false;
    }

    /**
     *
     * @param attributeName
     * @return
     */
    public boolean hasAttribute(String attributeName) {
        for (Attribute get : attributes)
            if (get.getName().equals(attributeName)) {
                return true;
            }

        return false;
    }


    /**
     * Use isEmpty() instead
     * @return
     */
    @Deprecated
    public boolean hasAttributes() {
        return !attributes.isEmpty();
    }
}
