aspbbery-timerclock

A wifi heating steering that replaces an old Viessmann Trimatik analogue timer clock.

Uses Angular 5 for frontend, Spring Boot as restful Backend, Lighthttpd Webserver for static Angular Files, Tomcat for the Spring Restful Services

I am not not responsible for any damages:

Instructions

Get a Raspberry Pi Zero with a min class 10 sd card

Format SD Card using SD Card Formatter

Download Noobs Raspian Image and copy on SD card. Install Raspian.

Get a 2 Channel Octocopler Relay wich handels 2 states (NO (normally open), NC (normally close), GND (Ground)) and has 3 connections per channel

Enter the shell commands:

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

mkdir /home/pi/heetingsteeringfrontend/

mkdir /home/pi/heetingsteeringconfig/

touch /home/pi/heetingsteeringconfig/timer_weekdays.json

touch /home/pi/heetingsteeringconfig/timer_weekend.json

sudo chown tomcat8:tomcat8 /home/pi/heetingsteeringconfig/timer__weekdays.json

sudo chown tomcat8:tomcat8 /home/pi/heetingsteeringconfig/timer_weekend.json

Copy Files

copy content of frontend/dist to /var/www/html

copy Spring boot war to /var/lib/tomcat8/webapps/ROOT.war

Connections Rapberry Pi with a 2 Channel Relay:

Raspberry 5v(Pin2) to VCC 2 channel relay

Raspberry GND(Pin6) to GND of 2 channel relay

Raspberry GPIO 04(Pin7) to IN1 of 2 channel relay

RaspbRaspberry GPIO 27(Pin11) to IN2 of 2 channel relay

Useful commands:

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

Further development on Windows PC:

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



# Heatingfrontend

This project was generated with [Angular CLI](https://github.com/angular/angular-cli) version 1.5.0.

## Development server

Run `ng serve` for a dev server. Navigate to `http://localhost:4200/`. The app will automatically reload if you change any of the source files.

## Code scaffolding

Run `ng generate component component-name` to generate a new component. You can also use `ng generate directive|pipe|service|class|guard|interface|enum|module`.

## Build

Run `ng build` to build the project. The build artifacts will be stored in the `dist/` directory. Use the `-prod` flag for a production build.

## Running unit tests

Run `ng test` to execute the unit tests via [Karma](https://karma-runner.github.io).

## Running end-to-end tests

Run `ng e2e` to execute the end-to-end tests via [Protractor](http://www.protractortest.org/).

## Further help

To get more help on the Angular CLI use `ng help` or go check out the [Angular CLI README](https://github.com/angular/angular-cli/blob/master/README.md).
