/*
 *  ofxTweener.cpp
 *  openFrameworks
 *
 *  Created by Sander ter Braak on 26-08-10.
 *	added completeEvent, some code formatting and other stuff by Patrick Meister, rob&rose grafik, November2012
 *
 */

#include "ofxTweener.h"

ofxTweener Tweener;

ofxTweener::ofxTweener() {
	_scale = 1;
	setMode(TWEENMODE_OVERRIDE);
}

// template <class TweenTargetClass>
// void addTween(TweenTargetClass *target, float (TweenTargetClass::*getterMethod)(const void *), void (TweenTargetClass::*setterMethod)(const void *, float),
//							float to, float time) {
//
//	cout << "target->getterMethod(): " << target->getterMethod() << endl;
//
//	target->setterMethod(to);
//}

void ofxTweener::addTween(std::function<float()> getter, std::function<void(float)> setter, float to, float time) {
	Tween t;

	t.getter = getter;
	t.setter = setter;
	t.useGetterSetter = true;
	// t._var = getter.target();
	t._from = t.getter();
	t._to = to;
	t._by = 0;
	t._useBezier = false;
	t._easeFunction = &ofxTransitions::easeOutExpo;
	t._timestamp = Poco::Timestamp() + ((0 / _scale) * 1000000.0f);
	t._duration = (time / _scale) * 1000000.0f;

	tweens.push_back(t);
}

// template <class TweenTargetClass, class TweenTargetGetterClass, class TweenTargetSetterClass>
// void ofxTweener::addTween(TweenTargetClass *target, float (TweenTargetGetterClass::*getterMethod)() const, void
// (TweenTargetSetterClass::*setterMethod)(float),
//													float to, float time) {
//
//	int i = 1;
//}

//	Tween t;
//	t.target = target;
//	t.getterMethod = getterMethod;
//	t.setterMethod = setterMethod;
//	t.useGetterSetter = true;
//	t._var = 0;
//	// t._from = (t.target->*t.getterMethod)();
//	t._to = to;
//	t._by = 0;
//	t._useBezier = false;
//	t._easeFunction = &ofxTransitions::easeOutExpo;
//	t._timestamp = Poco::Timestamp() + ((0 / _scale) * 1000000.0f);
//	t._duration = (time / _scale) * 1000000.0f;
//
//	tweens.push_back(t);

//	// float to, float time) {
//
//	cout << "target: " << target->getName() << endl;
//
//	cout << "target->getterMethod(): " << (target->*getterMethod)() << endl;
//	(target->*setterMethod)(to);
//}

void ofxTweener::addTween(BasicScreenObject* callingObject, float &var, float to, float time) {
	addTween(callingObject, var, to, time, &ofxTransitions::easeOutExpo, 0, 0, false);
}

void ofxTweener::addTween(BasicScreenObject* callingObject, float &var, float to, float time, float (ofxTransitions::*ease)(float, float, float, float)) {
	addTween(callingObject, var, to, time, ease, 0, 0, false);
}
void ofxTweener::addTween(BasicScreenObject* callingObject, float &var, float to, float time, float (ofxTransitions::*ease)(float, float, float, float), float delay) {
	addTween(callingObject, var, to, time, ease, delay, 0, false);
}
void ofxTweener::addTween(BasicScreenObject* callingObject, float &var, float to, float time, float (ofxTransitions::*ease)(float, float, float, float), float delay, float bezierPoint) {
	addTween(callingObject, var, to, time, ease, delay, bezierPoint, true);
}

