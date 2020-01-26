import * as THREE from 'three-full';


import { Injectable, ElementRef, OnInit, OnDestroy, NgZone, Component, ViewChild  } from '@angular/core';

interface Scheme {
  path: string;
  position: THREE.Vector3;
  datapoints: Datapoint[];
  mesh?: THREE.Mesh[];
}

interface Datapoint {
  address: string;
  label: string;
  value: number;
  unit: string;
  graphPosition: THREE.Vector3;
}

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

  private frameId: number = null;

  loaderOBJ: THREE.OBJLoader; // GLTFLoader;
  loaderGLTF: THREE.GLTFLoader; // GLTFLoader;
  loaderFBX: THREE.FBXLoader; // GLTFLoader;
  fontLoader: THREE.FontLoader;

  draggable: any[];
  controls: THREE.OrbitControls;
  mouseDragControls: THREE.DragControls;

  // arrays of default colors/materials/geometry
  geometries: THREE.Geometry[];
  // geometry with texture
  colorsBosh: THREE.Color[];
  materials: THREE.Material[];

  schemes: Scheme[];
  schemeSelected: Scheme;

  datapointMeshesParent: THREE.Object3D;
  datapointMeshes: THREE.Mesh;



  @ViewChild('rendererCanvas', { static: true })
  public rendererCanvas: ElementRef<HTMLCanvasElement>;

  constructor(private ngZone: NgZone) {

    this.draggable = new Array();
    this.loaderOBJ = new THREE.OBJLoader();
    this.loaderGLTF = new THREE.GLTFLoader();
    this.loaderFBX = new THREE.FBXLoader();

    this.colorsBosh = new Array<THREE.Color>();
    this.materials = new Array<THREE.Material>();
    this.geometries = new Array<THREE.Geometry>();



    const color = new THREE.Color( 'green' );
    // TODO pushing not working: this.colorsBosh.push(color);

    this.materials.push(new THREE.MeshPhongMaterial( {color}));

    this.geometries.push( new THREE.CylinderGeometry(0.07, 0.07, 0.01, 32));

  }


  loadData(schemeIndex: number) {

    this.schemes = [

      {
        path: 'commercialHeater1.obj',
        position: { x: 0, y: -0.4, z: 0 },
        datapoints: [

          { address: '.1.3.6.1.4.1.8284.2.1.3.1.11.1.4.1001', label: 'Vorlauftemperatur', value: 86, unit: '°C', graphPosition: { x: -2.7, y: 2.5, z: 0 } },
          { address: '.1.3.6.1.4.1.8284.2.1.3.1.11.1.4.1002', label: 'Rücklauflauftemperatur', value: 66, unit: '°C', graphPosition: { x: -0.2, y: 2.5, z: 0 } },
          { address: '.1.3.6.1.4.1.8284.2.1.3.1.11.1.4.1003', label: 'Leistung', value: 230, unit: 'KW', graphPosition: { x: 2, y: 1.7, z: 0 } }
        ]
      },
      {
        path: 'commercialHeater2.obj',
        position: { x: 0, y: -0.4, z: 0 },
        datapoints: [
          { address: '.1.3.6.1.4.1.8284.2.1.3.1.11.1.4.1001', label: 'Vorlauftemperatur', value: 86, unit: '°C', graphPosition: { x: -2.7, y: 2.5, z: 0 } },
          { address: '.1.3.6.1.4.1.8284.2.1.3.1.11.1.4.1002', label: 'Rücklauflauftemperatur', value: 66, unit: '°C', graphPosition: { x: -0.2, y: 2.5, z: 0 } },
          { address: '.1.3.6.1.4.1.8284.2.1.3.1.11.1.4.1003', label: 'Leistung', value: 230, unit: 'KW', graphPosition: { x: 0.5, y: 1.7, z: 0 } }
        ]
      },
      {
        path: 'commercialHeater3.obj',
        position: { x: -100, y: -170, z: -400 },
        datapoints: [
          { address: '.1.3.6.1.4.1.8284.2.1.3.1.11.1.4.1001', label: 'Vorlauftemperatur', value: 86, unit: '°C', graphPosition: { x: -220.7, y: 2.5, z: -400 } },
          { address: '.1.3.6.1.4.1.8284.2.1.3.1.11.1.4.1002', label: 'Rücklauflauftemperatur', value: 66, unit: '°C', graphPosition: { x: -0.2, y: 2.5, z: -400 } },
          { address: '.1.3.6.1.4.1.8284.2.1.3.1.11.1.4.1003', label: 'Leistung', value: 230, unit: 'KW', graphPosition: { x: 2, y: -100, z: -400 } }
        ]
      }

    ];



    this.schemeSelected = this.schemes[0];
    // 3d import of commercial heating
    this.loaderOBJ.load( '../assets/' + this.schemeSelected.path, ( obj ) => {
      obj.position.x = this.schemes[0].position.x;
      obj.position.y = this.schemes[0].position.y;
      obj.position.z = this.schemes[0].position.z;
      obj.name = this.schemes[0].path;
      this.scene.add( obj );
      this.schemeSelected.mesh = obj;

    }, undefined, ( error ) => {
      alert(error);
      console.error( error );
    } );

    this.placeLabels(0);

/*
    this.loaderFBX.load( '../assets/commercialHeater.fbx', ( fbx ) => {
      fbx.position.y = -0.4;
      this.scene.add( fbx );
    }, undefined, ( error ) => {
      alert(error);
      console.error( error );
    } );
*/

/*
    this.loaderGLTF.load( '../assets/scene.gltf', ( gltf ) => {
      gltf.position.y = -0.4;
      this.scene.add( gltf.scene );
    }, undefined, ( error ) => {
      alert(error);
      console.error( error );
    } );
*/


    this.subscribeWS();

  }


   // place html labels with cylinder mesh
   placeLabels(schemeIndex: number) {

    const oldDatapointMeshesParent = this.scene.getObjectByName( 'datapointMeshesParent');
    this.scene.remove( oldDatapointMeshesParent );

    let parentHtml = document.getElementById('labels');
    while (parentHtml.firstChild) {
      parentHtml.firstChild.remove();
    }


    // the cylinders to place
    this.datapointMeshesParent = new THREE.Object3D();
    this.datapointMeshesParent.name = 'datapointMeshesParent';
    this.datapointMeshes = new Array<THREE.Mesh>();



    for (const datapoint of this.schemes[schemeIndex].datapoints) {

      const dpMesh = new THREE.Mesh(this.geometries[0], this.materials[0]);

      dpMesh.position.x = datapoint.graphPosition.x;
      dpMesh.position.y = datapoint.graphPosition.y;
      dpMesh.position.z = datapoint.graphPosition.z;
      dpMesh.rotation.x = Math.PI / 2;

      this.datapointMeshes.push(dpMesh);
      this.draggable.push(dpMesh);
      this.datapointMeshesParent.add(dpMesh);

      // datapoint labels are html elements
      const labelContainerElem = document.querySelector('#labels');
      const elem = document.createElement('div');
      elem.setAttribute('id', datapoint.address);
      elem.setAttribute('class', 'datapoint-labels');
      elem.innerHTML = datapoint.label + '&nbsp;<span id="value_'
                          + datapoint.address + '">' + datapoint.value + '</span>&nbsp;' + datapoint.unit;
      labelContainerElem.appendChild(elem);
      console.log('constructed');
    }

    this.scene.add(this.datapointMeshesParent);


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


    this.loadData(0);


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
                  + (document.getElementById(this.schemeSelected.datapoints[ndx].address).offsetWidth / 2) + 20;

      const y = (tempVector.y * -.5 + .5) * this.canvas.clientHeight;

      // move the elem to that position
      document.getElementById(this.schemeSelected.datapoints[ndx].address).style.transform = `translate(-50%, -50%) translate(${x}px,${y}px)`;
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
    // '' +  Number(document.getElementById('value_' + dpAdress).innerHTML) + 1;
  }


  resize() {
    const width = window.innerWidth;
    const height = window.innerHeight;

    this.camera.aspect = width / height;
    this.camera.updateProjectionMatrix();

    this.renderer.setSize( width, height );
  }

  onSchemeChange(event: Event) {

    const newSchemeSelected =  this.schemes.find( scheme => scheme.path === '' + event);
    console.log('schemeSelected:' + newSchemeSelected.path );

    // 3d import of commercial heating
    if ( typeof newSchemeSelected.mesh === 'undefined' ) {
      this.loaderOBJ.load( '../assets/' + newSchemeSelected.path, ( obj ) => {
        obj.position.x = newSchemeSelected.position.x;
        obj.position.y = newSchemeSelected.position.y;
        obj.position.z = newSchemeSelected.position.z;
        obj.name = newSchemeSelected.path;
        const oldSchemeMesh = this.scene.getObjectByName( this.schemeSelected.path);
        this.scene.remove( oldSchemeMesh );

        this.scene.add( obj );

        newSchemeSelected.mesh = obj;

        this.schemeSelected = newSchemeSelected;

        this.placeLabels(this.schemes.findIndex( scheme => scheme.path === '' + event)) ;

      }, undefined, ( error ) => {
        alert(error);
        console.error( error );
      } );
    } else {

      const oldSchemeMesh = this.scene.getObjectByName( this.schemeSelected.path);
      this.scene.remove( oldSchemeMesh );

      this.scene.add( newSchemeSelected.mesh );

      this.schemeSelected = newSchemeSelected;
      this.placeLabels(this.schemes.findIndex( scheme => scheme.path === '' + event)) ;

    }
  }
}
