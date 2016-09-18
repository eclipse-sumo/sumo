package de.tudresden.ws.container;

public class SumoPrimitive implements SumoObject {

	public Object val;
	public String type;
	
	public SumoPrimitive(Object o){
		
		this.val = o;
		if(o.getClass().equals(Integer.class)){
			type = "integer";
		}else if(o.getClass().equals(Double.class)){
			type = "double";
		}else if(o.getClass().equals(String.class)){
			type = "string";
		}else{
			System.err.println("unsupported format " + o.getClass());
		}
		
	}
	
}
