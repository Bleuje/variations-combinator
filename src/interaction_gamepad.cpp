#include "ofApp.h"

void ofApp::axisChanged(ofxGamepadAxisEvent& e)
{
	//cout << "AXIS " << e.axis << " VALUE " << ofToString(e.value) << endl;
    int axisType = e.axis;
    float value = e.value;
    if(axisType==6 && e.value>0.5)
    {
        actionChangeFunctionAtCursor(1);
    }
    if(axisType==6 && e.value<-0.5)
    {
        actionChangeFunctionAtCursor(-1);
    }
    if(axisType==7 && e.value>0.5)
    {
        actionMoveCursor(-1);
    }
    if(axisType==7 && e.value<-0.5)
    {
        actionMoveCursor(1);
    }
    if(axisType==0 || axisType==1)
    {
        if(axisType==0) curTranslationAxis1 = 0;
        if(axisType==1) curTranslationAxis2 = 0;
        if(abs(e.value)>0.09)
        {
            if(axisType==0) curTranslationAxis1 = e.value;
            if(axisType==1) curTranslationAxis2 = e.value;
            renderNewOne = true;
            doPrintState = false;
        }
    }
    if(axisType==3 || axisType==4)
    {
        if(axisType==3) curRotAxis1 = 0;
        if(axisType==4) curRotAxis2 = 0;
        latest3DJSMoveTime = time;
        if(abs(e.value)>0.09)
        {
            latest3DJSMoveTime = time;
            if(axisType==3) curRotAxis1 = e.value;
            if(axisType==4) curRotAxis2 = e.value;
        }
    }
    if(axisType==4 && threeD==0)
    {
        curScaleAxis1 = 0;
        if(abs(e.value)>0.09)
        {
            curScaleAxis1 = -e.value;
            renderNewOne = true;
            doPrintState = false;
        }
    }
    if(axisType==3 && threeD==0)
    {
        curRotationAxis1 = 0;
        if(abs(e.value)>0.09)
        {
            curRotationAxis1 = e.value;
            renderNewOne = true;
            doPrintState = false;
        }
    }
    if(axisType==2)
    {
        curL2 = e.value;
    }
    if(axisType==5)
    {
        curR2 = e.value;
    }
}

void ofApp::buttonPressed(ofxGamepadButtonEvent& e)
{
	//cout << "BUTTON " << e.button << " PRESSED" << endl;
    int buttonId = e.button;
    if(buttonId == 0)
    {
        actionNewParameters();
    }
    if(buttonId == 1)
    {
        actionRandomizeParametersOfAllVariations();
    }
    if(buttonId == 2)
    {
        actionRandomizeParametersOfSingleVariation();
    }
    if(buttonId == 3)
    {
        if(threeD==0) actionSetBounder();
        else actionChangeProjection();
    }
    if(buttonId == 4)
    {
        actionAddFunctionAtCursor();
    }
    if(buttonId == 5)
    {
        actionRemoveFunctionAtCursor();
    }
    if(buttonId == 6)
    {
        actionChange2D3D();
    }
    if(buttonId == 7)
    {
        actionChangeColors();
    }
    if(buttonId == 10)
    {
        actionResetScales();
        actionResetRotations();
    }
    if(buttonId == 9)
    {
        actionResetTranslations();
    }
}

void ofApp::buttonReleased(ofxGamepadButtonEvent& e)
{
	//cout << "BUTTON " << e.button << " RELEASED" << endl;
}