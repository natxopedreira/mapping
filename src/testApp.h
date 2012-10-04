#pragma once

#include "ofMain.h"
#include "ofxMapamok.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
    
    
    ofxMapamok mapamoko, mapamoko2;
    ofVideoGrabber webCamera;
    int textWidth, textHeight;
		
};
