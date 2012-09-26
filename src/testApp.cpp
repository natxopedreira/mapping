#include "testApp.h"

using namespace ofxCv;
using namespace cv;

//------------------------------------------------------------------------- SETUP
void testApp::setup() {
	ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
    ofEnableAlphaBlending();
	ofSetVerticalSync(true);
	
    //  Load Mesh
    //
	loadMesh("EscenarioEleccionesNacho_centrada.obj");
    
    //  Video ( temporal, just for getting a texture that change )
    //
    video.initGrabber(640,480);
    
    //  GUI
    //
    gui.setup("controles");
    gui.add(setupMode.setup("setupMode", true));
    gui.add(savecalibration.setup("savecalibration", false));
    gui.add(highlight.setup("highlight", false));
    gui.add(useSmoothing.setup("useSmoothing", false));
    
    gui.add(useLights.setup("useLights", false));
    gui.add(randomLighting.setup("randomLighting", false));
    gui.add(useShader.setup("useShader", false));
    gui.add(verEjes.setup("verEjes", true));
    
    gui.add(faces.setup("faces", false));
    gui.add(fullWireframe.setup("fullWireframe", false));
    gui.add(outlineWireframe.setup("outlineWireframe", false));
    gui.add(occludedWireframe.setup("occludedWireframe", true));
    
    faces.addListener(this,&testApp::facesPressed);
    fullWireframe.addListener(this,&testApp::fullWireframePressed);
    outlineWireframe.addListener(this,&testApp::outlineWireframePressed);
    occludedWireframe.addListener(this,&testApp::occludedWireframePressed);
    
    gui.add(scale.setup("scale", 1, .1, 25));
    gui.add(backGroundColor.setup("backgroundColor", 0, 0, 255));
    gui.add(highlightPosition.setup("highlightPosition",  0, 0, 1));
    gui.add(highlightOffset.setup("highlightOffset",  .1, 0, 1));
    gui.add(aov.setup("aov", 80, 50, 100));
    gui.add(lineWidth.setup("lineWidth", 2, 1, 8));
    gui.add(screenPointSize.setup("screenPointSize", 2, 1, 16));
    gui.add(selectedPointSize.setup("selectedPointSize", 8, 1, 16));
    gui.add(selectionRadius.setup("selectionRadius", 12, 1, 32));
    gui.add(lightX.setup("lightX", 200.0, -1000.0, 1000.0));
    gui.add(lightY.setup("lightY", 400.0, -1000.0, 1000.0));
    gui.add(lightZ.setup("lightZ", 800.0, -1000.0, 1000.0));
    
    gui.add(slowLerpRate.setup("slowLerpRate", .001, 0, .01));
    gui.add(fastLerpRate.setup("fastLerpRate", 1., 0, 1.));
    
    //  Flags
    //
    calibrationReady = false;
    
    validShader = true;
    selectionMode = true;
    hoverSelected = false;
    selectedVert = false;
    dragging = false;
    arrowing = false;
    
    fullWireframe = false;
    outlineWireframe = false;
    occludedWireframe = true;
    
    cvCALIB_FIX_ASPECT_RATIO = true;
    cvCALIB_FIX_K1 = true;
    cvCALIB_FIX_K2 = true;
    cvCALIB_FIX_K3 = true;
    cvCALIB_ZERO_TANGENT_DIST = true;
    cvCALIB_FIX_PRINCIPAL_POINT = false;
}

