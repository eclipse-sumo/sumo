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
/// @file    Observable.java
/// @author  Mario Krumnow
/// @date    2019
/// @version $Id$
///
//
/****************************************************************************/
package de.tudresden.sumo.util;

import java.util.ArrayList;
import java.util.List;

import de.tudresden.sumo.subscription.SubscriptionObject;

public class Observable {

    private List<Observer> observers = new ArrayList<Observer>();

    public void addObserver(Observer observer) {
        observers.add(observer);
    }

    public void notifyObservers(SubscriptionObject so) {
        for (Observer observer : observers) {
            observer.update(this, so);
        }
    }

}
