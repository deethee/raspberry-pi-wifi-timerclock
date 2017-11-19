package heatingbackend;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.SortedSet;
import java.util.TreeSet;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.scheduling.annotation.EnableScheduling;

@SpringBootApplication
@EnableScheduling
public class HeatingbackendApplication {
	
	public static SortedSet <Integer> switches = new TreeSet<Integer>();
	public static SortedSet <Integer> switchesWeekend = new TreeSet<Integer>();
	
	
	public static boolean isHeatingOn = false;

	public static Integer overrideUntilSwitch=null;
	public static Boolean overrideState=null;
	public static Date overrideDay=null;
	
	public static String timerConfigPath = "/home/pi/heetingsteeringconfig/";

	public static Date NORMALNIGHTTIME = new Date(new Date().getYear(), new Date().getMonth(), 22);
	
	
	public static void main(String[] args) {
		
		System.out.println("--------------------main started --------------------------");
		System.out.println("--------------------main started --------------------------");
		System.out.println("--------------------main started --------------------------");
		

		Api.configFilesToSwitches();
		
		SpringApplication.run(HeatingbackendApplication.class, args);
		
		
	}
}
