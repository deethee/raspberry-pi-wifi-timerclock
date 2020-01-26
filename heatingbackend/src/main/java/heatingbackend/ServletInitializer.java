package heatingbackend;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.boot.builder.SpringApplicationBuilder;
import org.springframework.boot.web.servlet.support.SpringBootServletInitializer;

import com.pi4j.io.gpio.GpioController;
import com.pi4j.io.gpio.GpioFactory;
import com.pi4j.io.gpio.GpioPinDigitalInput;
import com.pi4j.io.gpio.PinPullResistance;
import com.pi4j.io.gpio.RaspiPin;
import com.pi4j.io.gpio.event.GpioPinDigitalStateChangeEvent;
import com.pi4j.io.gpio.event.GpioPinListenerDigital;

public class ServletInitializer extends SpringBootServletInitializer {

	@Override
	protected SpringApplicationBuilder configure(SpringApplicationBuilder application) {
		
		
		System.out.println("--------------------ServletInitializer started --------------------------");
		System.out.println("--------------------ServletInitializer started --------------------------");
		System.out.println("--------------------ServletInitializer started --------------------------");
		
		
		Api.configFilesToSwitches();

		// Read digital only values with pi4k
		GpioController gpio = GpioFactory.getInstance();	 
		// provision gpio pin #02 as an input pin with its internal pull down resistor enabled
		GpioPinDigitalInput radarInputPin= gpio.provisionDigitalInputPin(RaspiPin.GPIO_02);



	    // set shutdown state for this input pin
	    radarInputPin.setShutdownOptions(true);
	    	
	        radarInputPin.addListener(new GpioPinListenerDigital() {
	            @Override
	            public void handleGpioPinDigitalStateChangeEvent(GpioPinDigitalStateChangeEvent event) {
	                // display pin state on console
	                System.out.println(" --> GPIO PIN STATE CHANGE: " + event.getPin() + " = " + event.getState());
	            }
	      
	        });
	    System.out.println(" ... complete the GPIO #02 circuit and see the listener feedback here in the console.");
	    // END: Read digital only values with pi4k	    
	        
	        
		return application.sources(HeatingbackendApplication.class);
	}

}