void testApp::loadMesh(string _daeModel){
    ofxAssimpModelLoader model;
    modelFile = _daeModel;
	model.loadModel(modelFile);
	objectMesh = model.getMesh(0);
	int n = objectMesh.getNumVertices();
    if ( n != objectMesh.getNumTexCoords() ){
        cout << "ERROR: not same amount of texCoords for all vertices" << endl;
    }
    
    for(int i = 0; i < n; i++){
        float x = objectMesh.getTexCoords()[i].x;
        float y = objectMesh.getTexCoords()[i].y;
        objectMesh.getTexCoords()[i] = ofVec2f( x*640, y*480 );
    }
    
	objectPoints.resize(n);
	imagePoints.resize(n);
	referencePoints.resize(n, false);
	for(int i = 0; i < n; i++) {
		objectPoints[i] = toCv(objectMesh.getVertex(i));
	}
    
    ofxXmlSettings XML;
    if (XML.loadFile(modelFile)){
        if (XML.pushTag("MAPAMOK")){
            
            int total = XML.getNumTags("point");
            for (int i = 0; i < total; i++) {
                XML.pushTag("point",i);
                if ( XML.getValue("calib", 1) ){
                    Point2f& cur = imagePoints[i];
                    referencePoints[i] = true;
                    cur.x = XML.getValue("x", 0.0f);
                    cur.y = XML.getValue("y", 0.0f);
                    cout << "Point " << i << " loaded at " << cur << endl;
                }
                XML.popTag();
            }
        }
        
        XML.popTag();
    }
}

void testApp::saveCalibration(string _folder) {
	
    //  Create a folder to store the calibration files
    //
	ofDirectory dir(_folder);
	dir.create();
	
	FileStorage fs(ofToDataPath(_folder + "calibration-advanced.yml"), FileStorage::WRITE);
    
	Mat cameraMatrix = intrinsics.getCameraMatrix();
	fs << "cameraMatrix" << cameraMatrix;
	double focalLength = intrinsics.getFocalLength();
	fs << "focalLength" << focalLength;
	Point2d fov = intrinsics.getFov();
	fs << "fov" << fov;
	Point2d principalPoint = intrinsics.getPrincipalPoint();
	fs << "principalPoint" << principalPoint;
	cv::Size imageSize = intrinsics.getImageSize();
	fs << "imageSize" << imageSize;
	fs << "translationVector" << tvec;
	fs << "rotationVector" << rvec;
	Mat rotationMatrix;
	Rodrigues(rvec, rotationMatrix);
	fs << "rotationMatrix" << rotationMatrix;
	double rotationAngleRadians = norm(rvec, NORM_L2);
	double rotationAngleDegrees = ofRadToDeg(rotationAngleRadians);
	Mat rotationAxis = rvec / rotationAngleRadians;
	fs << "rotationAngleRadians" << rotationAngleRadians;
	fs << "rotationAngleDegrees" << rotationAngleDegrees;
	fs << "rotationAxis" << rotationAxis;
	
	ofVec3f axis(rotationAxis.at<double>(0), rotationAxis.at<double>(1), rotationAxis.at<double>(2));
	ofVec3f euler = ofQuaternion(rotationAngleDegrees, axis).getEuler();
	Mat eulerMat = (Mat_<double>(3,1) << euler.x, euler.y, euler.z);
	fs << "euler" << eulerMat;
	ofFile basic("calibration-basic.txt", ofFile::WriteOnly);
	ofVec3f position( tvec.at<double>(1), tvec.at<double>(2));
	basic << "position (in world units):" << endl;
	basic << "\tx: " << ofToString(tvec.at<double>(0), 2) << endl;
	basic << "\ty: " << ofToString(tvec.at<double>(1), 2) << endl;
	basic << "\tz: " << ofToString(tvec.at<double>(2), 2) << endl;
	basic << "axis-angle rotation (in degrees):" << endl;
	basic << "\taxis x: " << ofToString(axis.x, 2) << endl;
	basic << "\taxis y: " << ofToString(axis.y, 2) << endl;
	basic << "\taxis z: " << ofToString(axis.z, 2) << endl;
	basic << "\tangle: " << ofToString(rotationAngleDegrees, 2) << endl;
	basic << "euler rotation (in degrees):" << endl;
	basic << "\tx: " << ofToString(euler.x, 2) << endl;
	basic << "\ty: " << ofToString(euler.y, 2) << endl;
	basic << "\tz: " << ofToString(euler.z, 2) << endl;
	basic << "fov (in degrees):" << endl;
	basic << "\thorizontal: " << ofToString(fov.x, 2) << endl;
	basic << "\tvertical: " << ofToString(fov.y, 2) << endl;
	basic << "principal point (in screen units):" << endl;
	basic << "\tx: " << ofToString(principalPoint.x, 2) << endl;
	basic << "\ty: " << ofToString(principalPoint.y, 2) << endl;
	basic << "image size (in pixels):" << endl;
	basic << "\tx: " << ofToString(principalPoint.x, 2) << endl;
	basic << "\ty: " << ofToString(principalPoint.y, 2) << endl;
	
	saveMat(Mat(objectPoints), _folder + "objectPoints.yml");
	saveMat(Mat(imagePoints), _folder + "imagePoints.yml");
    
    ofxXmlSettings XML;
    
    if (XML.loadFile(modelFile)){
        
        if (!XML.tagExists("MAPAMOK")){
            XML.addTag("MAPAMOK");
        }
        
        XML.pushTag("MAPAMOK");
        
        //  How much there are
        //
        int totalPoints = XML.getNumTags("point");
        
        //  Create the need ones
        //
        for(int i = totalPoints; i < referencePoints.size(); i++){
            XML.addTag("point");
        }
        
        //  Fill everything with data
        //
        for(int i = 0; i < referencePoints.size(); i++){
            XML.pushTag("point",i);
            
            XML.setValue("id", i);
            XML.setValue("calib", referencePoints[i]);
            XML.setValue("x", imagePoints[i].x );
            XML.setValue("y", imagePoints[i].y );
            XML.popTag();
        }
        
        XML.popTag();
        
        XML.saveFile(modelFile);
    }
    
    
}

