
package heatingbackend;



//import org.springframework.beans.factory.annotation.Autowired;
//import org.springframework.messaging.handler.annotation.MessageMapping;
//import org.springframework.messaging.handler.annotation.SendTo;
//import org.springframework.messaging.simp.SimpMessagingTemplate;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;

import model.MeasuredValue;
import model.Message;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.Iterator;
import java.util.List;
import java.util.Map.Entry;
import java.util.TreeSet;

import org.springframework.web.bind.annotation.RequestMapping;

@org.springframework.web.bind.annotation.RestController
public class Api {

/*	@MessageMapping("/chat")
	@SendTo("/topic/messages")
	public Message send(Message message) throws Exception {

	    String time = new SimpleDateFormat("HH:mm").format(new Date());
	    Message messageOut = new Message();
	    messageOut.setMessage("this is a test message");
	    return messageOut;
	}
	*/
	
	
	 @RequestMapping("/api/gettimerconfig")
	 public String getConfigFile( String timeRangeType) {
              return Api.readConfigFile(timeRangeType);
    }
	 
	 
	 @RequestMapping("/api/getinfo")
	 public String getConfigFile( ) {		 
		 	
			String t = "\"\"";
		    String h = "\"\"";
		    
			if (HeatingbackendApplication.averageHumidityLastHour != null
					&& 
					HeatingbackendApplication.sensorValues.get(HeatingbackendApplication.sensorValues.size()-1).getDate().getTime()
					>= ( new Date(new Date().getYear(), new Date().getMonth(), new Date().getDate(), new Date().getHours()-1, new Date().getMinutes()).getTime())					
					
					){
				
		 		t = "\""+Math.round(HeatingbackendApplication.averageTemperatureLastHour * 100)/100 +"°C"+"\"";		 	

		  		h = "\""+Math.round(HeatingbackendApplication.averageHumidityLastHour * 100)/100 +"%"+"\"";
		 	}
		 	
			String i = "\"\"";
		 	if (HeatingbackendApplication.info != null ){
		 		i = "\""+HeatingbackendApplication.info+"\"";
		 	}
		 	
			String s = "\"\"";
		 	if (HeatingbackendApplication.info != null ){
		 		s = ""+HeatingbackendApplication.overrideUntilSwitch;
		 	}

		 System.out.println(HeatingbackendApplication.sensorValues.toString());
            return "{\"temperature\":" + t +",\"humidity\":" + h + ",\"info\":" + i+ ",\"overrideUntilSwitch\":" + s+ ",\"sensorValues\":" + HeatingbackendApplication.sensorValues.toString() + "}";
    }
	 
	
	@RequestMapping(value="/api/toggleswitch", method = RequestMethod.POST)
	public @ResponseBody String  toggleSwitch(@RequestBody String body) {
	
		Integer switchindex;
		String timeRangeType;
		ObjectMapper mapper = new ObjectMapper();
		try {
			JsonNode node = mapper.readTree(body);
			 switchindex = node.get("switchIndex").asInt();
			 timeRangeType = node.get("timeRangeType").asText();
			
			System.out.println(timeRangeType);
			
			if (timeRangeType.equals("weekdays")) {
				if (HeatingbackendApplication.switches.contains(switchindex)) {
					HeatingbackendApplication.switches.remove(switchindex);
				} else {
					HeatingbackendApplication.switches.add(switchindex);
				}
			} else {
				if (HeatingbackendApplication.switchesWeekend.contains(switchindex)) {
					HeatingbackendApplication.switchesWeekend.remove(switchindex);
				} else {
					HeatingbackendApplication.switchesWeekend.add(switchindex);
				}				
			}

			return Api.saveSwitchesToConfigFile(timeRangeType);
		
		
		} catch (IOException e) {
			// TODO Auto-generated catch block
			return e.getMessage();
		}
	}

