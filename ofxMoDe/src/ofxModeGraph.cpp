#include "ofxMoDeGraph.h"

ofxMoDeGraph::ofxMoDeGraph()
{
    ofAddListener(ofEvents().mousePressed, this, &ofxMoDeGraph::mousePressed);
    ofAddListener(ofEvents().mouseDragged, this, &ofxMoDeGraph::mouseDragged);
    ofAddListener(ofEvents().mouseReleased, this, &ofxMoDeGraph::mouseReleased);
    
    hue = 0.0;
    scale = 10.0;
    text = "graph";

	threshold = 0.0;
    
    screenPos = ofVec2f(0, 0);
    size = ofVec2f(400, 300);
    
    mode = MODE_1D;
    for (size_t i = 0; i < N_POINTS; i++)
    {
        oneTimeSeries.push_back(0.0);
        threeTimeSeries.push_back(ofVec3f(0.0, 0.0, 0.0));
    }
    prevMouseMove = ofVec2f(0.0);
    prevMouseResize = ofVec2f(0.0);
    prevMouseScale = ofVec2f(0.0);
}

ofxMoDeGraph::ofxMoDeGraph(float x, float y, float w, float h, float colorHue, string graphText)
{
    screenPos = ofVec2f(x, y);
    size = ofVec2f(w, h);
    hue = colorHue;
    
    ofAddListener(ofEvents().mousePressed, this, &ofxMoDeGraph::mousePressed);
    ofAddListener(ofEvents().mouseDragged, this, &ofxMoDeGraph::mouseDragged);
    ofAddListener(ofEvents().mouseReleased, this, &ofxMoDeGraph::mouseReleased);
    
    scale = 10.0;
    text = graphText;
	threshold = 0.0;
    
    mode = MODE_1D;
    for (size_t i = 0; i < N_POINTS; i++)
    {
        oneTimeSeries.push_back(0.0);
        threeTimeSeries.push_back(ofVec3f(0.0, 0.0, 0.0));
    }
    prevMouseMove = ofVec2f(0.0);
    prevMouseResize = ofVec2f(0.0);
    prevMouseScale = ofVec2f(0.0);
}

void ofxMoDeGraph::setText(string graphText){
    text = graphText;
}

void ofxMoDeGraph::setPos(float x, float y)
{
    screenPos = ofVec2f(x, y);
}

void ofxMoDeGraph::setSize(float w, float h)
{
    size = ofVec2f(w, h);
}

void ofxMoDeGraph::setHue(float colorHue)
{
    hue = colorHue;
}

float ofxMoDeGraph::getHue()
{
    return hue;
}

void ofxMoDeGraph::addValue(float value)
{
    mode = MODE_1D;
    if (oneTimeSeries.size() <= N_POINTS) {
        oneTimeSeries.insert(oneTimeSeries.begin(), value);
    }
    if (oneTimeSeries.size() > N_POINTS) {
        oneTimeSeries.pop_back();
    }
    
    for (auto &event : events)
        event += 1;
}


void ofxMoDeGraph::addValue(ofVec3f value)
{
    mode = MODE_3D;
    if (threeTimeSeries.size() <= N_POINTS) {
        threeTimeSeries.insert(threeTimeSeries.begin(), value);
    }
    if (threeTimeSeries.size() > N_POINTS) {
        threeTimeSeries.pop_back();
    }
    
    for (auto &event : events)
        event += 1;
}

void ofxMoDeGraph::setThreshold(float thresholdValue)
{
	threshold = thresholdValue;
}

void ofxMoDeGraph::newEvent()
{
    if (events.size() <= N_EVENTS) {
        events.insert(events.begin(), 0);
    }
    if (events.size() > N_EVENTS) {
        events.pop_back();
    }
}

