#include "ofApp.h"

// names of primitive variations
std::string ofApp::getName(int ind)
{
    switch(ind){
        case 0: return "lines"; break;
        case 1: return "twintrian"; break;
        case 2: return "julian"; break;
        case 3: return "juliascope"; break;
        case 4: return "rectangles"; break;
        case 5: return "sinusoidal"; break;
        case 6: return "scry"; break;
        case 7: return "fan2"; break;
        case 8: return "pdj"; break;
        case 9: return "butterfly"; break;
        case 10: return "wedgejulia"; break;
        case 11: return "rotate"; break;
        case 12: return "cross"; break;
        case 13: return "curl"; break;
        case 14: return "rings2"; break;
        case 15: return "heart"; break;
        case 16: return "ex"; break;
        case 17: return "popcorn"; break;
        case 18: return "waves"; break;
        case 19: return "horseshoe"; break;
        case 20: return "polar"; break;
        case 21: return "ennepers"; break;
        case 22: return "cell"; break;
        case 23: return "boarders"; break;
        case 24: return "supershape"; break;
        case 25: return "loonie"; break;
        case 26: return "foci"; break;
        case 27: return "blade"; break;
        case 28: return "ngon"; break;
        case 29: return "diamond"; break;
        case 30: return "oscilloscope"; break;
        case 31: return "oscilloscope2"; break;
        case 32: return "blocky"; break;
        case 33: return "waves2"; break;
        case 34: return "blob"; break;
        case 35: return "bsplit"; break;
        case 36: return "bswirl"; break;
        case 37: return "bwraps7"; break;
        case 38: return "exponential"; break;
        case 39: return "blurzoom"; break;
        case 40: return "bmod"; break;
        case 41: return "bcollide"; break;
        case 42: return "boarders2"; break;
        case 43: return "eyefish"; break;
        case 44: return "disc"; break;
        case 45: return "cot"; break;
        case 46: return "julia"; break;
        case 47: return "spiral"; break;
        case 48: return "tangent"; break;
        case 49: return "rings"; break;
        case 50: return "split"; break;
    default : return "undefined";
    }
    return "";
}