//----------------------------------------------------------------------------- MAIN LOOP

void testApp::update() {
    ofSetWindowTitle("mapamok");
    
    video.update();
    
    if(randomLighting) {
        lightX = ofSignedNoise(ofGetElapsedTimef(), 1, 1) * 1000;
        lightY = ofSignedNoise(1, ofGetElapsedTimef(), 1) * 1000;
        lightZ = ofSignedNoise(1, 1, ofGetElapsedTimef()) * 1000;
	}
	light.setPosition(lightX,lightY, lightZ);
		
	if(selectionMode) {
		//cam.enableMouseInput();
	} else {
		updateRenderMode();
		//cam.disableMouseInput();
	}
}

void testApp::updateRenderMode() {
	// generate camera matrix given aov guess
	//float aov = getf("aov");
	Size2i imageSize(ofGetWidth(), ofGetHeight());
	float f = imageSize.width * ofDegToRad(aov); // i think this is wrong, but it's optimized out anyway
	Point2f c = Point2f(imageSize) * (1. / 2);
	Mat1d cameraMatrix = (Mat1d(3, 3) <<
                          f, 0, c.x,
                          0, f, c.y,
                          0, 0, 1);
    
	// generate flags
//#define getFlag(flag) (panel.getValueB((#flag)) ? flag : 0)
	int flags =
    CV_CALIB_USE_INTRINSIC_GUESS |
    cvCALIB_FIX_PRINCIPAL_POINT |
    cvCALIB_FIX_ASPECT_RATIO |
    cvCALIB_FIX_K1 |
    cvCALIB_FIX_K2 |
    cvCALIB_FIX_K3 |
    cvCALIB_ZERO_TANGENT_DIST;
    
	
	vector<Mat> rvecs, tvecs;
	Mat distCoeffs;
	vector<vector<Point3f> > referenceObjectPoints(1);
	vector<vector<Point2f> > referenceImagePoints(1);
	int n = referencePoints.size();
	for(int i = 0; i < n; i++) {
		if(referencePoints[i]) {
			referenceObjectPoints[0].push_back(objectPoints[i]);
			referenceImagePoints[0].push_back(imagePoints[i]);
		}
	}
	const static int minPoints = 6;
	if(referenceObjectPoints[0].size() >= minPoints) {
		calibrateCamera(referenceObjectPoints, referenceImagePoints, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs, flags);
		rvec = rvecs[0];
		tvec = tvecs[0];
		intrinsics.setup(cameraMatrix, imageSize);
		modelMatrix = makeMatrix(rvec, tvec);
		calibrationReady = true;
	} else {
		calibrationReady = false;
	}
}