	@RequestMapping("/api/togglehour")
	public @ResponseBody String toggleHour(@RequestBody String body) {		
		
		Integer hour;
		String timeRangeType;
		ObjectMapper mapper = new ObjectMapper();
		
		try {
			JsonNode node = mapper.readTree(body);
			hour = node.get("hour").asInt();
			 timeRangeType = node.get("timeRangeType").asText();
		
			if (timeRangeType.equals("weekdays")) {
				if (HeatingbackendApplication.switches.contains(hour*4)) {
					HeatingbackendApplication.switches.remove(new Integer(hour*4));
					HeatingbackendApplication.switches.remove(new Integer(hour*4+1));
					HeatingbackendApplication.switches.remove(new Integer(hour*4+2));
					HeatingbackendApplication.switches.remove(new Integer(hour*4+3));				
					
				}else {
					HeatingbackendApplication.switches.add(hour*4);
					HeatingbackendApplication.switches.add(hour*4+1);
					HeatingbackendApplication.switches.add(hour*4+2);
					HeatingbackendApplication.switches.add(hour*4+3);				
				}
			}else {
				if (HeatingbackendApplication.switchesWeekend.contains(hour*4)) {
					HeatingbackendApplication.switchesWeekend.remove(new Integer(hour*4));
					HeatingbackendApplication.switchesWeekend.remove(new Integer(hour*4+1));
					HeatingbackendApplication.switchesWeekend.remove(new Integer(hour*4+2));
					HeatingbackendApplication.switchesWeekend.remove(new Integer(hour*4+3));				
					
				}else {
					HeatingbackendApplication.switchesWeekend.add(hour*4);
					HeatingbackendApplication.switchesWeekend.add(hour*4+1);
					HeatingbackendApplication.switchesWeekend.add(hour*4+2);
					HeatingbackendApplication.switchesWeekend.add(hour*4+3);				
				}
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			return e.getMessage();
		}
		

		return Api.saveSwitchesToConfigFile(timeRangeType);
	}

	
	@RequestMapping("/api/override")
	public String heatingOverride(@RequestBody long milliseconds) {		

			HeatingbackendApplication.overrideState = true;
			HeatingbackendApplication.overrideUntilSwitch = Api.getSwitchfromDate(new Date(milliseconds));	
			HeatingbackendApplication.overrideDay= new Date(new Date().getYear(), new Date().getMonth(), new Date().getDay());
			
			Api.switchHeating(HeatingbackendApplication.overrideState);
		
			HeatingbackendApplication.info="Heizung an bis "+new Date(milliseconds);
		return HeatingbackendApplication.info;
	}
	
	
	@RequestMapping("/api/switchheating")
	public String heatingOverride(@RequestBody String isTurnedOnString) {		
			
		if (Boolean.parseBoolean(isTurnedOnString) == isHeatingRegularyScheduledOnNow()){
			System.out.println("Warning heating is already " + isTurnedOnString );
			//HeatingbackendApplication.overrideState = null;
			//HeatingbackendApplication.overrideUntilSwitch = null;
			//HeatingbackendApplication.info="Warnung, die Heizung ist regulär bereits " + (Boolean.parseBoolean(isTurnedOnString) ? "an":"aus" );
			//return HeatingbackendApplication.info;			
		}
	
		HeatingbackendApplication.overrideState =  Boolean.parseBoolean(isTurnedOnString);
		Api.switchHeating(HeatingbackendApplication.overrideState);
		
		int nowSwitch = Api.witchSwitchIsNow();
		
		if (HeatingbackendApplication.overrideState) {

			Integer nextHigherOnSwitch;
					
			if ("weekdays".equals(getTimeRangeTypeToday())){
				nextHigherOnSwitch = ((TreeSet<Integer>) HeatingbackendApplication.switches).higher(nowSwitch);
			}else {
				nextHigherOnSwitch = ((TreeSet<Integer>) HeatingbackendApplication.switchesWeekend).higher(nowSwitch);
			}
		
			System.out.println("Nowswitch:" + nowSwitch + " next on switch:" + nextHigherOnSwitch );

			if (nextHigherOnSwitch != null) {
				System.out.println("Heizungsoverride an bis nächster an Zyklus greift um " + Api.getDatefromSwitch(nextHigherOnSwitch));
				HeatingbackendApplication.overrideUntilSwitch = nextHigherOnSwitch-1;

			} else {				
				HeatingbackendApplication.overrideUntilSwitch = Api.getSwitchfromDate(HeatingbackendApplication.NORMALNIGHTTIME);
				System.out.println( "Heizungsoverride an bis zur regulären Nachtruhe um " + HeatingbackendApplication.NORMALNIGHTTIME);
			}

		} else {

			Integer counter = nowSwitch;
			Integer nextHigherOnSwitch = null;
			if ("weekdays".equals(getTimeRangeTypeToday())) {
				try {
					while ((counter + 1) == ((TreeSet<Integer>) HeatingbackendApplication.switches).higher(counter)) {
						System.out.println("###" + ((TreeSet<Integer>) HeatingbackendApplication.switches).higher(counter));
						nextHigherOnSwitch = ((TreeSet<Integer>) HeatingbackendApplication.switches).higher(counter);
						counter++;
					}
				}catch (Exception ignore){}
			
			}else {
				try {
					while ((counter + 1) == ((TreeSet<Integer>) HeatingbackendApplication.switchesWeekend).higher(counter)) {
						System.out.println("###" + ((TreeSet<Integer>) HeatingbackendApplication.switchesWeekend).higher(counter));
						nextHigherOnSwitch = ((TreeSet<Integer>) HeatingbackendApplication.switchesWeekend).higher(counter);
						counter++;
					}
				}catch (Exception ignore){}
				
			}
			
			System.out.println("Nowswitch:" + nowSwitch + " heating is on up to switch:" + nextHigherOnSwitch );

			if (nextHigherOnSwitch != null) {
				Integer nextOffSwitch = nextHigherOnSwitch ;
				HeatingbackendApplication.overrideUntilSwitch = nextOffSwitch;
				System.out.println("Heizung aus überschrieben bis nächster aus Zyklus greift um " + Api.getDatefromSwitch(HeatingbackendApplication.overrideUntilSwitch+1)+" switch:"+HeatingbackendApplication.overrideUntilSwitch);
			
			} else {
				HeatingbackendApplication.overrideUntilSwitch = nowSwitch;
				System.out.println("Heizung für aktuellen switch ausgeschalten, bis " + Api.getDatefromSwitch(HeatingbackendApplication.overrideUntilSwitch+1));
				
			}
			
			
		}
		
		HeatingbackendApplication.overrideDay= new Date(new Date().getYear(), new Date().getMonth(), new Date().getDay());		
		HeatingbackendApplication.info = "Heizung " + (HeatingbackendApplication.overrideState ? "an":"aus")+ " bis " +Api.getDatefromSwitch(HeatingbackendApplication.overrideUntilSwitch+1);
		return HeatingbackendApplication.info;
	}
	

	// @Autowired
	   // private SimpMessagingTemplate template;
	/*
	@RequestMapping("/api/sensortemperature")
	public String setSensorTemperature(@RequestBody double temperature) {		

			HeatingbackendApplication.sensorValues.add(new MeasuredValue(MeasureValueType.TEMPERATURE, temperature));
			System.out.println("Logging sensor value:" + temperature);
			Double averageTemperatureLastHour = 0.0;
			int countMeasuresLastHour=1;
					
			for (int i = HeatingbackendApplication.sensorValues.size()-1; i > -1; i--) {
				if(new Date().getTime()
						< ( new Date(new Date().getYear(), new Date().getMonth(), new Date().getDate(), new Date().getHours()+1, new Date().getMinutes()).getTime())) {
					averageTemperatureLastHour = averageTemperatureLastHour +  HeatingbackendApplication.sensorValues.get(i).getValue();
					countMeasuresLastHour++;
				}else {
					break;
				}
				
			}
			averageTemperatureLastHour = averageTemperatureLastHour / countMeasuresLastHour;
			if(averageTemperatureLastHour > 25) {
				switchHeating(false);
			}else if(averageTemperatureLastHour < 13) {
				switchHeating(true);
			}
			template.convertAndSend("/topic/greetings", "Heizung Durchschnittstemperatur der letzten Stunde beträgt: "+averageTemperatureLastHour +" Anzahl Messungen:" +countMeasuresLastHour);

		return "Heizung Durchschnittstemperatur der letzten Stunde beträgt: "+averageTemperatureLastHour  +" Anzahl Messungen:" +countMeasuresLastHour;
	}
	*/		
	
	/* only working with postman
	@RequestMapping("/api/sensorvalueparams")
	public String setSensorValueParams(@RequestParam("type") String type, @RequestParam("value") Double value) {		

			HeatingbackendApplication.sensorValues.add(new MeasuredValue(MeasureValueType.TEMPERATURE, value));
		
			Double averageTemperatureLastHour = 0.0;
			int countMeasuresLastHour=0;
					
			for (int i = HeatingbackendApplication.sensorValues.size()-1; i > -1; i--) {
				if(new Date().getTime()
						< ( new Date(new Date().getYear(), new Date().getMonth(), new Date().getDate(), new Date().getHours()+1, new Date().getMinutes()).getTime())) {
					averageTemperatureLastHour = averageTemperatureLastHour +  HeatingbackendApplication.sensorValues.get(i).getValue();
					countMeasuresLastHour++;
				}else {
					break;
				}
				
			}
			averageTemperatureLastHour = averageTemperatureLastHour / countMeasuresLastHour;
			if(averageTemperatureLastHour > 25) {
				switchHeating(false);
			}else if(averageTemperatureLastHour < 13) {
				switchHeating(true);
			}
		
			return "Heizung Durchschnittstemperatur der letzten Stunde beträgt: "+averageTemperatureLastHour +" Anzahl Messungen:" +countMeasuresLastHour;
	}
	*/
	


	
	@RequestMapping("/api/sensorvalue")
	public String setSensorValue(@RequestBody String body) {		
		
		ObjectMapper mapper = new ObjectMapper();
		
		double temperature = 0;
		double humidity = 0;
		try {
			JsonNode node = mapper.readTree(body);

			System.out.println("Total Measures before:"+ HeatingbackendApplication.sensorValues.size());
			Iterator<Entry<String, JsonNode>> fields = node.fields();
		
		    while (fields.hasNext()) {
		        Entry<String, JsonNode> jsonField = fields.next();
		        jsonField.getValue();
		        
		        HeatingbackendApplication.sensorValues.add(new MeasuredValue(jsonField.getKey(), jsonField.getValue().asDouble()));
		        
		        System.out.println("Logging sensor value temperature:" + jsonField.getKey() +" value:"+ jsonField.getValue().asDouble() +" Total Measures before:"+ HeatingbackendApplication.sensorValues.size());
		    }

		    
		    System.out.println("Total Measures before:"+ HeatingbackendApplication.sensorValues.size());

			System.out.println(" Total Measures after:"+ HeatingbackendApplication.sensorValues.size());
			
		
			Double averageTemperatureLastHour = 0.0;
			int countTemperatureMeasuresLastHour=0;
					
			for (int t = HeatingbackendApplication.sensorValues.size()-1; t > -1; t--) {
				if(
						HeatingbackendApplication.sensorValues.get(t).getDate().getTime()
						>= ( new Date(new Date().getYear(), new Date().getMonth(), new Date().getDate(), new Date().getHours()-1, new Date().getMinutes()).getTime())						
					
						
						) {					
					if (HeatingbackendApplication.sensorValues.get(t).getType().toUpperCase().equals("TEMPERATURE")) {
						averageTemperatureLastHour = averageTemperatureLastHour +  HeatingbackendApplication.sensorValues.get(t).getValue();
						countTemperatureMeasuresLastHour++;
					}
				}
				
				else {
					break;
				}
				
			}
			averageTemperatureLastHour = averageTemperatureLastHour / countTemperatureMeasuresLastHour;
			if(averageTemperatureLastHour > 24) {

				HeatingbackendApplication.overrideState = false;
				int switchInOneHoure = Api.getSwitchfromDate(new Date())+4;
				HeatingbackendApplication.overrideUntilSwitch = switchInOneHoure;	
				HeatingbackendApplication.overrideDay= new Date(new Date().getYear(), new Date().getMonth(), new Date().getDay());				
				Api.switchHeating(HeatingbackendApplication.overrideState);			
				HeatingbackendApplication.info="Heizung aufgrund Temperatur über 24 Grad aus bis "+Api.getDatefromSwitch(switchInOneHoure);
				
			}else if(averageTemperatureLastHour < 12) {
				HeatingbackendApplication.overrideState = true;
				int switchInOneHoure = Api.getSwitchfromDate(new Date())+4;
				HeatingbackendApplication.overrideUntilSwitch = switchInOneHoure;	
				HeatingbackendApplication.overrideDay= new Date(new Date().getYear(), new Date().getMonth(), new Date().getDay());				
				Api.switchHeating(HeatingbackendApplication.overrideState);			
				HeatingbackendApplication.info="Heizung aufgrund Temperatur unter 12 Grad an bis "+Api.getDatefromSwitch(switchInOneHoure);
			}
			
			

			Double averageHumidityLastHour = 0.0;
			int countHumidityMeasuresLastHour=0;
					
			for (int h = HeatingbackendApplication.sensorValues.size()-1; h > -1; h--) {
				if(HeatingbackendApplication.sensorValues.get(h).getDate().getTime()
						>= ( new Date(new Date().getYear(), new Date().getMonth(), new Date().getDate(), new Date().getHours()-1, new Date().getMinutes()).getTime())) {
					
					 if (HeatingbackendApplication.sensorValues.get(h).getType().toUpperCase().equals("HUMIDITY")) {
						averageHumidityLastHour = averageHumidityLastHour +  HeatingbackendApplication.sensorValues.get(h).getValue();
						countHumidityMeasuresLastHour++;
					}
				} else {
					break;
				}
				
			}
			averageHumidityLastHour = averageHumidityLastHour / countHumidityMeasuresLastHour;
			if(averageHumidityLastHour > 75) {
					
				HeatingbackendApplication.overrideState = true;
				int switchInOneHoure = Api.getSwitchfromDate(new Date())+4;
				HeatingbackendApplication.overrideUntilSwitch = switchInOneHoure;	
				HeatingbackendApplication.overrideDay= new Date(new Date().getYear(), new Date().getMonth(), new Date().getDay());				
				Api.switchHeating(HeatingbackendApplication.overrideState);			
				HeatingbackendApplication.info="Heizung aufgrund Feuchtigkeit über 75% an bis "+Api.getDatefromSwitch(switchInOneHoure);
				
			}
			HeatingbackendApplication.averageHumidityLastHour = averageHumidityLastHour;
			HeatingbackendApplication.averageTemperatureLastHour = averageTemperatureLastHour;

			return "Gemessene Temperatur: " +temperature+" Feuchtigkeit: " + humidity+" Heizung Durchschnittstemperatur der letzten Stunde beträgt: "
			+averageTemperatureLastHour +" Anzahl Messungen: " +countTemperatureMeasuresLastHour + " Feuchtigkeit: " + averageHumidityLastHour+" Anzahl Messungen: " +countHumidityMeasuresLastHour;
			
		}catch (Exception e) {
			System.out.println(e.getLocalizedMessage());
			return e.getLocalizedMessage();
			
		}
		

	}
	

	 
	 /* Boeblingen weather
	 
	 http://samples.openweathermap.org/data/2.5/weather?lat=48.6833&lon=9.0167&appid=2a381eafe17754824673d61f5153d099
	 */
	
	
	 public static String saveSwitchesToConfigFile(String timeRangeType) {
		 
		 System.out.println("trying to save config file");
		 String content = "";
		 
		 try {
		 
			// Create new file
			if (timeRangeType.contentEquals("weekdays")) {
				content = HeatingbackendApplication.switches.toString();
			}else {
				content = HeatingbackendApplication.switchesWeekend.toString();
			}
			File file = new File(HeatingbackendApplication.timerConfigPath+"timer_"+timeRangeType+".json");

			// If file doesn't exists, then create it
			if (!file.exists()) {
				file.createNewFile();
			}

			FileWriter fw = new FileWriter(file.getAbsoluteFile());
			BufferedWriter bw = new BufferedWriter(fw);

			// Write in file
			bw.write(content);

			// Close connection
			bw.close();
			System.out.println("successfully saved config file " + content);
			
		} catch (Exception e) {
			System.out.println(e);
		}
		 
		 return content;
	 }
	 
	 
	 
	 public static String readConfigFile(String config) {
		 
		String sCurrentLine = null;
	
		 try (BufferedReader br = new BufferedReader(new FileReader(HeatingbackendApplication.timerConfigPath+"timer_"+config+".json"))) {
			
				while ((sCurrentLine = br.readLine()) != null) {
					System.out.println("------------------------"+sCurrentLine);
					String trimmedContent = sCurrentLine.substring(1, sCurrentLine.length()-1);
					
					return  sCurrentLine;					
				}

			} catch (IOException e) {
				e.printStackTrace();
			}
	
		 
		  return  sCurrentLine;
	 }
	 
	 
	 public static void configFilesToSwitches() {

		 
			List <String> configFileNames = new ArrayList<String>();
			
			configFileNames.add("weekdays");	
			configFileNames.add("weekend");
			
			
			for (String configFileName: configFileNames) {
				
				File file = new File(HeatingbackendApplication.timerConfigPath+"timer_" + configFileName + ".json");

				// If file doesn't exists, then create it
				if (!file.exists()) {
					try {
						System.out.println("timer.json Config File to be created at:" + file.getAbsolutePath());
						file.createNewFile();
						System.out.println("timer.json Config File created at:" + file.getAbsolutePath());
					} catch (IOException e) {
						e.printStackTrace();
					}
				}else {
					System.out.println("existing timer.json config file exists at:" + file.getAbsolutePath());
				}
				
				
				try (BufferedReader br = new BufferedReader(new FileReader(HeatingbackendApplication.timerConfigPath+"timer_"+configFileName+".json"))) {

					String sCurrentLine;

					while ((sCurrentLine = br.readLine()) != null) {
						System.out.println("timer_"+ configFileName +".json: "+sCurrentLine);
						String trimmedContent = sCurrentLine.substring(1, sCurrentLine.length()-1);
						
						if (trimmedContent.length()>0) {
							String[] tokens = trimmedContent.split(", ");
						
							for(int t = 0; t < tokens.length; t++) {
								if (configFileName.equals("weekdays")) {
									HeatingbackendApplication.switches.add(Integer.parseInt(tokens[t]));
								}else {
									HeatingbackendApplication.switchesWeekend.add(Integer.parseInt(tokens[t]));
								}
							}
						}
					}

				} catch (IOException e) {
					e.printStackTrace();
				}
			 
				// Make sure gpio state is initially set right
				Api.switchHeating(isHeatingRegularyScheduledOnNow());
				System.out.println("Initial heating state:" + HeatingbackendApplication.isHeatingOn);
				

			}
		 		 
	 }
	 
	 public static boolean isHeatingRegularyScheduledOnNow() {
		
		int nowSwitch = Api.witchSwitchIsNow();
				
		if (new Date().getDay() != 0 && new Date().getDay() != 6) {
			return HeatingbackendApplication.switches.contains(nowSwitch);
		}else {
			return HeatingbackendApplication.switchesWeekend.contains(nowSwitch);
		}		 

	 }
	 
	 public static String getTimeRangeTypeToday() {
			
		int nowSwitch = Api.witchSwitchIsNow();
				
		if (new Date().getDay() != 0 && new Date().getDay() != 6 && new Date().getDay() != 5) {
			return "weekdays";
		}else {
			return "weekend";
		}		 

	 }
	 
	 
	 
	 public static void switchHeating(boolean isHeatingOn) {
				
			if (isHeatingOn) {
		   		Api.executeCommand("gpio export 4 out");
		   		HeatingbackendApplication.isHeatingOn = true;
		   		System.out.println("Switched pi relay on");

			}else {
		   		Api.executeCommand("gpio export 4 in");
		   		HeatingbackendApplication.isHeatingOn = false;
		   		System.out.println("Switched pi relay off");
			}
	 }
	 
	 
	 public static boolean toggleHeating() {
		 
			boolean isHeatingOn = !HeatingbackendApplication.isHeatingOn;
			Api.switchHeating(isHeatingOn);
			return isHeatingOn;
	 }
	 
	 
	 public static int witchSwitchIsNow() {
		
	        return Api.getSwitchfromDate(new Date());
		 
	 }
	 
	 public static Integer getSwitchfromDate(Date date) {

		 if(date!=null) {
		
			 int hours = date.getHours();
		        int minutes =date.getMinutes();
		        
		        int minuteSwitches = 0;
		        
		        if (minutes >= 15 && minutes < 30) {
		        	minuteSwitches = 1;
		        } else if (minutes >= 30 && minutes < 45) {
		        	minuteSwitches = 2;
		        } else if (minutes >= 45 && minutes < 60) {
		        	minuteSwitches = 3;
		        }

		        int switchNumber = hours * 4 + minuteSwitches;
		        
		        System.out.println("Switch number:" + switchNumber);
		        return switchNumber;
			 
			 
			 
		 }else {
			 return null;
		 }
		 
	 }
	 
	 public static Date getDatefromSwitch(Integer switcher) {
		 if(switcher!=null) {
		 	Date switchingTime = new Date();
		 
		 	switchingTime.setHours(switcher / 4 );
		 	switchingTime.setMinutes(switcher % 4 *15);
		 	switchingTime.setSeconds(0);

	        return switchingTime;
		 }else {
			 return null;
		 }
		 
	 }
	 
	 

	 
	 
		public static String executeCommand(String command) {

	        System.out.println("trying to execute linux command:" + command);
			StringBuffer output = new StringBuffer();

			Process p;
			try {
				p = Runtime.getRuntime().exec(command);
				p.waitFor();
				BufferedReader reader =
	                            new BufferedReader(new InputStreamReader(p.getInputStream()));

	                        String line = "";
				while ((line = reader.readLine())!= null) {
					output.append(line + "\n");
				}

				System.out.println("successfully executed linux command:" + output.toString());
			} catch (Exception e) {
				e.printStackTrace();
			}
			
			
			return output.toString();

		}
		
		
		@RequestMapping(value="/api/togglegaragedoor")
		public  String toggleGarageDoor() {		
			try {
			String prg = "#!/usr/bin/env python\n\nimport automationhat\n\nautomationhat.relay.one.on()";
			BufferedWriter out = new BufferedWriter(new FileWriter("/home/pi/Pimoroni/automationhat/examples/relay_on.py"));
			out.write(prg);
			out.close();

			String prg2 = "#!/usr/bin/env python\n\nimport automationhat\n\nautomationhat.relay.one.off()";
			BufferedWriter out2 = new BufferedWriter(new FileWriter("/home/pi/Pimoroni/automationhat/examples/relay_off.py"));
			out2.write(prg2);
			out2.close();
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			/** install autommation phat
			 * 
			 * sudo nano /etc/sudoers			 * 
			 * add 
			 * tomcat8 ALL = NOPASSWD: ALL
			 * 
			 * change python script comment   
			 * # GPIO.cleanup() 
			 * at /usr/lib/python2.7/dist-packages/__init__.py
			 * 
			 * https://forums.pimoroni.com/t/automation-hat-relay-operation/4528/2
			 */
	   		Api.executeCommand("sudo python /home/pi/Pimoroni/automationhat/examples/relay_on.py");
	   		try {
				Thread.sleep(200);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
	   		Api.executeCommand("sudo python /home/pi/Pimoroni/automationhat/examples/relay_off.py");
			return "toggled";
		}

	 
	 
		public static void executePythonScript(String pathScriptFile) {

			try{

				File f = new File(pathScriptFile);
				System.out.println(f.getAbsolutePath() +" exists:"+ f.exists());
				ProcessBuilder pb = new ProcessBuilder("python", pathScriptFile);
				pb.redirectErrorStream(true);
				Process p = pb.start();
				
				   for(String s:pb.command()){
			            System.out.println(s);
			        }
				   	String line;
					BufferedReader in = new BufferedReader(new InputStreamReader(p.getInputStream()));
					while ((line = in.readLine()) != null) {
						  System.out.println(line);
					}
		
				}catch(Exception e){System.out.println(e);}
			
			
		}
	
}