void ofxTweener::addTween(BasicScreenObject* callingObject, float &var, float to, float time, float (ofxTransitions::*ease)(float, float, float, float), float delay, float bezierPoint, bool useBezier) {
//    if (var == to) return;
   
    
	float from = var;
	float _delay = delay;
	Poco::Timestamp latest = 0;

	for (int i = 0; i < tweens.size(); ++i) {
		if (tweens[i]._var == &var) {
			// object already tweening, just kill the old one
			if (_override) {
                tweens[i].callingObject = callingObject;
				tweens[i]._from = from;
				tweens[i]._to = to;
				tweens[i]._by = bezierPoint;
				tweens[i]._useBezier = useBezier;
				tweens[i]._easeFunction = ease;
				tweens[i]._timestamp = Poco::Timestamp() + ((delay / _scale) * 1000000.0f);
				tweens[i]._duration = (time / _scale) * 1000000.0f;
				return;
			} else {
				// sequence mode
				if ((tweens[i]._timestamp + tweens[i]._duration) > latest) {
					latest = (tweens[i]._timestamp + tweens[i]._duration);
					delay = _delay + ((tweens[i]._duration - tweens[i]._timestamp.elapsed()) / 1000000.0f);
					from = tweens[i]._to;
				}
			}
		}
	}

	Tween t;
	t.useGetterSetter = false;
    t.callingObject = callingObject;
	t._var = &var;
	t._from = from;
	t._to = to;
	t._by = bezierPoint;
	t._useBezier = useBezier;
	t._easeFunction = ease;
	t._timestamp = Poco::Timestamp() + ((delay / _scale) * 1000000.0f);
	t._duration = (time / _scale) * 1000000.0f;

	tweens.push_back(t);
}

void ofxTweener::update() {
    int t = ofGetElapsedTimeMillis();
    int preTweens = tweens.size();
    
	for (int i = tweens.size() - 1; i >= 0; --i) {
		if (float(tweens[i]._timestamp.elapsed()) >= float(tweens[i]._duration)) {
			// tween is done
			bool found = false;
			if (!_override) {
				// if not found anymore, place on exact place
				for (int j = 0; j < tweens.size(); ++j) {
					if (tweens[j]._var == tweens[i]._var) {
						found = true;
						break;
					}
				}
			}
			if (!found) tweens[i]._var[0] = tweens[i]._to;
            
//			ofNotifyEvent(onTweenCompleteEvent, tweens[i]._var[0]);
            tweens[i].callingObject->onTweenComplete(tweens[i]._var[0]);
			tweens.erase(tweens.begin() + i);
			// dispatch event here! complete event

		} else if (float(tweens[i]._timestamp.elapsed()) > 0) {
			// smaller than 0 would be delayed
			if (tweens[i]._useBezier) {
				tweens[i]._var[0] = bezier(tweens[i]._from, tweens[i]._to,
																	 (a.*tweens[i]._easeFunction)(float(tweens[i]._timestamp.elapsed()), 0, 1, float(tweens[i]._duration)), tweens[i]._by);
			} else {

				if (tweens[i].useGetterSetter) {

					tweens[i].setter((a.*tweens[i]._easeFunction)(float(tweens[i]._timestamp.elapsed()), tweens[i]._from, tweens[i]._to - tweens[i]._from,
																												float(tweens[i]._duration)));

				} else {

					tweens[i]._var[0] =
							(a.*tweens[i]._easeFunction)(float(tweens[i]._timestamp.elapsed()), tweens[i]._from, tweens[i]._to - tweens[i]._from, float(tweens[i]._duration));
				}
			}
			// dispatch change event
		}
	}
    int elapsedT = ofGetElapsedTimeMillis() - t;
    if (elapsedT > 16){
        cout << "update tween: " << preTweens << "->" << tweens.size() << " = " << preTweens - tweens.size() << " in " << elapsedT << "ms" << endl;
    }
}

void ofxTweener::removeTween(float &var) {
	for (int i = 0; i < tweens.size(); i++) {
		if (tweens[i]._var == &var) {
			// tween found, erase it
			tweens.erase(tweens.begin() + i);
			return;
		}
	}
}

float ofxTweener::bezier(float b, float e, float t, float p) {
	return b + t * (2 * (1 - t) * (p - b) + t * (e - b));
}

void ofxTweener::removeAllTweens() {
	tweens.clear();
}
void ofxTweener::setMode(int mode) {
	_override = (mode == TWEENMODE_OVERRIDE);
}

int ofxTweener::getTweenCount() {
	return int(tweens.size());
}

void ofxTweener::setTimeScale(float scale) {
	_scale = scale;
}
