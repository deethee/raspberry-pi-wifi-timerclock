# raspbbery-timerclock
A wifi heating steering that replaces an old Viessmann Trimatik analogue timer clock.


Uses Angular 5 for frontend, Spring Boot as restful Backend, Lighthttpd Webserver for static Angular Files, Tomcat for the Spring Restservices


Not responsible for any damages:

Get a Raspberry Pi Zero with a min class 10 sd card

Format SD Card using SD Card Formatter

Download Noobs Raspian Image and copy on SD card. Install Raspian.

Enter the shell commands:


sudo apt-get update
sudo apt-get upgrade

sudo apt-get install tomcat8
sudo apt-get install tomcat8-admin

sudo apt-get install lighttpd

sudo groupadd www-data
sudo usermod -G www-data -a pi
sudo chown -R www-data:www-data /var/www/html
sudo chmod -R 775 /var/www/html


copy content of frontend/dist to /var/www/html

copy Spring boot war to /

Useful commands:
#Change port for frontend webapp
sudo nano /etc/lighttpd/lighttpd.conf
#force reload of frontend webserver
sudo lighttpd -t -f lighttpd.conf

sudo apt-get install lighttpd
#set rights
sudo groupadd www-data
sudo usermod -G www-data -a pi
sudo chown -R www-data:www-data /var/www/html
sudo chmod -R 775 /var/www/html

#When backend war modified copy war in /home/pi/heetingsteeringfrontend/heatingbackend-0.0.1-SNAPSHOT.war usin e.g. MobaXterm
#remove old backend app
sudo rm -rf /var/lib/tomcat8/webapps/*
#deploy new app
sudo cp /home/pi/heetingsteeringfrontend/heatingbackend-0.0.1-SNAPSHOT.war /var/lib/tomcat8/webapps/ROOT.war

#realtime tomcat backend logs
tail -f /var/lib/tomcat8/logs/catalina.out

