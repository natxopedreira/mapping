#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofEnableAlphaBlending();
    ofBackground(0, 0, 0);
    
    textWidth = 640;
    textHeight = 480;
    
    webCamera.initGrabber(textWidth, textHeight);
    
    mapamoko.loadMesh("cubos_demo.dae", textWidth, textHeight);
    mapamoko.viewport.x = 0;
    mapamoko.viewport.y = 0;
    mapamoko.viewport.width = 640;
    mapamoko.viewport.height = 480;
    mapamoko.drawMode = 3;
    mapamoko.useSmoothing = true;
    mapamoko.faceColor.set(80, 80, 80, 255);
   // mapamoko.loadShader("shader");
    
    mapamoko2.loadMesh("cubos_demo.dae", textWidth, textHeight);
    mapamoko2.viewport.x = 640;
    mapamoko2.viewport.y = 0;
    mapamoko2.viewport.width = 640;
    mapamoko2.viewport.height = 480;
    mapamoko2.drawMode = 3;
    mapamoko2.useSmoothing = true;
    mapamoko2.faceColor.set(80, 80, 80, 255);
   // mapamoko2.loadShader("shader");
}

//--------------------------------------------------------------
void testApp::update(){
    webCamera.update();
    mapamoko.update();
    mapamoko2.update();
}

//--------------------------------------------------------------
void testApp::draw(){
    mapamoko.draw(webCamera.getTextureReference());
    mapamoko2.draw(webCamera.getTextureReference());
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    mapamoko._keyPressed(key);
    mapamoko2._keyPressed(key);
}
//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    mapamoko._mousePressed(x, y, button);
    mapamoko2._mousePressed(x, y, button);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    mapamoko._mouseReleased(x, y, button);
    mapamoko2._mouseReleased(x, y, button);
}