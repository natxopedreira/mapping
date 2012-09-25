#pragma once

#include "ofMain.h"

#include "ofxAssimpModelLoader.h"

#include "ofxCv.h"

#include "ofxProCamToolkit.h"
#include "ofxAutoControlPanel.h"
#include "LineArt.h"

class testApp : public ofBaseApp {
public:
	
	void setup();
    
    void loadMesh(string _daeModel);
    void saveCalibration(string _xmlfile);
    
	void update();
	void draw();
    
	void keyPressed(int key);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	
    //  GUI
    //  --------------------------------
	void    setupControlPanel();
    void    setb(string name, bool value);
	void    seti(string name, int value);
	void    setf(string name, float value);
	bool    getb(string name);
	int     geti(string name);
	float   getf(string name);
	ofxAutoControlPanel panel;
	
	void drawLabeledPoint(int label, ofVec2f position, ofColor color, ofColor bg = ofColor::black, ofColor fg = ofColor::white);
	void updateRenderMode();
	void drawSelectionMode();
	void drawRenderMode();
	void render();
	
    ofVideoGrabber          video;
	ofEasyCam               cam;

	ofVboMesh               objectMesh;
	ofMesh                  imageMesh;
	ofLight                 light;
	
	vector<cv::Point3f>     objectPoints;
	vector<cv::Point2f>     imagePoints;
	vector<bool> referencePoints;
	
	cv::Mat rvec, tvec;
	ofMatrix4x4 modelMatrix;
	ofxCv::Intrinsics intrinsics;
	bool calibrationReady;
	
    string  modelFile;
	Poco::Timestamp lastFragTimestamp, lastVertTimestamp;
	ofShader shader;
};