void ofxMoDeGraph::draw()
{
    ofPushMatrix();
    ofPushStyle();
    ofTranslate(screenPos);
    ofScale(size.x, size.y);
    
    ofSetColor(200, 200);
    ofDrawRectangle(0, 0, 1, 1);
    //Change size square
    ofSetColor(0, 200);
    ofDrawRectangle(0.97, 1 - 0.03 * size.x / size.y, 0.03, 0.03 * size.x / size.y);
    //Change descriptor square
    ofSetColor(122, 20, 32, 200);
    ofDrawRectangle(0, 1 - 0.03 * size.x / size.y, 0.03, 0.03 * size.x / size.y);
    //Change joint square
    ofSetColor(204, 102, 114, 200);
    ofDrawRectangle(0.03, 1 - 0.03 * size.x / size.y, 0.03, 0.03 * size.x / size.y);
    
    //descriptor name
    ofDrawBitmapString(text, 0.07, 1 - 0.03 * size.x / size.y);
    
    if (mode == MODE_1D)
    {
        ofPolyline line;
        for (int i = 0; i < oneTimeSeries.size(); i++) {
            float x = 0.95 - (float)i / N_POINTS * 0.9;
            float y = 0.5 - scale * oneTimeSeries[i];
            line.addVertex(x, y);
        }
        ofSetColor(ofColor::fromHsb(hue, 255, 255));
        line.draw();

		stringstream stream;
		stream << fixed << setprecision(3) << oneTimeSeries[0];
		ofDrawBitmapString(stream.str(), 0.80, 0.15);
    }
    else if (mode == MODE_3D) {
        ofPolyline line1, line2, line3;
        for (int i = 0; i < threeTimeSeries.size(); i++) {
            float x = 0.95 - (float)i / N_POINTS * 0.9;
            ofVec3f y = 0.5 - scale * threeTimeSeries[i];
            line1.addVertex(x, y.x);
            line2.addVertex(x, y.y);
            line3.addVertex(x, y.z);
        }
        ofSetColor(ofColor::fromHsb(hue, 255, 50));
        line1.draw();
        ofSetColor(ofColor::fromHsb(hue, 255, 150));
        line2.draw();
        ofSetColor(ofColor::fromHsb(hue, 255, 255));
        line3.draw();
    }

	if (threshold != 0.0)
	{
		ofSetColor(0);
		ofDrawLine(0.05, 0.5 - scale * threshold, 0.95, 0.5 - scale * threshold);
	}
    
    ofSetColor(ofColor::fromHsb(hue, 255, 10));
    for (auto event : events) {
        if (event < N_POINTS)
            ofDrawLine(0.95 - (float)event / N_POINTS * 0.9, 0, 0.95 - (float)event / N_POINTS * 0.9, 1);
    }
    
    ofPopStyle();
    ofPopMatrix();
    
}

void ofxMoDeGraph::mousePressed(ofMouseEventArgs& event) {
    //bottom-right corner (change size)
    if (event.x > screenPos.x + 0.97 * size.x && event.x < screenPos.x + size.x &&
        event.y > screenPos.y + size.y - 0.03 * size.x && event.y < screenPos.y + size.y)
        prevMouseResize = event;
    
    //bottom-left corner (change descriptor)
    else if (scale >= 0.0 &&
             event.x > screenPos.x && event.x < screenPos.x + 0.03 * size.x &&
             event.y > screenPos.y + size.y - 0.03 * size.x && event.y < screenPos.y + size.y)
        prevMouseScale = event;
    
    //everywhere else (move)
    else if (event.x > screenPos.x && event.x < screenPos.x + size.x &&
             event.y > screenPos.y && event.y < screenPos.y + size.y)
        prevMouseMove = event;
}

void ofxMoDeGraph::mouseReleased(ofMouseEventArgs& event) {
    prevMouseMove = ofVec2f(0.0);
    prevMouseResize = ofVec2f(0.0);
    prevMouseScale = ofVec2f(0.0);
}


void ofxMoDeGraph::mouseDragged(ofMouseEventArgs & event)
{
    if (prevMouseMove != ofVec2f(0.0)) {
        screenPos += event - prevMouseMove;
        prevMouseMove = event;
    } else if (prevMouseResize != ofVec2f(0.0)) {
        size += event - prevMouseResize;
        prevMouseResize = event;
    } else if (prevMouseScale != ofVec2f(0.0)) {
        scale += (prevMouseScale.y - event.y) / 100.0;
        if (scale <= 0.0) scale = 0.0;
        prevMouseScale = event;
    }
    
}
