#include "ofApp.h"

#define NCOLORS 3

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0, 0, 0);
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    
    cam.initGrabber(320,240);
    vibe.setup(NCOLORS,20,4,17,2,16);
}

//--------------------------------------------------------------
void ofApp::update(){
    cam.update();
    
    if(cam.isFrameNew()){
        cv::Mat cvFrame = ofxCv::toCv(cam);
        vibe.update(cvFrame);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    cam.draw(0, 0);
    vibe.drawCurrentFrame(320, 0);
    vibe.drawForeground(640,0);
    vibe.drawMask(0, 240);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
