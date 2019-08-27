package de.tudresden.sumo.util;

import de.tudresden.sumo.subscription.SubscriptionObject;

public interface Observer {
	
   public void update(Observable ob, SubscriptionObject so);

}