void testApp::draw() {
	ofBackground(backGroundColor);
	if(savecalibration) {
		saveCalibration( "calibration-" + ofGetTimestampString() + "/" );
		savecalibration = false;
	}
	if(selectionMode) {
		drawSelectionMode();
	} else {
		drawRenderMode();
	}
	if(!validShader) {
		ofPushStyle();
		ofSetColor(magentaPrint);
		ofSetLineWidth(8);
		ofLine(0, 0, ofGetWidth(), ofGetHeight());
		ofLine(ofGetWidth(), 0, 0, ofGetHeight());
		string message = "Shader failed to compile.";
		ofVec2f center(ofGetWidth(), ofGetHeight());
		center /= 2;
		center.x -= message.size() * 8 / 2;
		center.y -= 8;
		drawHighlightString(message, center);
		ofPopStyle();
	}
    
    gui.draw();
}

void testApp::render() {
	ofPushStyle();
	ofSetLineWidth(lineWidth);
	if(useSmoothing) {
		ofEnableSmoothing();
	} else {
		ofDisableSmoothing();
	}

	if(useLights) {
		light.enable();
		ofEnableLighting();
		glShadeModel(GL_SMOOTH);
		glEnable(GL_NORMALIZE);
	}
	
	if(highlight) {
		objectMesh.clearColors();
		int n = objectMesh.getNumVertices();
		float highlightPosition = highlightPosition;
		float highlightOffset = highlightOffset;
		for(int i = 0; i < n; i++) {
			int lower = ofMap(highlightPosition - highlightOffset, 0, 1, 0, n);
			int upper = ofMap(highlightPosition + highlightOffset, 0, 1, 0, n);
			ofColor cur = (lower < i && i < upper) ? ofColor::white : ofColor::black;
			objectMesh.addColor(cur);
		}
	}
	
	ofSetColor(255);
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glEnable(GL_DEPTH_TEST);
	if(useShader) {
		ofFile fragFile("shader.frag"), vertFile("shader.vert");
		Poco::Timestamp fragTimestamp = fragFile.getPocoFile().getLastModified();
		Poco::Timestamp vertTimestamp = vertFile.getPocoFile().getLastModified();
		if(fragTimestamp != lastFragTimestamp || vertTimestamp != lastVertTimestamp) {
			validShader = shader.load("shader");
		}
		lastFragTimestamp = fragTimestamp;
		lastVertTimestamp = vertTimestamp;
		
		shader.begin();
		shader.setUniform1f("elapsedTime", ofGetElapsedTimef());
        shader.setUniformTexture("tex0", video.getTextureReference(), 0);
		shader.end();
	}
	ofColor transparentBlack(0, 0, 0, 0);
    
	switch(drawMode) {
		case 0: // faces
			if(useShader) shader.begin();
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
            video.getTextureReference().bind();
			objectMesh.drawFaces();
            video.getTextureReference().unbind();
			if(useShader) shader.end();
			break;
		case 1: // fullWireframe
			if(useShader) shader.begin();
			objectMesh.drawWireframe();
			if(useShader) shader.end();
			break;
		case 2: // outlineWireframe
			LineArt::draw(objectMesh, true, transparentBlack, useShader ? &shader : NULL);
			break;
		case 3: // occludedWireframe
			LineArt::draw(objectMesh, false, transparentBlack, useShader ? &shader : NULL);
			break;
	}
	glPopAttrib();
	if(useLights) {
		ofDisableLighting();
	}
	ofPopStyle();
}

