package de.tudresden.sumo.util;

import java.util.ArrayList;
import java.util.List;

import de.tudresden.sumo.subscription.SubscriptionObject;

public class Observable {
	
   private List<Observer> observers = new ArrayList<Observer>();

   public void addObserver(Observer observer){
      observers.add(observer);		
   }

   public void notifyObservers(SubscriptionObject so){
	   
	  System.out.println("here!!!!");
      for (Observer observer : observers) {observer.update(this, so);}
   } 	

}