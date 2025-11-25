#include "ofxEasyFft.h"

ofxEasyFft::ofxEasyFft()
:useNormalization(true) {
}
ofxEasyFft::~ofxEasyFft(){
    stream.close();
}

void ofxEasyFft::setup(int bufferSize, fftWindowType windowType, fftImplementation implementation, int audioBufferSize, int audioSampleRate) {
	if(bufferSize < audioBufferSize) {
		ofLogWarning("ofxEasyFft") << "bufferSize (" << bufferSize << ") less than audioBufferSize (" << audioBufferSize << "), using " << audioBufferSize;
		bufferSize = audioBufferSize;
	}
	fft = ofxFft::create(bufferSize, windowType, implementation);
	
	bins.resize(fft->getBinSize());
	
	audioFront.resize(bufferSize);
	audioMiddle.resize(bufferSize);
	audioBack.resize(bufferSize);
	audioRaw.resize(bufferSize);
	
    stream.getDeviceList();
    stream.setup(0, 1, audioSampleRate, audioBufferSize, 2);
    stream.setInput(this);
}

void ofxEasyFft::setUseNormalization(bool useNormalization) {
	this->useNormalization = useNormalization;
}

void ofxEasyFft::update() {
	soundMutex.lock();
	audioFront = audioMiddle;
	soundMutex.unlock();
	
	fft->setSignal(&audioFront[0]);
	float* curFft = fft->getAmplitude();
	copy(curFft, curFft + fft->getBinSize(), bins.begin());
	normalize(bins);
}

vector<float>& ofxEasyFft::getAudio() {
	return audioFront;
}

vector<float>& ofxEasyFft::getBins() {
	return bins;
}

void ofxEasyFft::audioReceived(float* input, int bufferSize, int nChannels) {
	if(bufferSize <= 0 || input == nullptr) {
		ofLogWarning("ofxEasyFft") << "audioReceived called with empty input or zero bufferSize: " << bufferSize;
		return;
	}

	// Ensure internal buffers are at least as large as incoming buffer
	if((int)audioRaw.size() < bufferSize) {
		ofLogWarning("ofxEasyFft") << "audioRaw size (" << audioRaw.size() << ") < bufferSize (" << bufferSize << "), resizing internal buffers.";
		audioRaw.resize(bufferSize);
		audioBack.resize(bufferSize);
		audioMiddle.resize(bufferSize);
		audioFront.resize(bufferSize);
	}

	if(audioRaw.size() > (size_t)bufferSize) {
		// shift old data left by bufferSize
		copy(audioRaw.begin() + bufferSize, audioRaw.end(), audioRaw.begin()); // shift old
	}

	// now push new data at the end (guaranteed space)
	copy(input, input + bufferSize, audioRaw.end() - bufferSize); // push new

	// copy to back and normalize
	if(audioBack.size() != audioRaw.size()) audioBack.resize(audioRaw.size());
	copy(audioRaw.begin(), audioRaw.end(), audioBack.begin());
	normalize(audioBack);

	soundMutex.lock();
	audioMiddle = audioBack;
	soundMutex.unlock();
}



void ofxEasyFft::normalize(vector<float>& data) {
	if(useNormalization) {
		float maxValue = 0;
		for(int i = 0; i < data.size(); i++) {
			if(abs(data[i]) > maxValue) {
				maxValue = abs(data[i]);
			}
		}
		for(int i = 0; i < data.size(); i++) {
			data[i] /= maxValue;
		}
	}
}