void testApp::drawLabeledPoint(int label, ofVec2f position, ofColor color, ofColor bg, ofColor fg) {
	glPushAttrib(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	//glEnable(GL_DEPTH_TEST);
	ofVec2f tooltipOffset(5, -25);
	ofSetColor(color);
	float w = ofGetWidth();
	float h = ofGetHeight();
	ofSetLineWidth(1.5);
	ofLine(position - ofVec2f(w,0), position + ofVec2f(w,0));
	ofLine(position - ofVec2f(0,h), position + ofVec2f(0,h));
	ofCircle(position, selectedPointSize);
	drawHighlightString(ofToString(label), position + tooltipOffset, bg, fg);
	glPopAttrib();
}

void testApp::drawSelectionMode() {
	ofSetColor(255);
	cam.begin();
    
    if(verEjes){
        ofSetColor(255,100,100,180);
        drawGrid(100.0f, 10.0f, false, true, true,true);
    }
    ofSetColor(255,255,255,255);
	ofScale(scale, scale, scale);
	
	render();
    
	if(setupMode) {
		imageMesh = getProjectedMesh(objectMesh);
	}
	cam.end();
	
	if(setupMode) {
		// draw all points cyan small
		glPointSize(screenPointSize);
		glEnable(GL_POINT_SMOOTH);
		ofSetColor(cyanPrint);
		imageMesh.drawVertices();
        
		// draw all reference points cyan
		int n = referencePoints.size();
		for(int i = 0; i < n; i++) {
			if(referencePoints[i]) {
				drawLabeledPoint(i, imageMesh.getVertex(i), cyanPrint);
			}
		}
		
		// check to see if anything is selected
		// draw hover point magenta
		int choice;
		float distance;
		ofVec3f selected = getClosestPointOnMesh(imageMesh, mouseX, mouseY, &choice, &distance);
		if(!ofGetMousePressed() && distance < selectionRadius) {
			hoverChoice = choice;
			hoverSelected = true;
			drawLabeledPoint(choice, selected, magentaPrint);
		} else {
            hoverSelected = false;
		}
		
		// draw selected point yellow
		if(selectedVert) {
			int choice = selectionChoice;
			ofVec2f selected = imageMesh.getVertex(choice);
			drawLabeledPoint(choice, selected, yellowPrint, ofColor::white, ofColor::black);
		}
	}
}

void testApp::drawRenderMode() {
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	
	if(calibrationReady) {
		intrinsics.loadProjectionMatrix(10, 2000);
		applyMatrix(modelMatrix);
		render();
		if(setupMode) {
			imageMesh = getProjectedMesh(objectMesh);
		}
	}
	
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	
	if(setupMode) {
		// draw all reference points cyan
		int n = referencePoints.size();
		for(int i = 0; i < n; i++) {
			if(referencePoints[i]) {
				drawLabeledPoint(i, toOf(imagePoints[i]), cyanPrint);
			}
		}
		
		// move points that need to be dragged
		// draw selected yellow
		int choice = selectionChoice;
		if(selectedVert) {
			referencePoints[choice] = true;
			Point2f& cur = imagePoints[choice];
			if(cur == Point2f()) {
				if(calibrationReady) {
					cur = toCv(ofVec2f(imageMesh.getVertex(choice)));
				} else {
					cur = Point2f(mouseX, mouseY);
				}
			}
		}
		if(dragging) {
			Point2f& cur = imagePoints[choice];
			float rate = ofGetMousePressed(0) ? slowLerpRate : fastLerpRate;
			cur = Point2f(ofLerp(cur.x, mouseX, rate), ofLerp(cur.y, mouseY, rate));
			drawLabeledPoint(choice, toOf(cur), yellowPrint, ofColor::white, ofColor::black);
			ofSetColor(ofColor::black);
			ofRect(toOf(cur), 1, 1);
		} else if(arrowing) {
			Point2f& cur = imagePoints[choice];
			drawLabeledPoint(choice, toOf(cur), yellowPrint, ofColor::white, ofColor::black);
			ofSetColor(ofColor::black);
			ofRect(toOf(cur), 1, 1);
        } else {
			// check to see if anything is selected
			// draw hover magenta
			float distance;
			ofVec2f selected = toOf(getClosestPoint(imagePoints, mouseX, mouseY, &choice, &distance));
			if(!ofGetMousePressed() && referencePoints[choice] && distance < selectionRadius) {
				hoverChoice = choice;
                hoverSelected = true;
				drawLabeledPoint(choice, selected, magentaPrint);
			} else {
				hoverSelected = false;
			}
		}
	}
}
//------------------------------------------------------------ GUI
void testApp::facesPressed(bool & pressed){ 
    if (pressed){
        drawMode = 0;
        fullWireframe.value = false;
        outlineWireframe.value = false;
        occludedWireframe.value = false; 
    }
}
void testApp::fullWireframePressed(bool & pressed){
    if (pressed){
        drawMode = 1;
        faces.value = false;
        outlineWireframe.value = false;
        occludedWireframe.value = false; 
    }
}
void testApp::outlineWireframePressed(bool & pressed){
    if (pressed){
        drawMode = 2;
        fullWireframe.value = false;
        faces.value = false;
        occludedWireframe.value = false; 
    }
}
void testApp::occludedWireframePressed(bool & pressed){
    if (pressed){
        drawMode = 3;
        fullWireframe.value = false;
        outlineWireframe.value = false;
        faces.value = false; 
    }
}
//------------------------------------------------------------ EVENT

void testApp::keyPressed(int key) {
	if(key == OF_KEY_LEFT || key == OF_KEY_UP || key == OF_KEY_RIGHT|| key == OF_KEY_DOWN){
		int choice = selectionChoice;
        arrowing = true;
		if(choice > 0){
			Point2f& cur = imagePoints[choice];
			switch(key) {
				case OF_KEY_LEFT: cur.x -= 1; break;
				case OF_KEY_RIGHT: cur.x += 1; break;
				case OF_KEY_UP: cur.y -= 1; break;
				case OF_KEY_DOWN: cur.y += 1; break;
			}
		}
	} else {
		arrowing = false;
	}
	if(key == OF_KEY_BACKSPACE) { // delete selected
		if(selectedVert) {
			selectedVert = false;
			int choice = selectionChoice;
			referencePoints[choice] = false;
			imagePoints[choice] = Point2f();
		}
	}
	if(key == '\n') { // deselect
		selectedVert = false;
	}
	if(key == ' ') { // toggle render/select mode
        selectionMode = !selectionMode;
	}
    if(key == 'f'){
        ofToggleFullscreen();
    }
    if(key == 's') {
		gui.saveToFile("settings.xml");
	}
	if(key == 'l') {
		gui.loadFromFile("settings.xml");
	}

}

void testApp::mousePressed(int x, int y, int button) {
	selectedVert = hoverSelected;
	selectionChoice = hoverChoice;
	if(selectedVert) {
        dragging = true;
	}
}

void testApp::mouseReleased(int x, int y, int button) {
	dragging = false;
}


void testApp::drawGrid(float scale, float ticks, bool labels, bool x, bool y, bool z) {
	
	ofColor c = ofGetStyle().color;
	
	ofPushStyle();
	
	if (x) {
		c.setHue(0.0f);
		ofSetColor(c);
		drawGridPlane(scale, ticks, labels);
	}
	if (y) {
		c.setHue(255.0f / 3.0f);
		ofSetColor(c);
		ofPushMatrix();
		ofRotate(90, 0, 0, 1);
		drawGridPlane(scale, ticks, labels);
		ofPopMatrix();
	}
	if (z) {
		c.setHue(255.0f * 2.0f / 3.0f);
		ofSetColor(c);
		ofPushMatrix();
		ofRotate(90, 0, 1, 0);
		drawGridPlane(scale, ticks, labels);
		ofPopMatrix();
	}
	
	ofPopStyle();
}


//-------------------------------------------------------------- GRID 
void testApp::drawGridPlane(float scale, float ticks, bool labels) {
    
	float minor = scale / ticks;
	float major =  minor * 2.0f;
	
	ofPushStyle();
	for (int iDimension=0; iDimension<2; iDimension++)
	{
		for (float yz=-scale; yz<=scale; yz+= minor)
		{
			//major major
			if (fabs(yz) == scale || yz == 0)
				ofSetLineWidth(1);
			
			//major
			else if (yz / major == floor(yz / major) )
				ofSetLineWidth(1);
			
			//minor
			else
				ofSetLineWidth(1);
			if (iDimension==0)
				ofLine(0, yz, -scale, 0, yz, scale);
			else
				ofLine(0, -scale, yz, 0, scale, yz);
		}
	}
	ofPopStyle();
	
	if (labels) {
		//draw numbers on axes
		ofPushStyle();
		ofSetColor(255, 255, 255);
		
		ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
		for (float yz = -scale; yz<=scale; yz+=minor)
		{
			ofDrawBitmapString(ofToString(yz, 0), 0, yz, 0);
			ofDrawBitmapString(ofToString(yz, 0), 0, 0, yz);		
		}
		ofPopStyle();
	}
    
}