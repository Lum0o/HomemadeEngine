#include "GameTimer.h"

#include "RenderEngine/RenderEngine.h"

GameTimer* GameTimer::mInstance = nullptr;

GameTimer* GameTimer::GetInstance()
{
    if(mInstance == nullptr)
        mInstance = new GameTimer();

    return mInstance;
}

GameTimer::GameTimer() : mSecondsPerCount(0.0), mDeltaTime(-1.0), mBaseTime(0), mPausedTime(0), mPrevTime(0), mCurrTime(0), mStopped(false)
{
    __int64 countsPerSec;
    QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&countsPerSec));
    mSecondsPerCount = 1.0 / static_cast<double>(countsPerSec);
}

void GameTimer::Tick()
{
    if( mStopped )
    {
        mDeltaTime = 0.0;
        return;
    }
    // Get the time this frame.
    __int64 currTime;
    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currTime));
    mCurrTime = currTime;
    // Time difference between this frame and the previous.
    mDeltaTime = (mCurrTime - mPrevTime)*mSecondsPerCount;
    // Prepare for next frame.
    mPrevTime = mCurrTime;
    // Force nonnegative. The DXSDK’s CDXUTTimer mentions that if the
    // processor goes into a power save mode or we get shuffled to
    // another processor, then mDeltaTime can be negative.
    if(mDeltaTime < 0.0)
    {
        mDeltaTime = 0.0;
    }
}
float GameTimer::DeltaTime()const
{
    return static_cast<float>(mDeltaTime);
}

void GameTimer::Reset()
{
    __int64 currTime;
    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currTime));
    mBaseTime = currTime;
    mPrevTime = currTime;
    mStopTime = 0;
    mStopped = false;
}

void GameTimer::Stop()
{
    // If we are already stopped, then don’t do anything.
    if( !mStopped )
    {
        __int64 currTime;
        QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currTime));
        // Otherwise, save the time we stopped at, and set
        // the Boolean flag indicating the timer is stopped.
        mStopTime = currTime;
        mStopped = true;
    }
}
void GameTimer::Start()
{
    __int64 startTime;
    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&startTime));

    if( mStopped )
    {
         // then accumulate the paused time.
         mPausedTime += (startTime - mStopTime);
         // since we are starting the timer back up, the current
         // previous time is not valid, as it occurred while paused.
         // So reset it to the current time.
         mPrevTime = startTime;
         // no longer stopped...
         mStopTime = 0;
         mStopped = false;
    }
}

float GameTimer::TotalTime()const
{
    if( mStopped )
    {
        return static_cast<float>(((mStopTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
    }
    else
    {
         return static_cast<float>(((mCurrTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
    }
}