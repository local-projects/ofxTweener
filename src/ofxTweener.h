/*
 *  ofxTweener.h
 *  openFrameworks
 *
 *  Created by Sander ter Braak on 26-08-10.
 *	added completeEvent, some code formatting and other stuff by Patrick Meister, rob&rose grafik, November2012
 *
 */

#ifndef _OFXTWEEN
#define _OFXTWEEN

#include "poco/Timestamp.h"
#include "ofMain.h"
#include "ofxTransitions.h"
#include "BasicScreenObject.h"

#define TWEENMODE_OVERRIDE 0x01
#define TWEENMODE_SEQUENCE 0x02

class Tween {
public:
	typedef float (ofxTransitions::*easeFunction)(float, float, float, float);
	float *_var;
	float _from, _to, _duration, _by, _useBezier;

	//	void *target;
	//	void *getterMethod;
	//	void *setterMethod;
	// float (*getterMethod)();
	// void (*setterMethod)(float);

	std::function<float()> getter;
	std::function<void(float)> setter;

	bool useGetterSetter;

	easeFunction _easeFunction;
	Poco::Timestamp _timestamp;
    
    BasicScreenObject *callingObject;
};

class ofxTweener : public ofBaseApp {

public:
	ofxTweener();

	void addTween(BasicScreenObject* callingObject, float &var, float to, float time);

	// This more generic template signature works, but doesn't enforce function type signature(?):
	// void addTween(TweenTargetClass *target, TweenTargetGetterClass getterMethod, TweenTargetSetterClass setterMethod, float to, float time) {

	// Extra classes handle calls to superclasses...
	template <class TweenTargetClass, class TweenTargetGetterClass, class TweenTargetSetterClass>
	void addTween(TweenTargetClass *target, float (TweenTargetGetterClass::*getterMethod)() const, void (TweenTargetSetterClass::*setterMethod)(float), float to,
								float time) {
		// Pass this through to the std::function implementation
		addTween(std::bind(getterMethod, std::ref(*target)), std::bind(setterMethod, std::ref(*target), std::placeholders::_1), to, time);
	};

	// Or use std::function directly, but then binding syntax is so nasty...
	void addTween(std::function<float()> getter, std::function<void(float)> setter, float to, float time);
	void addTween(BasicScreenObject* callingObject, float &var, float to, float time, float (ofxTransitions::*ease)(float, float, float, float));
    
	void addTween(BasicScreenObject* callingObject, float &var, float to, float time, float (ofxTransitions::*ease)(float, float, float, float), float delay);
    
	void addTween(BasicScreenObject* callingObject, float &var, float to, float time, float (ofxTransitions::*ease)(float, float, float, float), float delay, float bezierPoint);

	void removeTween(float &var);
	void setTimeScale(float scale);
	void update();
	void removeAllTweens();
	void setMode(int mode);

	int getTweenCount();

	ofEvent<float> onTweenCompleteEvent;

private:
	float _scale;
	ofxTransitions a;
	bool _override;
	void addTween(BasicScreenObject* callingObject, float &var, float to, float time, float (ofxTransitions::*ease)(float, float, float, float), float delay, float bezierPoint, bool useBezier);
	float bezier(float b, float e, float t, float p);
	vector<Tween> tweens;
};

extern ofxTweener Tweener;
#endif