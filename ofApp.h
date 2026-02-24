#pragma once
#include "ofMain.h"

// Simple Synthesizer - Group Project
// We used two OOP relationships:
// 1. INHERITANCE - SineOsc, SquareOsc, TriangleOsc all inherit from BaseOscillator
// 2. COMPOSITION - ofApp owns a list of PianoKey objects

// Base class for all sound types
class BaseOscillator {
public:
    virtual float getSample(float time, float freq) = 0;
    virtual string getName() = 0;
    virtual ~BaseOscillator() {}
};

// Sine wave - smooth sound
class SineOscillator : public BaseOscillator {
public:
    float getSample(float time, float freq) override {
        return sin(TWO_PI * freq * time);
    }
    string getName() override { return "Sine"; }
};

// Square wave - buzzy sound
class SquareOscillator : public BaseOscillator {
public:
    float getSample(float time, float freq) override {
        float phase = fmod(freq * time, 1.0f);
        return phase < 0.5f ? 1.0f : -1.0f;
    }
    string getName() override { return "Square"; }
};

// Triangle wave - in between sine and square
class TriangleOscillator : public BaseOscillator {
public:
    float getSample(float time, float freq) override {
        float phase = fmod(freq * time, 1.0f);
        return phase < 0.5f ? (4.0f * phase - 1.0f) : (3.0f - 4.0f * phase);
    }
    string getName() override { return "Triangle"; }
};

// One piano key - has mouse rect and keyboard shortcut
// COMPOSITION: ofApp has a vector of these
struct PianoKey {
    float       freq;
    string      label;
    string      keyboardKey;
    ofRectangle rect;
    bool        isBlack;
    bool        pressed;
};

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void exit();

    void audioOut(ofSoundBuffer& buffer);

    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseDragged(int x, int y, int button);

    void keyPressed(int key);
    void keyReleased(int key);

private:
    void buildPianoKeys();

    SineOscillator     sineOsc;
    SquareOscillator   squareOsc;
    TriangleOscillator triangleOsc;
    BaseOscillator*    currentOsc;
    int                currentOscIndex;

    vector<PianoKey> pianoKeys;

    ofSoundStream soundStream;
    double        audioTime;
    float         volume;
    bool          notePlaying;
    float         currentFreq;
    string        currentNoteName;

    vector<float> waveBuffer;
    ofMutex       audioMutex;

    ofRectangle   volTrack;
    bool          draggingVol;
};
