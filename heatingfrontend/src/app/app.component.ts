
import { Component, Input, Output, EventEmitter, ElementRef, OnInit  } from '@angular/core';
import { Router, ActivatedRoute, Params } from '@angular/router';

import { HttpModule, RequestOptions, Headers, Http, URLSearchParams,  } from '@angular/http';

import { WebsocketService } from './websocket.service';
import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Time } from '@angular/common/src/i18n/locale_data_api';
import 'rxjs/operators/map';
import { Subject, Observable, Subscription } from 'rxjs/Rx';

import { ChartModule, ChartSeriesComponent } from 'angular2-highcharts';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})
export class AppComponent implements OnInit {

  public static overrideUntilSwitch: number;
  public static localhostString = 'localhost';

  title = 'app';

  averageTemperatureLasthHour: Number;
  averageHumidityLasthHour: Number;

  temperatureChartOptions: Object;
  humidityChartOptions: Object;

  private socket: Subject<any>;
  private sentMessage: string;

  timeOverride: Time;

  message = '';



  graphNames: Set<string>;
  charts: Chart[];

  constructor(private http: Http
    // , websocketService: WebsocketService
  ) {
   // this.socket = websocketService.createWebsocket();


   const headers = new Headers();
   headers.append('Content-Type', 'application/json');

   const params = new URLSearchParams();



   let dataPointsHumidity: Array<number[]>;

   this.http.get('http://' + AppComponent.localhostString + ':8080/api/getinfo/', { headers: headers, search: params } )
   .map(res => res.json())
   .subscribe(val => {
    console.log('info:' + val.temperature);
    this.averageTemperatureLasthHour = val.temperature;
    this.averageHumidityLasthHour = val.humidity;
    this.message = val.info;
    AppComponent.overrideUntilSwitch = val.overrideUntilSwitch;

    dataPointsHumidity = new Array<number[]>();
    const sensValues: SensorValue[] = val.sensorValues;
    // JSON.parse(val.sensorValues);

    this.graphNames = new Set<string>();
    sensValues.forEach(element => {
      this.graphNames.add(element.type);
    });
    this.charts = new Array<Chart>();
     this.graphNames.forEach(graphName => {
       console.log('sensorvalues for: ' + graphName);
      const dataPoints: Array<number[]> = new Array<number[]>();



       for (let s = 0; s < sensValues.length; s++) {
         if (sensValues[s].type === graphName) {
           const timeStampValuePair: number[] = new Array<number>();
           timeStampValuePair.push(sensValues[s].date);
           timeStampValuePair.push(sensValues[s].value);
           dataPoints.push(timeStampValuePair);
         }

        console.log(this.charts.length);

      }

      this.charts.push( {
        name: graphName,
         chartOptions : {
           title: { text: graphName },
           series: [{
             data: dataPoints,
           }],
           xAxis: {
             type: 'datetime',
             events: {
               afterSetExtremes: function (event) {
                 var date = new Date(event.min);
                 var datevalues = date.getFullYear()
                   + '-' + date.getMonth() + 1
                   + '-' + date.getDate()
                   + ' ' + date.getUTCHours()
                   + ':' + date.getMinutes()
                   + ':' + date.getSeconds();

               }
             }
           },
         }
        });

     });
console.log(this.charts.length);


   });



  }

  ngOnInit() {
    /*
      this.socket.subscribe(
        message => this.message = message.data
      );*/
    }

  heatingTimed(event: Event) {
    event.preventDefault();
    console.log('timeOff:' + this.timeOverride + ' ' + this.timeOverride.hours + ' ' + this.timeOverride.minutes );
      const now = new Date();
      console.log('now:' + now);
      now.setHours(Number(String(this.timeOverride).split(':')[0]) );
      now.setMinutes(Number(String(this.timeOverride).split(':')[1]));

      const headers = new Headers({ 'Content-Type': 'application/json' });
      const options = new RequestOptions({ headers: headers });

      console.log('dateOff:' + now);

      this.http.post('http://' + AppComponent.localhostString + ':8080/api/override/',
      JSON.stringify( now.getTime()), options ).subscribe(res => {
        this.message = res.text();
      });

  }

switchHeating(doTurnOn: boolean) {
    const headers = new Headers({ 'Content-Type': 'application/json' });
    const options = new RequestOptions({ headers: headers });

    this.http.post('http://' + AppComponent.localhostString + ':8080/api/switchheating/',
    JSON.stringify( doTurnOn ), options ).subscribe(res => {
      this.message = res.text();
    });
}

toggleGarageDoor() {
  const headers = new Headers({ 'Content-Type': 'application/json' });
  const options = new RequestOptions({ headers: headers });

  this.http.post('http://192.168.0.46/toggle',  JSON.stringify( '' ), options ).subscribe(res => {
    this.message = res.text();
  });
}


}


interface SensorValue {
  type: string;
  value: number;
  date: number;
}
interface Chart {
  name: string;
  chartOptions: any;
}
