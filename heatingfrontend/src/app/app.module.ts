import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';


import { AppComponent } from './app.component';

import { AtomRangePickerComponent } from './atom-rangepicker.component';

import { NumberToArrayPipe } from './numbertoarray';


import { HttpModule } from '@angular/http';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';

import { AtomChartComponent } from './atom-chart.component';

import { HighchartsStatic } from 'angular2-highcharts/dist/HighchartsService';
import * as highcharts from 'highcharts';
import { ChartModule } from 'angular2-highcharts';

export function highchartsFactory() {
  return highcharts;
}


@NgModule({

  declarations: [
    AppComponent, AtomRangePickerComponent, AtomChartComponent, NumberToArrayPipe
  ],
  imports: [
    BrowserModule, HttpModule, FormsModule, ReactiveFormsModule, ChartModule
  ],
  providers: [
    {
      provide: HighchartsStatic,
      useFactory: highchartsFactory
    },
  ],
  bootstrap: [AppComponent]
})
export class AppModule { }
