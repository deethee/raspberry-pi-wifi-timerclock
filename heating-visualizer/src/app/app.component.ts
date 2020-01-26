import * as THREE from 'three-full';

import { Injectable, ElementRef, OnInit, OnDestroy, NgZone, Component, ViewChild  } from '@angular/core';
import { analyzeAndValidateNgModules } from '@angular/compiler';


@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})


// https://threejsfundamentals.org/threejs/lessons/threejs-align-html-elements-to-3d.html
// https://threejsfundamentals.org/threejs/lessons/threejs-canvas-textures.html
// https://github.com/JohnnyDevNull/ng-three-template

// install node
// npm install npm@latest -g
// npm install -g @angular/cli
// ng new heating-visualizer
// npm install --save three-full
// npm install @types/three --save-dev
// npm install @types/webgl2 --save-dev
// npm install @types/offscreencanvas --save-dev

export class AppComponent implements OnInit, OnDestroy {

  private canvas: HTMLCanvasElement;
  private renderer: THREE.WebGLRenderer;
  private camera: THREE.PerspectiveCamera;
  private scene: THREE.Scene;
  private lightDirectional: THREE.directionalLight;
  private lightAmbient: THREE.AmbientLight;

  private circle: THREE.Mesh;

  private frameId: number = null;

  loader3d: THREE.OBJLoader; // GLTFLoader;
  fontLoader: THREE.FontLoader;



  draggable: any[];
  controls: THREE.OrbitControls;

  mouseDragControls: THREE.DragControls;

  datapoints: any;

  geometries: THREE.Geometry[];
  // geometry with texture
  datapointMeshes: THREE.Mesh[];
  colorsBosh: THREE.Color[];
  materials: THREE.Material[]

  @ViewChild('rendererCanvas', { static: true })
  public rendererCanvas: ElementRef<HTMLCanvasElement>;

  constructor(private ngZone: NgZone) {

    this.draggable = new Array();
    this.loader3d = new THREE.OBJLoader();

    this.colorsBosh = new Array<THREE.Color>();
    this.materials = new Array<THREE.Material>();
    this.geometries = new Array<THREE.Geometry>();
    this.datapointMeshes = new Array<THREE.Mesh>();

    const color = new THREE.Color( 'green' );
    this.colorsBosh.push(color); // TODO pushing not working


    this.materials.push(new THREE.MeshPhongMaterial( {color}));

    this.geometries.push( new THREE.CylinderGeometry(0.07, 0.07, 0.01, 32));





  }


  loadData() {
    this.datapoints =   [
      {address: '.1.3.6.1.4.1.8284.2.1.3.1.11.1.4.1001', name: 'Vorlauftemperatur', value: 86, unit: '°C', x: -2.7, y: 2.5, z: 0 } ,
      {address: '.1.3.6.1.4.1.8284.2.1.3.1.11.1.4.1002', name: 'Rücklauflauftemperatur', value: 66, unit: '°C', x: -0.2, y: 2.5, z: 0 },
      {address: '.1.3.6.1.4.1.8284.2.1.3.1.11.1.4.1003', name: 'Leistung', value: 230, unit: 'KW', x: 2, y: 1.7, z: 0 }
    ];

    for (let datapoint of this.datapoints) {

      let dpMesh = new THREE.Mesh( this.geometries[0], this.materials[0] );

      dpMesh.position.x = datapoint.x;
      dpMesh.position.y = datapoint.y;
      dpMesh.position.z = datapoint.z;
      dpMesh.rotation.x =  Math.PI / 2;

      this.datapointMeshes.push( dpMesh );
      this.scene.add(dpMesh);
      this.draggable.push(dpMesh);

      // datapoint labels are html elements
      const labelContainerElem = document.querySelector('#labels');
      const elem = document.createElement('div');
      elem.setAttribute('id', datapoint.address);
      elem.innerHTML = datapoint.name + '&nbsp;<span id="value_' + datapoint.address + '">' + datapoint.value + '</span>&nbsp;' + datapoint.unit;
      labelContainerElem.appendChild(elem);
      console.log('constructed');

    }
    this.subscribeWS();

  }


  ngOnInit() {
    this.createScene(this.rendererCanvas);
    this.animate();
  }


  ngOnDestroy() {
    if (this.frameId != null) {
      cancelAnimationFrame(this.frameId);
    }
  }

