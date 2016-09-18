package de.tudresden.sumo.subscription;

import de.tudresden.ws.container.SumoObject;

public class SubscriptionObject {

	public String id;
	public ResponseType response;
	public int domain;
	public String name;
	public int variable;
	public int status;
	public int return_type;
	public SumoObject object;
	
	
		//context
		public SubscriptionObject(String id, ResponseType response, int domain, String name, int variable, int status, int return_type, SumoObject object){
			this.id = id;
			this.response = response;
			this.domain = domain;
			this.name = name;
			this.variable = variable;
			this.status = status;
			this.return_type = return_type;
			this.object = object;
		}
	
		//variable
		public SubscriptionObject(String id, ResponseType response, int variable, int status, int return_type, SumoObject object){
			this.id = id;
			this.response = response;
			this.variable = variable;
			this.status = status;
			this.return_type = return_type;
			this.object = object;
		}
	
}
