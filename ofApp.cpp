#include "ofApp.h"

void ofApp::setup() {
    ofSetWindowTitle("Group Synthesizer");
    ofSetFrameRate(60);
    ofBackground(30, 30, 40);
    currentOsc=&sineOsc; currentOscIndex=0;
    audioTime=0.0; volume=0.6f; notePlaying=false;
    currentFreq=440.0f; currentNoteName=""; draggingVol=false;
    waveBuffer.assign(256,0.0f);
    volTrack=ofRectangle(50,530,280,14);
    buildPianoKeys();
    ofSoundStreamSettings settings;
    settings.setOutListener(this);
    settings.sampleRate=44100; settings.numOutputChannels=2;
    settings.numInputChannels=0; settings.bufferSize=512;
    soundStream.setup(settings);
}

void ofApp::buildPianoKeys(){
    struct NoteInfo{float freq;string note;string kbKey;bool isBlack;int whitePos;};
    vector<NoteInfo> notes={
        {261.63f,"C","A",false,0},{277.18f,"C#","W",true,0},
        {293.66f,"D","S",false,1},{311.13f,"D#","E",true,1},
        {329.63f,"E","D",false,2},{349.23f,"F","F",false,3},
        {369.99f,"F#","T",true,3},{392.00f,"G","G",false,4},
        {415.30f,"G#","Y",true,4},{440.00f,"A","H",false,5},
        {466.16f,"A#","U",true,5},{493.88f,"B","J",false,6},
        {523.25f,"C5","K",false,7},
    };
    int sx=50,sy=340,wkW=55,wkH=130,bkW=32,bkH=80;
    for(auto& n:notes) if(!n.isBlack){
        PianoKey k;k.freq=n.freq;k.label=n.note;k.keyboardKey=n.kbKey;
        k.isBlack=false;k.pressed=false;
        k.rect=ofRectangle(sx+n.whitePos*wkW,sy,wkW-2,wkH);
        pianoKeys.push_back(k);
    }
    for(auto& n:notes) if(n.isBlack){
        PianoKey k;k.freq=n.freq;k.label=n.note;k.keyboardKey=n.kbKey;
        k.isBlack=true;k.pressed=false;
        k.rect=ofRectangle(sx+n.whitePos*wkW+wkW-bkW/2,sy,bkW,bkH);
        pianoKeys.push_back(k);
    }
}

void ofApp::update(){}

void ofApp::draw(){
    ofBackground(30,30,40);
    ofSetColor(255,255,255);
    ofDrawBitmapString("Simple Synthesizer - Group Project",50,30);
    ofSetColor(140,140,160);
    ofDrawBitmapString("Click piano keys OR use keyboard: A S D F G H J  (sharps: W E T Y U)",50,48);
    ofSetColor(40,40,55); ofDrawRectangle(50,60,580,150);
    ofSetColor(80,80,100); ofNoFill(); ofDrawRectangle(50,60,580,150); ofFill();
    ofSetColor(120,120,140);
    ofDrawBitmapString("Waveform - "+currentOsc->getName(),65,82);
    {
        ofScopedLock lock(audioMutex);
        if(!waveBuffer.empty()){
            if(currentOscIndex==0) ofSetColor(80,160,255);
            else if(currentOscIndex==1) ofSetColor(255,120,60);
            else ofSetColor(80,220,130);
            ofSetLineWidth(2);
            int n=waveBuffer.size();
            float cx=50,cy=135,cw=580,ch=150;
            ofBeginShape();
            for(int i=0;i<n;i++) ofVertex(cx+(float)i/n*cw,cy+waveBuffer[i]*ch*0.38f);
            ofEndShape(false);
            ofSetLineWidth(1);
        }
    }
    ofSetColor(180,180,200);
    ofDrawBitmapString("Sound type (or press 1 / 2 / 3):",50,240);
    string names[]={"Sine","Square","Triangle"};
    ofColor cols[]={ofColor(80,160,255),ofColor(255,120,60),ofColor(80,220,130)};
    string descs[]={"smooth","buzzy","in-between"};
    for(int i=0;i<3;i++){
        ofRectangle btn(50+i*150,248,138,42);
        bool sel=(i==currentOscIndex);
        ofSetColor(sel?cols[i]:ofColor(50,50,65)); ofDrawRectangle(btn);
        ofSetColor(sel?cols[i]:ofColor(90,90,110)); ofNoFill(); ofDrawRectangle(btn); ofFill();
        ofSetColor(sel?ofColor(20,20,30):ofColor(200,200,210));
        ofDrawBitmapString(names[i]+" ("+descs[i]+")",btn.x+8,btn.y+26);
    }
    ofSetColor(160,160,180);
    if(notePlaying){ofSetColor(180,255,180);ofDrawBitmapString("Playing: "+currentNoteName+"  ("+ofToString(currentFreq,1)+" Hz)",50,308);}
    else ofDrawBitmapString("Not playing - press a key or click the piano",50,308);
    ofSetColor(160,160,180); ofDrawBitmapString("Piano:",50,328);
    for(auto& k:pianoKeys) if(!k.isBlack){
        ofSetColor(k.pressed?ofColor(160,210,255):ofColor(235,235,240)); ofDrawRectangle(k.rect);
        ofSetColor(90,90,110); ofNoFill(); ofDrawRectangle(k.rect); ofFill();
        ofSetColor(80,80,100); ofDrawBitmapString(k.label,k.rect.x+10,k.rect.y+k.rect.height-22);
        ofSetColor(120,120,150); ofDrawBitmapString("["+k.keyboardKey+"]",k.rect.x+8,k.rect.y+k.rect.height-8);
    }
    for(auto& k:pianoKeys) if(k.isBlack){
        ofSetColor(k.pressed?ofColor(60,120,190):ofColor(28,28,38)); ofDrawRectangle(k.rect);
        ofSetColor(60,60,80); ofNoFill(); ofDrawRectangle(k.rect); ofFill();
        ofSetColor(160,160,180); ofDrawBitmapString(k.keyboardKey,k.rect.x+8,k.rect.y+k.rect.height-8);
    }
    ofSetColor(160,160,180); ofDrawBitmapString("Volume: "+ofToString((int)(volume*100))+"%",50,520);
    ofSetColor(45,45,60); ofDrawRectangle(volTrack);
    ofSetColor(80,140,220); ofDrawRectangle(volTrack.x,volTrack.y,volTrack.width*volume,volTrack.height);
    float hx=volTrack.x+volTrack.width*volume-7;
    ofSetColor(200,200,230); ofDrawRectangle(hx,volTrack.y-5,14,volTrack.height+10);
    ofSetColor(100,100,120); ofNoFill(); ofDrawRectangle(volTrack); ofFill();
}

