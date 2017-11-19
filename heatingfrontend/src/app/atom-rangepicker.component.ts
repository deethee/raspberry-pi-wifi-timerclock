/**
 * Created by VCL7WE on 10.11.2017.
 */

import { Component, Input, Output, EventEmitter, ElementRef, OnInit} from '@angular/core';
import { Router, ActivatedRoute, Params } from '@angular/router';

import { HttpModule, RequestOptions, Headers, Http, URLSearchParams,  } from '@angular/http';

import { HttpClient, HttpHeaders } from '@angular/common/http';

import { AppComponent } from './app.component';

import 'rxjs/add/operator/map';

@Component({
  moduleId: module.id,
  selector: 'atom-rangepicker',
  templateUrl: 'atom-rangepicker.component.html',
  providers: []
})



export class AtomRangePickerComponent  implements OnInit{

  /** usually 24 hours  */
  @Input() divisions: number = 24;
  @Input() subDivisions: number = 4;

  @Input() type: string;

  switchesActive: number[];
  switches: Switch[];

  constructor(private http:Http) {
    
    this.switches = new Array<Switch>();
    for (let i = 0; i < (this.divisions * this.subDivisions); i++) {
              this.switches.push(new Switch(false));
     }
     console.log('setup default switches' + this.switches);
    // localhost

  }


  ngOnInit() {
    let headers = new Headers();
    headers.append('Content-Type', 'application/json');

    let params = new URLSearchParams();
    console.log('##################################'+this.type)
    params.append("timeRangeType", this.type);

    this.http.get('http://'+ AppComponent.localhostString +':8080/api/gettimerconfig/', { headers: headers, search: params } ).map(res => res.json()).subscribe(val => {
      console.log('LoadedConfig:'+val);

      this.switches = new Array<Switch>();
      this.switchesActive = val; //[4, 5, 16, 17, 18];

      for (let i = 0; i < (this.divisions * this.subDivisions); i++) {
  
        if (this.switchesActive.indexOf(i) > -1) {
          this.switches.push(new Switch(true));
        } else {
          this.switches.push(new Switch(false));
        }
     
      }
      console.log('setup switches' + this.switches);

    });
  }

  toggleSwitch(index: number) {
    console.log(index);
    this.switches[index].active = !this.switches[index].active;

    let data = new URLSearchParams();
    data.append('switchIndex', ''+index);


    let headers = new Headers({ 'Content-Type': 'application/json' });
    let options = new RequestOptions({ headers: headers });

    this.http.post('http://'+ AppComponent.localhostString +':8080/api/toggleswitch/',  JSON.stringify({switchIndex: index, timeRangeType:this.type }), options  ).subscribe();
  }

  toggleHourSwitch(index: number) {
    console.log('toggleHourSwitchindex:'+index);
    console.log('toggleHourSwitchindex:'+(index*this.subDivisions));
    console.log('toggleHourSwitchindex:'+(index*this.subDivisions+1));
    console.log('toggleHourSwitchindex:'+(index*this.subDivisions+2));
    console.log('toggleHourSwitchindex:'+(index*this.subDivisions+3));

    this.switches[(index*this.subDivisions+1)].active = !this.switches[index*this.subDivisions].active;
    this.switches[(index*this.subDivisions+2)].active = !this.switches[index*this.subDivisions].active;
    this.switches[(index*this.subDivisions+3)].active = !this.switches[index*this.subDivisions].active;
    this.switches[(index*this.subDivisions)].active = !this.switches[index*this.subDivisions].active;

    let headers = new Headers({ 'Content-Type': 'application/json' });
    let options = new RequestOptions({ headers: headers });

    this.http.post('http://'+ AppComponent.localhostString +':8080/api/togglehour/',  JSON.stringify({hour: index, timeRangeType:this.type }), options ).subscribe();

    
  }

}

  export class Switch {
    active: boolean;
    constructor(active: boolean) {
      this.active = active;
    }
  }

  export class transfer {
    weekdays: number[];
    weekend: number[];

    constructor(weekdays: number[], weekend:number[]) {
    this.weekdays = weekdays;
    this.weekend = weekend;
    }
  }






