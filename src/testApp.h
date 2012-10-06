#pragma once

#include "ofMain.h"

#include "ofxMapamok.h"
#include "ofxSmartViewPort.h"

//  Natxo,
//          Aqui implemente el canvas mode al cual entras con 'c'
//  pero q puedes editar con 'e'. Si quieres mover los view ports
//  de los MapaMokos lo haces con 'E'
//

//  Luego con SPACE cambias el modo de setup. Fijate que ahora esta
//  todo mejor explicado. Te va guiando como continuar
//  Asi mismo puedes cambiar el modo de dibujo con TAB
//

//  Todo ahora est‡ m‡s sencillo y encapsulado.
//  Quiz‡ demasiado. Av’same cualquier cosa.
//

//

class testApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    
    ofxMapaMok      mapamoko01;
    ofxMapaMok      mapamoko02;
    
    
    ofVideoGrabber      video;
    ofxSmartViewPort    videoViewPort;
    
    ofShader        shader;
    ofFbo           canvas;
    
    int             textWidth;
    int             textHeight;
    
    bool            bCanvasMode;
};
