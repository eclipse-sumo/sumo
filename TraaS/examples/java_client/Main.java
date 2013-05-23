import de.tudresden.ws.ServiceImpl;
import de.tudresden.ws.SumoWebservice;


public class Main {


	public static void main(String[] args) {

		ServiceImpl ws =  new SumoWebservice().getServiceImplPort();
		ws.addOption("start", "");
		ws.addOption("step-length", "1");
				
		ws.start("user");
		
		for(int i=0; i<3600; i++){
			ws.doTimestep(1);
			
			if(i%10 == 0){
				ws.vehicleAdd("veh_"+i, "car", "r1", 0, 0, 13.8, (byte) 0);
			}
			
		}
		
		
	}

}