  createScene(canvas: ElementRef<HTMLCanvasElement>): void {

    // The first step is to get the reference of the canvas element from our HTML document
    this.canvas = canvas.nativeElement;

    this.renderer = new THREE.WebGLRenderer({
      canvas: this.canvas,
      alpha: true,    // transparent background
      antialias: true // smooth edges
    });
    this.renderer.setSize(window.innerWidth, window.innerHeight);


    // create the scene
    this.scene = new THREE.Scene();

    // add a camera
    this.camera = new THREE.PerspectiveCamera(
      75, window.innerWidth / window.innerHeight, 0.1, 1000
    );
    this.camera.position.z = 5;
    this.camera.position.y = 1.6;
    this.scene.add(this.camera);

    // lights
    this.lightDirectional = new THREE.DirectionalLight( 0x404040 );
    this.lightDirectional.position.z = 6;
    this.scene.add(this.lightDirectional);
    this.lightAmbient = new THREE.AmbientLight( 0x404040, 0.3 );
    this.scene.add(this.lightAmbient);

     // 3d import of commercial heating
    this.loader3d.load( '../assets/commercialHeater.obj', ( obj ) => {
      obj.position.y = -0.4;
      this.scene.add( obj );
    }, undefined, ( error ) => {
      alert(error);
      console.error( error );
    } );


    this.loadData();


    // drag and drop datapoints
    this.mouseDragControls = new THREE.DragControls( this.draggable, this.camera, this.renderer.domElement );

    this.mouseDragControls.addEventListener( 'dragstart',  ( event ) => {
      event.object.material.emissive.set( 0xaa6aaa );
    } );

    this.mouseDragControls.addEventListener( 'dragend', ( event ) => {
      event.object.material.emissive.set( 0x000000 );
    } );

    this.controls = new THREE.OrbitControls(this.camera, this.canvas);
    this.controls.target.set(0, 0, 0);

    this.controls.mouseButtons = {LEFT: THREE.MOUSE.MIDDLE, MIDDLE: THREE.MOUSE.RIGHT, RIGHT: THREE.MOUSE.LEFT};
    this.controls.enablePan = false;
    this.controls.update();

  }




  animate(): void {
    // We have to run this outside angular zones,
    // because it could trigger heavy changeDetection cycles.
    this.ngZone.runOutsideAngular(() => {
      if (document.readyState !== 'loading') {
        this.render();
      } else {
        window.addEventListener('DOMContentLoaded', () => {
          this.render();
        });
      }

      window.addEventListener('resize', () => {
        this.resize();
      });
    });
  }

  render() {
    this.frameId = requestAnimationFrame(() => {
      this.render();
    });
    this.updateLabelPositions();

    this.renderer.render(this.scene, this.camera);
  }

  updateLabelPositions() {
     this.datapointMeshes.forEach((dp, ndx) => {
      const tempVector: THREE.Vector3 = new THREE.Vector3();
      // get the position of the center of the cube
      dp.updateWorldMatrix(true, false);
      dp.getWorldPosition(tempVector);

      // get the normalized screen coordinate of that position
      // x and y will be in the -1 to +1 range with x = -1 being
      // on the left and y = -1 being on the bottom

      tempVector.project(this.camera);
      console.log('tempVector:' + tempVector.y + 'this.canvas.clientHeight' + this.canvas.clientHeight);
      // convert the normalized position to CSS coordinates
      const x = (tempVector.x *  .5 + .5) * this.canvas.clientWidth
                  + (document.getElementById(this.datapoints[ndx].address).offsetWidth / 2) + 20;

      const y = (tempVector.y * -.5 + .5) * this.canvas.clientHeight;

      // move the elem to that position
      document.getElementById(this.datapoints[ndx].address).style.transform = `translate(-50%, -50%) translate(${x}px,${y}px)`;
      console.log('x:' + x + '  y:' + y);
    });
  }



  subscribeWS() {
    setInterval(() => {
      this.updateValue('.1.3.6.1.4.1.8284.2.1.3.1.11.1.4.1001',
             Number(document.getElementById('value_' + '.1.3.6.1.4.1.8284.2.1.3.1.11.1.4.1001').innerHTML) + 1 );

    }, 2000);
  }

  updateValue(dpAdress: string, value: number) {
    document.getElementById('value_' + dpAdress).innerHTML = '' + value;
    //'' +  Number(document.getElementById('value_' + dpAdress).innerHTML) + 1;
  }


  resize() {
    const width = window.innerWidth;
    const height = window.innerHeight;

    this.camera.aspect = width / height;
    this.camera.updateProjectionMatrix();

    this.renderer.setSize( width, height );
  }

}
