#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofEnableAlphaBlending();
    ofBackground(0, 0, 0);
    
    textWidth = 640;
    textHeight = 480;
    
    webCamera.initGrabber(textWidth, textHeight);
    
    mapamoko01.loadSettings("viewport01.xml");
    mapamoko01.loadMesh("cubos_demo.dae", textWidth, textHeight);
    mapamoko01.drawMode = 3;
    mapamoko01.useSmoothing = true;
    mapamoko01.faceColor.set(80, 80, 80, 255);
    
    mapamoko02.loadSettings("viewport02.xml");
    mapamoko02.loadMesh("cubos_demo.dae", textWidth, textHeight);
    mapamoko02.drawMode = 3;
    mapamoko02.useSmoothing = true;
    mapamoko02.faceColor.set(80, 80, 80, 255);
}

//--------------------------------------------------------------
void testApp::update(){
    webCamera.update();
    
    mapamoko01.update();
    mapamoko02.update();
}

//--------------------------------------------------------------
void testApp::draw(){
    mapamoko01.draw(webCamera.getTextureReference());
    mapamoko02.draw(webCamera.getTextureReference());
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