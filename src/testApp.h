#pragma once

#include "ofMain.h"

#include "ofxAssimpModelLoader.h"

#include "ofxCv.h"

#include "ofxProCamToolkit.h"
#include "LineArt.h"

#include "ofxGui.h"

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
	
    void facesPressed(bool & pressed);
    void fullWireframePressed(bool & pressed);
    void outlineWireframePressed(bool & pressed);
    void occludedWireframePressed(bool & pressed);
    //  GUI
    //  --------------------------------
	void    setupControlPanel();
	
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
    
    // natxo 

    bool cvCALIB_FIX_ASPECT_RATIO, cvCALIB_FIX_K1, cvCALIB_FIX_K2, cvCALIB_FIX_K3, cvCALIB_ZERO_TANGENT_DIST, cvCALIB_FIX_PRINCIPAL_POINT;
    int drawMode;
    
    ofxPanel gui;
    ofxToggle setupMode, savecalibration, highlight, useSmoothing, randomLighting, useLights, faces, fullWireframe, outlineWireframe, occludedWireframe, useShader;
    ofxFloatSlider scale,  highlightPosition, highlightOffset, lightX, lightY, lightZ, slowLerpRate, fastLerpRate;
    ofxIntSlider lineWidth, backGroundColor, screenPointSize, selectedPointSize, selectionRadius, aov;
    
    int hoverChoice, selectionChoice;
    bool validShader, selectionMode, hoverSelected, selectedVert, dragging, arrowing;
};
