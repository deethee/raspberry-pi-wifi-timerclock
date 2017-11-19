# raspbbery-timerclock
A wifi heating steering that replaces an old Viessmann Trimatik analogue timer clock.


Uses Angular 5 for frontend, Spring Boot as restful Backend, Lighthttpd Webserver for static Angular Files, Tomcat for the Spring Restful Services



# I am not not responsible for any damages:


# Instructions

Get a Raspberry Pi Zero with a min class 10 sd card

Format SD Card using SD Card Formatter

Download Noobs Raspian Image and copy on SD card. Install Raspian.

Get a 2 Channel Octocopler Relay wich handels 2 states

# Enter the shell commands:

sudo apt-get update
sudo apt-get upgrade

sudo apt-get install tomcat8
sudo apt-get install tomcat8-admin

sudo apt-get install lighttpd
#set rights
sudo groupadd www-data
sudo usermod -G www-data -a pi
sudo chown -R www-data:www-data /var/www/html
sudo chmod -R 775 /var/www/html

sudo groupadd www-data
sudo usermod -G www-data -a pi
sudo chown -R www-data:www-data /var/www/html
sudo chmod -R 775 /var/www/html


#copy content of frontend/dist to /var/www/html
#copy Spring boot war to /var/lib/tomcat8/webapps/ROOT.war


# Connections Rapberry Pi with a 2 Channel Relay:

Raspberry 5v(Pin2) to VCC 2 channel relay

Raspberry GND(Pin6) to GND of 2 channel relay

Raspberry GPIO 04(Pin7) to IN1 of 2 channel relay

RaspbRaspberry GPIO 27(Pin11) to IN2 of 2 channel relay




# Useful commands:
#Change port for frontend webapp
sudo nano /etc/lighttpd/lighttpd.conf
#force reload of frontend webserver
sudo lighttpd -t -f lighttpd.conf


#When backend war modified copy war in /home/pi/heetingsteeringfrontend/heatingbackend-0.0.1-SNAPSHOT.war usin e.g. MobaXterm
#remove old backend app
sudo rm -rf /var/lib/tomcat8/webapps/*
#deploy new app
sudo cp /home/pi/heetingsteeringfrontend/heatingbackend-0.0.1-SNAPSHOT.war /var/lib/tomcat8/webapps/ROOT.war

#realtime tomcat backend logs
tail -f /var/lib/tomcat8/logs/catalina.out


# Development on Windows PC:

Install mobaxterm

Install Eclipse
Download eclipse Spring Boot Plugin via marketplace sts(Spring tools)
File new Spring Starter Project, Install with Web 
Run Project as Spring Boot App

Install Nodejs
Install git
Install Visual studio code
#install Angular CLI
sudo npm install @angular/cli –g
#New Angular Project
ng new angular-frontend
cd angular-frontend
ng serve
http://localhost:4200

