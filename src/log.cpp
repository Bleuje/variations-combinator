#include "ofApp.h"

// print info in console
void ofApp::printState()
{
    if(DISABLE_STATE_PRINT) return;

    std::cout << "----------" << std::endl;

    if(threeD) std::cout << "3D mode." << std::endl;

    if(doSinusoid) std::cout << "Constraining mode activated." << std::endl;

    if(operationsMode==0) std::cout << "Full composition";
    if(operationsMode==1) std::cout << "Sum with last 2 functions";
    if(operationsMode==2) std::cout << "Sum with last 3 functions";
    if(operationsMode==3) std::cout << "One fixed point iteration";

    std::cout << std::endl;

    if(distortionMode==1) std::cout << "distortion at center" << std::endl;
    if(distortionMode==2) std::cout << "lerp from input mode" << std::endl;

    std::cout << "Large resolution : " << CANVAS_WIDTH << " x " << CANVAS_HEIGHT << " ; Dots amount : " << nx << " x " << ny << std::endl;


    for(int i=0;i<curNumberOfSuccessiveVariations;i++)
    {
        std::cout << "--> " << getName(variationsParameters[i].variationType);
        if(indexOfChanges==i) std::cout << " <--";
        std::cout << std::endl;
    }
}

// save list of functions in text file
void ofApp::saveLog(std::string s)
{
    std::string filename = "images/functions_" + s + ".txt";

    myFunctionsTextFile.open(filename,ofFile::WriteOnly);

    int length = curNumberOfSuccessiveVariations;

    myFunctionsTextFile << " -> ";

    if(operationsMode==0)
    {
        for(int i=0;i<length;i++)
        {
            myFunctionsTextFile << getName(variationsParameters[i].variationType) << " -> ";
        }
    }
    else if(operationsMode==1)
    {
        for(int i=0;i<length-2;i++)
        {
            myFunctionsTextFile << getName(variationsParameters[i].variationType) << " -> ";
        }
        if(length>1)
        {
            myFunctionsTextFile << (getName(variationsParameters[length-2].variationType) + " + " + getName(variationsParameters[length-1].variationType)) << " -> ";
        }
        else
        {
            myFunctionsTextFile << getName(variationsParameters[length-1].variationType) << " -> ";
        }
    }
    else if(operationsMode==2)
    {
        for(int i=0;i<length-3;i++)
        {
            myFunctionsTextFile << getName(variationsParameters[i].variationType) << " -> ";
        }
        if(length>2)
        {
            myFunctionsTextFile << (getName(variationsParameters[length-3].variationType) + " + " + getName(variationsParameters[length-2].variationType) + " + " + getName(variationsParameters[length-1].variationType)) << " -> ";
        }
        else if(length>1)
        {
            myFunctionsTextFile << (getName(variationsParameters[length-2].variationType) + " + " + getName(variationsParameters[length-1].variationType)) << " -> ";
        }
        else
        {
            myFunctionsTextFile << getName(variationsParameters[length-1].variationType) << " -> ";
        }
    }
    else if(operationsMode==3)
    {
        for(int i=0;i<length;i++)
        {
            myFunctionsTextFile << getName(variationsParameters[i].variationType) << " -> ";
        }
        for(int i=0;i<length;i++)
        {
            myFunctionsTextFile << getName(variationsParameters[i].variationType) << " -> ";
        }
    }

    myFunctionsTextFile.close();

    std::cout << "Saved functions." << std::endl;
}