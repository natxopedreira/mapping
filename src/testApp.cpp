#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofEnableSmoothing();
    ofEnableAlphaBlending();
    ofBackground(0, 0, 0);
    
    textWidth = 640;
    textHeight = 480;
    
    webCamera.initGrabber(textWidth, textHeight);
    
    mapamoko01.loadSettings("viewport01.xml");
    mapamoko01.loadMesh("cubos_demo.dae", textWidth, textHeight);
    mapamoko01.drawMode = DRAW_FACES;
    
    mapamoko02.loadSettings("viewport02.xml");
    mapamoko02.loadMesh("cubos_demo.dae", textWidth, textHeight);
    
    if (shader.load("shader")){
        mapamoko02.linkShader(&shader);
    }
}

//--------------------------------------------------------------
void testApp::update(){
    webCamera.update();
    
    mapamoko01.update();
    mapamoko02.update();
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void testApp::draw(){
    mapamoko01.draw( &webCamera.getTextureReference() );
    mapamoko02.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if(key == 'f'){
        ofToggleFullscreen();
    }
}
//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
}