#pragma once

#include "FairyGUIMacros.h"

#include "base/Ref.h"

NS_FGUI_BEGIN

using namespace tweenfunc;

enum LoopType
{
    None,
    Yoyo,
    Loop
};

template<class T>
class Action : public Ref
{
public:
    Action(){}
    ~Action() { _isValid = false; _isStart = false; }

    CREATE_FUNC(Action<T>);

    bool init() { _isValid = true; return true; }

    void SetTweenData(T sourceData, T destData, float delay, float tweenTime, TweenType easeType, function<void(T)> updateFunc, function<void()> finishFunc)
    {
        if (_isValid == false) return;

        _sourceValue = sourceData;
        _destValue = destData;
        _delayTime = delay;
        _tweenTime = tweenTime;
        _easeType = easeType;
        _updateFunc = updateFunc;
        _finishFunc = finishFunc;
    }

    void SetLoopType(LoopType loopType, int playTimes = 1)
    {
        _loopType = loopType;
        _playTimes = playTimes;
    }

    void Run()
    {
        if (_isValid == false) return;
        if (_isStart == true) return;

        _isStart = true;
        _currentTime = 0.0f;
        this->retain();
        CALL_PER_FRAME(Action<T>, Update);

        if(_playTimes == 0)
        {
            this->Stop();
        }
    }

    void Update(float delta)
    {
        if (_isValid == false) return;
        if (_isStart == false) return;

        _currentTime += delta;
        if(_currentTime > _delayTime)
        {
            //caculate current value
            if (_updateFunc)
                _updateFunc(this->CaculateValue());

            if(_currentTime > (_delayTime + _tweenTime))
            {
                if(_loopType == LoopType::Yoyo && _playTimes > 0)
                {
                    --_playTimes;
                    _currentTime = _delayTime + _tweenTime - _currentTime;
                    T tmp = _sourceValue;
                    _sourceValue = _destValue;
                    _destValue = tmp;

                    switch (_easeType)
                    {
                    case Sine_EaseIn:
                        _easeType = Sine_EaseOut;
                        break;
                    case Sine_EaseOut:
                        _easeType = Sine_EaseIn;
                        break;
                    case Quad_EaseIn:
                        _easeType = Quad_EaseOut;
                        break;
                    case Quad_EaseOut:
                        _easeType = Quad_EaseIn;
                        break;
                    case Cubic_EaseIn:
                        _easeType = Cubic_EaseOut;
                        break;
                    case Cubic_EaseOut:
                        _easeType = Cubic_EaseIn;
                        break;
                    case Quart_EaseIn:
                        _easeType = Quart_EaseOut;
                        break;
                    case Quart_EaseOut:
                        _easeType = Quart_EaseIn;
                        break;
                    case Quint_EaseIn:
                        _easeType = Quint_EaseOut;
                        break;
                    case Quint_EaseOut:
                        _easeType = Quint_EaseIn;
                        break;
                    case Expo_EaseIn:
                        _easeType = Expo_EaseOut;
                        break;
                    case Expo_EaseOut:
                        _easeType = Expo_EaseIn;
                        break;
                    case Circ_EaseIn:
                        _easeType = Circ_EaseOut;
                        break;
                    case Circ_EaseOut:
                        _easeType = Circ_EaseIn;
                        break;
                    case Elastic_EaseIn:
                        _easeType = Elastic_EaseOut;
                        break;
                    case Elastic_EaseOut:
                        _easeType = Elastic_EaseIn;
                        break;
                    case Back_EaseIn:
                        _easeType = Back_EaseOut;
                        break;
                    case Back_EaseOut:
                        _easeType = Back_EaseIn;
                        break;
                    case Bounce_EaseIn:
                        _easeType = Bounce_EaseOut;
                        break;
                    case Bounce_EaseOut:
                        _easeType = Bounce_EaseIn;
                        break;
                    default:
                        break;
                    }
                }
                else if (_loopType == LoopType::Loop && _playTimes != 0)
                {
                    --_playTimes;
                    _currentTime = _delayTime + _tweenTime - _currentTime;
                }
                else
                {
                    this->Stop();
                }
            }
        }
    }

    void Stop()
    {
        if (_isValid == false) return;
        if (_isStart == false) return;

        _isStart = false;
        CALL_PER_FRAME_CANCEL(Action<T>, Update);
        if(_updateFunc)
            _updateFunc(_destValue);
        if(_finishFunc)
            _finishFunc();
        this->release();
        
    }

    void Kill() {
        if (_isValid == false) return;
        if (_isStart == false) return;

        _isStart = false;
        CALL_PER_FRAME_CANCEL(Action<T>, Update);
        this->release();
    }

    bool IsRunning() {
        return _isStart;
    }

private:
    T CaculateValue()
    {
        if (_isValid == false) return _sourceValue;

        float per = (_currentTime - _delayTime) / _tweenTime;
        per = per < 1.0f ? per : 1.0f;
        float now = tweenTo(per, _easeType, nullptr);
        return _sourceValue + (_destValue - _sourceValue) * now;
    }

    T _sourceValue;
    T _destValue;

    TweenType _easeType;
    float _tweenTime;
    float _delayTime;
    float _currentTime;
    function<void(T)> _updateFunc;
    function<void()> _finishFunc;

    bool _isValid = false;
    bool _isStart = false;

    LoopType _loopType = LoopType::None;
    int _playTimes = 1;
};

NS_FGUI_END
