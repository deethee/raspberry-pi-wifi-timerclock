package heatingbackend;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.text.SimpleDateFormat;
import java.util.Date;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Component;

//TODO to activate raspberry heating uncomment//
@Component
public class Timer {

    private static final Logger log = LoggerFactory.getLogger(Timer.class);

    private static final SimpleDateFormat dateFormat = new SimpleDateFormat("HH:mm:ss");

  //TODO to activate raspberry heating uncomment//
  @Scheduled(fixedRate = 60000)
    public void reportCurrentTime() {        
        
    	log.info("The time is now {}", dateFormat.format(new Date()));

        Integer switchNumber = Api.witchSwitchIsNow();
    	
        if(HeatingbackendApplication.overrideUntilSwitch == null || HeatingbackendApplication.overrideUntilSwitch < switchNumber 
        		|| HeatingbackendApplication.overrideDay == null || HeatingbackendApplication.overrideDay.getTime() != new Date(new Date().getYear(), new Date().getMonth(),  new Date().getDay()).getTime() ) {
        
	        if (Api.isHeatingRegularyScheduledOnNow()) {
	        	System.out.println("Heating shall be on");
	        	if (HeatingbackendApplication.isHeatingOn) {
	        		System.out.println("Heating is already on");
	        	}else {
	        		System.out.println("-------------- Switching Heating on -------------");
	        		Api.switchHeating(true);
	        		
	        	}
	        }else {
	        	System.out.println("Heating shall be off");
	        	if (HeatingbackendApplication.isHeatingOn) {
	        		System.out.println(" -------------- Switching Heating off -----------------");
	        		Api.switchHeating(false);
	        		
	        		
	        	}else {
	        		System.out.println("Heating is already off");
	        	}
	        }
	        
        }else{
        	System.out.println("Heating is overridden to "+ HeatingbackendApplication.overrideState);
        }
        

    }
    
    


    
}