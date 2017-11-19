
import { Component, Input, Output, EventEmitter, ElementRef } from '@angular/core';
import { Router, ActivatedRoute, Params } from '@angular/router';

import { HttpModule, RequestOptions, Headers, Http, URLSearchParams,  } from '@angular/http';

import { HttpClient, HttpHeaders } from '@angular/common/http';
import { Time } from '@angular/common/src/i18n/locale_data_api';
import 'rxjs/operators/map';
@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})
export class AppComponent {
  title = 'app';

  public static localhostString:string = '192.168.0.37';

  constructor(private http:Http) {

  }
  timeOverride:Time;
  message:string='';


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
