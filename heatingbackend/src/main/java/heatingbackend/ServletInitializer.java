package heatingbackend;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;

import org.springframework.boot.builder.SpringApplicationBuilder;
import org.springframework.boot.web.servlet.support.SpringBootServletInitializer;

public class ServletInitializer extends SpringBootServletInitializer {

	@Override
	protected SpringApplicationBuilder configure(SpringApplicationBuilder application) {
		
		
		System.out.println("--------------------ServletInitializer started --------------------------");
		System.out.println("--------------------ServletInitializer started --------------------------");
		System.out.println("--------------------ServletInitializer started --------------------------");
		

		Api.configFilesToSwitches();
		
		return application.sources(HeatingbackendApplication.class);
	}

}
