/**
 * Created by VCL7WE on 10.11.2017.
 */

import { Pipe, PipeTransform } from '@angular/core';

@Pipe({name: 'numberToArray'})
export class NumberToArrayPipe implements PipeTransform {
  transform(value, args:string[]) : any {
    let res = [];
    for (let i = 0; i < value; i++) {
        res.push(i);
      }
      return res;
  }
}

