#pragma once

//Singleton class
class GameTimer
{
public:

    static GameTimer* GetInstance();
    
    float GameTime()const; // in seconds
    float DeltaTime()const; // in seconds
    float TotalTime()const; 
    void Reset(); // Call before message loop.
    void Start(); // Call when unpaused.
    void Stop(); // Call when paused.
    void Tick(); // Call every frame.

private:
    //put the constructor in private to create a factory 
    GameTimer();

    //single instance
    static GameTimer* mInstance;
    
    double mSecondsPerCount;
    double mDeltaTime;
    __int64 mBaseTime;
    __int64 mPausedTime;
    __int64 mStopTime;
    __int64 mPrevTime;
    __int64 mCurrTime;
    bool mStopped;
};
