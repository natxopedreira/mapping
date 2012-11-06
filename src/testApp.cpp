#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofEnableSmoothing();
    ofEnableAlphaBlending();
    ofBackground(0, 0, 0);
    
    //  Load two MapaMoks
    //
    mapamoko.loadSettings("viewport.xml");
    mapamoko.init(0,0,ofGetScreenWidth(), ofGetScreenHeight());
    mapamoko.loadMesh("cubos_demo.dae", 800, 600);
    mapamoko.drawMode = DRAW_FACES;
    
    if (shader.load("shader")){
        //  One of the MapaMoks will have a cracy shader
        //
        mapamoko.linkShader(&shader);
    }
    
    //  Setup the canvas
    //
    canvas.allocate(ofGetScreenWidth(), ofGetScreenHeight());
    canvas.begin();
    ofClear(0);
    canvas.end();
}

//--------------------------------------------------------------
void testApp::update(){
        
    mapamoko.update();
    
    canvas.begin(false);
    ofClear(0);
    mapamoko.draw( );
    canvas.end();
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackgroundGradient(ofColor::gray, ofColor::black);
    
    ofSetColor(255);
    canvas.draw(0, 0);
    
    //mapamoko.draw( );
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if(key == 'f'){
        ofToggleFullscreen();
    } else if (key == 's'){
        mapamoko.saveSettings();
    } else if (key == 'S'){
        mapamoko.saveCalibration();
    }
}
//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
}