#include "ofApp.h"

void ofApp::setup() {
	ofSetVerticalSync(true);	
	ofSetFrameRate(60);
	fft.setup(16384);
}

void ofApp::update() {
	fft.update();
}

void ofApp::draw() {
	ofBackground(0, 0, 0);
	
	ofPushMatrix();
	ofTranslate(16, 16);
	ofSetColor(255);
	ofDrawBitmapString("Frequency Domain", 0, 0);
	plot(fft.getBins(), 128);
	ofPopMatrix();
	
	string msg = ofToString((int) ofGetFrameRate()) + " fps";
	ofDrawBitmapString(msg, ofGetWidth() - 80, ofGetHeight() - 20);
}

void ofApp::plot(vector<float>& buffer, float scale) {
    ofNoFill();
    int n = buffer.size();
    float plotWidth = ofGetWidth() - 32; // leave some margin (same as your translate)
    ofDrawRectangle(0, 0, plotWidth, scale);
    ofPushMatrix();
    ofTranslate(0, scale);
    ofScale(plotWidth / (float)n, -scale);
    ofBeginShape();
    for (int i = 0; i < n; i++) {
        ofVertex(i, buffer[i]);
    }
    ofEndShape();
    ofPopMatrix();
}
