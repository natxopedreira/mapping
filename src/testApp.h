#pragma once

#include "ofMain.h"

#include "ofxMapamok.h"
#include "ofxSmartViewPort.h"

class testApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    
    ofxMapamok      mapamoko01;
    ofxMapamok      mapamoko02;
    
    ofVideoGrabber  webCamera;
    
    int             textWidth,
                    textHeight;
};