void ofApp::mousePressed(int x,int y,int button){
    for(int i=0;i<3;i++){
        ofRectangle btn(50+i*150,248,138,42);
        if(btn.inside(x,y)){currentOscIndex=i;if(i==0)currentOsc=&sineOsc;else if(i==1)currentOsc=&squareOsc;else currentOsc=&triangleOsc;return;}
    }
    ofRectangle exp=ofRectangle(volTrack.x,volTrack.y-8,volTrack.width,volTrack.height+16);
    if(exp.inside(x,y)){draggingVol=true;volume=ofClamp((x-volTrack.x)/volTrack.width,0.0f,1.0f);return;}
    for(auto& k:pianoKeys) if(k.isBlack&&k.rect.inside(x,y)){k.pressed=true;notePlaying=true;currentFreq=k.freq;currentNoteName=k.label;return;}
    for(auto& k:pianoKeys) if(!k.isBlack&&k.rect.inside(x,y)){k.pressed=true;notePlaying=true;currentFreq=k.freq;currentNoteName=k.label;return;}
}
void ofApp::mouseDragged(int x,int y,int button){if(draggingVol)volume=ofClamp((x-volTrack.x)/volTrack.width,0.0f,1.0f);}
void ofApp::mouseReleased(int x,int y,int button){draggingVol=false;notePlaying=false;for(auto& k:pianoKeys)k.pressed=false;}
void ofApp::keyPressed(int key){
    char c=tolower(key);
    for(auto& pk:pianoKeys) if(tolower(pk.keyboardKey[0])==c){pk.pressed=true;notePlaying=true;currentFreq=pk.freq;currentNoteName=pk.label;return;}
    if(key=='1'){currentOscIndex=0;currentOsc=&sineOsc;}
    if(key=='2'){currentOscIndex=1;currentOsc=&squareOsc;}
    if(key=='3'){currentOscIndex=2;currentOsc=&triangleOsc;}
}
void ofApp::keyReleased(int key){
    char c=tolower(key);
    for(auto& pk:pianoKeys) if(tolower(pk.keyboardKey[0])==c){pk.pressed=false;notePlaying=false;return;}
}
void ofApp::audioOut(ofSoundBuffer& buffer){
    int n=buffer.getNumFrames(); double dt=1.0/44100.0;
    ofScopedLock lock(audioMutex); waveBuffer.resize(n);
    for(int i=0;i<n;i++){
        float sample=0.0f;
        if(notePlaying)sample=currentOsc->getSample((float)audioTime,currentFreq)*volume*0.5f;
        buffer[i*2]=sample;buffer[i*2+1]=sample;waveBuffer[i]=sample;audioTime+=dt;
    }
}
void ofApp::exit(){soundStream.stop();soundStream.close();}
