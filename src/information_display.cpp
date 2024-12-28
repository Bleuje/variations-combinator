#include "ofApp.h"

// show info on screen
void ofApp::showState()
{

    float u = 50.0/30.0*1080/CANVAS_HEIGHT;

    float dx = CANVAS_HEIGHT+35*u;
    float dy = 150*u;

    float infoParameter = ofMap(curL2,-0.8,0.85,0,1,true);

    ofPushMatrix();

    ofTranslate(dx,dy);

    float yTranslation = 90*40.0/30.0*1080/CANVAS_HEIGHT;

    int frameNum = ofGetFrameNum();

    ofPushMatrix();

    ofTranslate(1100*pow(infoParameter,2.8)*u,0);

    for(int i=0;i<curNumberOfSuccessiveVariations;i++)
    {
        ofPushMatrix();

        int reversedIndex = (curNumberOfSuccessiveVariations - i - 1);

        float col = (colorMode!=0 && colorMode!=3 ? 255 : 0);

        ofTranslate(0,yTranslation*reversedIndex);

        std::string cursor = (indexOfChanges==i ? "  <" : "");

        std::string functionString = std::to_string(reversedIndex+1) + " : " + getName(variationsParameters[i].variationType) + cursor;

        float change0 = ofClamp(4.3*(time-randomizationSignal[i]),0,1);
        float change1 = ofClamp(4.3*(time-randomizationSignal[max(i-1,0)]),0,1);

        ofPushMatrix();
        ofTranslate(18.0*sin(PI*change0),0);

        if(indexOfChanges==i)
        {
            //float strLength = functionString.size();
            
            ofPushMatrix();
            ofSetColor(col);
            ofTranslate(-10*u,-28*u);
            ofDrawRectangle(0,0,20*u+myFont.stringWidth(functionString),37*u);
            ofPopMatrix();

            ofSetColor(255-col);
            ofPushMatrix();
            //ofScale(1+0.075*(0.5+0.5*cos(0.5*frameNum))*15/strLength,1);
            myFont.drawString(functionString,0,0);
            ofPopMatrix();
        }
        else
        {
            ofSetColor(col);
            myFont.drawString(functionString,0,0);
        }

        ofPopMatrix();

        ofTranslate(18.0*sin(PI*min(change0,change1)),0);

        if(i>0 && (threeD==0||(reversedIndex!=curNumberOfSuccessiveVariations/2-1)))
        {
            float troffset = 7*cos(TWO_PI*ofClamp(2.3*fmod(123+0.027*frameNum - 0.15*i,2.3),0,1));

            ofPushMatrix();
            ofTranslate(1,35 + troffset);
            ofScale(1,0.6);
            ofSetColor(col);
            myFont.drawString("^",0,0);
            ofTranslate(5.5,15);
            myFont.drawString("|",0,0);
            ofPopMatrix();
        }

        ofPopMatrix();
    }

    ofPopMatrix();

    ofTranslate(0,0);

    ofScale(0.8);

    float infoAlpha = 255*pow(infoParameter,2.5);

    float alpha0 = 255-infoAlpha;

    float col = (colorMode!=0 && colorMode!=3 ? 255 : 0);

    if(numberOfGamepads>0)
    {
        ofPushMatrix();
        ofTranslate(500*u,-50-300*u*infoParameter);
        ofSetColor(col,alpha0*0.75);
        ofScale(0.9);
        myFont.drawString("L2 to show info and controls",0,0);
        ofPopMatrix();
    }

    if(numberOfGamepads>0 && infoParameter>0.0001)
    {
        ofPushMatrix();

        float einfoP = 1-pow(1-infoParameter,2.5);

        ofTranslate(0,100*u*(1-einfoP));

        yTranslation *= 0.8;

        std::string howItWorksString = "HOW IT WORKS :";
        ofPushMatrix();
        ofSetColor(col,infoAlpha);
        ofTranslate(-10*u,-30*u);
        ofDrawRectangle(0,0,20*u+myFont.stringWidth(howItWorksString),38*u);
        ofPopMatrix();
        ofSetColor(255-col,infoAlpha);
        myFont.drawString(howItWorksString,0,0);
        ofSetColor(col,infoAlpha);

        ofTranslate(0,yTranslation);
        myFont.drawString("A square of particles goes through a sequence",0,0);
        ofTranslate(0,yTranslation);
        myFont.drawString("of transformations/functions (following the arrows).",0,0);
        ofTranslate(0,yTranslation);
        myFont.drawString("Most of the functions have random parameters.",0,0);
        ofTranslate(0,yTranslation);
        ofTranslate(0,yTranslation);

        std::string controlsString = "CONTROLS :";
        ofPushMatrix();
        ofSetColor(col,infoAlpha);
        ofTranslate(-10*u,-30*u);
        ofDrawRectangle(0,0,20*u+myFont.stringWidth(controlsString),38*u);
        ofPopMatrix();
        ofSetColor(255-col,infoAlpha);
        myFont.drawString(controlsString,0,0);
        ofSetColor(col,infoAlpha);

        ofTranslate(0,yTranslation);
        myFont.drawString("                         : select and change functions",0,0);
        myFontBold.drawString("Pad",0,0);

        drawPad(127,-12,255-col,infoAlpha);
        ofSetColor(col,infoAlpha);

        ofTranslate(0,yTranslation);
        myFont.drawString("                         : random new set of functions",0,0);
        myFontBold.drawString("A",0,0);
        ofTranslate(0,yTranslation);
        myFont.drawString("                         : randomize parameters of all functions",0,0);
        myFontBold.drawString("B",0,0);
        ofTranslate(0,yTranslation);
        myFont.drawString("                         : randomize parameters of selected function",0,0);
        myFontBold.drawString("X",0,0);
        ofTranslate(0,yTranslation);
        myFont.drawString("                         : in 2D, keep particles in screen or not",0,0);
        myFontBold.drawString("Y",0,0);
        ofTranslate(0,yTranslation);
        myFont.drawString("                         : in 3D, change 3D projection",0,0);
        ofTranslate(0,yTranslation);
        myFont.drawString("                         : add/remove function",0,0);
        myFontBold.drawString("L1/R1",0,0);
        ofTranslate(0,yTranslation);
        myFont.drawString("                         : translation on function",0,0);
        myFontBold.drawString("Left joystick",0,0);
        ofTranslate(0,yTranslation);
        myFont.drawString("                         : scale/rotation on function (view rotation in 3D)",0,0);
        myFontBold.drawString("Right joystick",0,0);
        ofTranslate(0,yTranslation);
        myFont.drawString("                         : color modes",0,0);
        myFontBold.drawString("Middle right",0,0);
        ofTranslate(0,yTranslation);
        ofPushMatrix();
        ofScale(1.22);
        ofTranslate(0,3);
        myFont.drawString("                     : 2D/3D modes",0,0);
        myFontBold.drawString("Middle left",0,0);
        ofPopMatrix();
        ofTranslate(0,yTranslation);
        ofTranslate(0,yTranslation);

        std::string aboutString = "ABOUT :";
        ofPushMatrix();
        ofSetColor(col,infoAlpha);
        ofTranslate(-10*u,-30*u);
        ofDrawRectangle(0,0,20*u+myFont.stringWidth(aboutString),38*u);
        ofPopMatrix();
        ofSetColor(255-col,infoAlpha);
        myFont.drawString(aboutString,0,0);
        ofSetColor(col,infoAlpha);

        ofTranslate(0,yTranslation);
        myFont.drawString("By Etienne Jacob (bleuje)",0,0);
        ofTranslate(0,yTranslation);
        myFont.drawString("Inspired by work from GenerateMe (Twitter: @generateme_blog)",0,0);

        ofPopMatrix();
    }

    ofPopMatrix();
}

// draw pad with arrows when explaining controls
void ofApp::drawPad(float x, float y, float col, float alpha)
{
    ofPushMatrix();
    ofTranslate(x,y);
    
    for(int i=0;i<4;i++)
    {
        ofPushMatrix();
        ofRotateRad(HALF_PI*i);
        float rectangleHeight = 25;
        ofSetColor(255-col,255*pow(alpha/255.0,2.0));
        ofDrawRectangle(0,-rectangleHeight/2,30,rectangleHeight);
        ofSetColor(col,255*pow(alpha/255.0,2.0));
        ofBeginShape();
        ofVertex(16,rectangleHeight/3);
        ofVertex(25,0);
        ofVertex(16,-rectangleHeight/3);
        ofEndShape(true);
        ofPopMatrix();
    }
    ofPopMatrix();
}