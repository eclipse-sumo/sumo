/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2017-2024 German Aerospace Center (DLR) and others.
// TraaS module
// Copyright (C) 2016-2017 Dresden University of Technology
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
/// @file    SumoLinkList.java
/// @author  Mario Krumnow
/// @author  Evamarie Wiessner
/// @date    2016
///
//
/****************************************************************************/
package de.tudresden.sumo.objects;

import java.io.Serializable;
import java.util.Collection;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.ListIterator;

/**
 *
 * @author Mario Krumnow
 *
 */

public class SumoLinkList implements List<SumoLink>, Serializable, SumoObject {

    private static final long serialVersionUID = -6530046166179152137L;

    private final List<SumoLink> list;

    public SumoLinkList() {
        list = new LinkedList<SumoLink>();
    }

    public SumoLinkList(List<SumoLink> list) {
        this.list = list;
    }

    public void add(int index, SumoLink element) {
        list.add(index, element);
    }

    public boolean addAll(Collection<? extends SumoLink> elements) {
        return list.addAll(elements);
    }


    public boolean addAll(int index, Collection<? extends SumoLink> elements) {
        return list.addAll(index, elements);
    }


    public void clear() {
        list.clear();
    }

    public boolean contains(Object element) {
        return list.contains(element);
    }


    public boolean containsAll(Collection<?> elements) {
        return list.containsAll(elements);
    }


    public SumoLink get(int index) {
        return list.get(index);
    }


    public int indexOf(Object element) {
        return list.indexOf(element);
    }

    public boolean isEmpty() {
        return list.isEmpty();
    }

    public int lastIndexOf(Object element) {
        return list.lastIndexOf(element);
    }

    public ListIterator<SumoLink> listIterator() {
        return list.listIterator();
    }

    public ListIterator<SumoLink> listIterator(int index) {
        return list.listIterator(index);
    }

    public boolean remove(Object element) {
        return list.remove(element);
    }

    public SumoLink remove(int index) {
        return list.remove(index);
    }

    public boolean removeAll(Collection<?> elements) {
        return list.removeAll(elements);
    }

    public boolean retainAll(Collection<?> elements) {
        return list.retainAll(elements);
    }

    public SumoLink set(int index, SumoLink element) {
        return list.set(index, element);
    }


    public int size() {
        return list.size();
    }


    public List<SumoLink> subList(int from, int to) {
        return list.subList(from, to);
    }


    public Object[] toArray() {
        return list.toArray();
    }


    public <T> T[] toArray(T[] element) {
        return list.toArray(element);
    }


    public boolean add(SumoLink element) {
        return list.add(element);
    }


    public Iterator<SumoLink> iterator() {
        return list.iterator();
    }

    public String toString() {
        StringBuilder sb = new StringBuilder();
        for (SumoLink sl : this.list) {
            sb.append(sl.toString() + "#");
        }

        return sb.toString();
    }

}
