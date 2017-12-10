import { Component, Input } from '@angular/core';

@Component({
   selector: 'atom-chart',
   template: `
       <chart [options]="options"></chart>
   `
})

//let dataPoints: Array<number[]>;




export class AtomChartComponent {

    @Input() options: Object;

   constructor() {
       {
           /*
            this.options = {
                title : { text : 'simple chart' },
                series: [{
                    data: [29.9, 71.5, 106.4, 129.2],
                }]
            };*/
        }
   }

}



