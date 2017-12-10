
import { Component, Input, Output, EventEmitter, ElementRef, OnInit  } from '@angular/core';
import { Router, ActivatedRoute, Params } from '@angular/router';

import { HttpModule, RequestOptions, Headers, Http, URLSearchParams,  } from '@angular/http';

import { WebsocketService } from './websocket.service';
import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Time } from '@angular/common/src/i18n/locale_data_api';
import 'rxjs/operators/map';
import { Subject, Observable, Subscription } from 'rxjs/Rx';

import { ChartModule } from 'angular2-highcharts'; 

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})
export class AppComponent implements OnInit {


  title = 'app';

  averageTemperatureLasthHour: Number;
  averageHumidityLasthHour: Number;

  temperatureChartOptions: Object;
  humidityChartOptions: Object;

  private socket: Subject<any>;
  private sentMessage: string;

  timeOverride: Time;
  public static overrideUntilSwitch:number;
  message: string='';




  public static localhostString:string = 'localhost';
  constructor(private http:Http
    //, websocketService: WebsocketService
  ) {
   // this.socket = websocketService.createWebsocket();


   let headers = new Headers();
   headers.append('Content-Type', 'application/json');

   let params = new URLSearchParams();


   let dataPointsTemp: Array<number[]>;
   let dataPointsHumidity: Array<number[]>;

   this.http.get('http://'+ AppComponent.localhostString +':8080/api/getinfo/', { headers: headers, search: params } )
   .map(res => res.json())
   .subscribe(val => {
    console.log('info:'+val.temperature);
    this.averageTemperatureLasthHour =val.temperature;
    this.averageHumidityLasthHour =val.humidity;
    this.message =val.info;
    AppComponent.overrideUntilSwitch = val.overrideUntilSwitch;
    dataPointsTemp = new Array<number[]>();
    dataPointsHumidity = new Array<number[]>();
    let sensValues: SensorValue[] = val.sensorValues;
    //JSON.parse(val.sensorValues);

    for (let s=0; s < sensValues.length; s++ ){
      let timeStampValuePair: number[] = new Array<number>();
      timeStampValuePair.push(sensValues[s].date);
      timeStampValuePair.push(sensValues[s].value);
      if(sensValues[s].type === 'TEMPERATURE'){
        dataPointsTemp.push(timeStampValuePair);
      }
      if(sensValues[s].type === 'HUMIDITY'){
        dataPointsHumidity.push(timeStampValuePair);
      }
    }

    this.temperatureChartOptions = {
      title : { text : 'Temperatur' },
      series: [{
          data: dataPointsTemp,
      }],
      xAxis: {
        type: 'datetime',
        events: {
            afterSetExtremes: function(event) {
               var date = new Date(event.min);
                var datevalues = date.getFullYear()
                +'-'+ date.getMonth()+1
                +'-'+ date.getDate()
                +' '+ date.getUTCHours()
                +':'+ date.getMinutes()
                +':'+ date.getSeconds();
               
            }
        }
    },
  };

  this.humidityChartOptions = {
    title : { text : 'Feuchtigkeit' },
    series: [{
        data: dataPointsHumidity,
    }],
    xAxis: {
      type: 'datetime',
      events: {
          afterSetExtremes: function(event) {
             var date = new Date(event.min);
              var datevalues = date.getFullYear()
              +'-'+ date.getMonth()+1
              +'-'+ date.getDate()
              +' '+ date.getUTCHours()
              +':'+ date.getMinutes()
              +':'+ date.getSeconds();
             
          }
      }
  },
};

  });



  }

  ngOnInit(){
    /*
      this.socket.subscribe(
        message => this.message = message.data
      );*/
    }

  heatingTimed(event:Event){  
    event.preventDefault();
    console.log('timeOff:' + this.timeOverride +' '+this.timeOverride.hours+' ' + this.timeOverride.minutes );
      let now = new Date();
      console.log('now:' + now);
      now.setHours(Number(String(this.timeOverride).split(':')[0]) );
      now.setMinutes(Number(String(this.timeOverride).split(':')[1]));

      let headers = new Headers({ 'Content-Type': 'application/json' });
      let options = new RequestOptions({ headers: headers });

      console.log('dateOff:' + now);

      this.http.post('http://' + AppComponent.localhostString + ':8080/api/override/', JSON.stringify( now.getTime()), options ).subscribe(res => { 
        this.message = res.text();
      })

  }

switchHeating(doTurnOn: boolean){
    let headers = new Headers({ 'Content-Type': 'application/json' });
    let options = new RequestOptions({ headers: headers });
  
    this.http.post('http://'+ AppComponent.localhostString +':8080/api/switchheating/',  JSON.stringify( doTurnOn ), options ).subscribe(res => { 
      this.message = res.text();
    })
}


}


interface SensorValue{
  type: string;
  value: number;
  date: number;
}