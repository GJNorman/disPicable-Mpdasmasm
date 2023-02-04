//
//  Timer.h
//  annoyed
//
//  Created by Greg Norman on 28/1/2023.
//



#ifndef Timer_h
#define Timer_h

#include <chrono>
#include <stdint.h>
class TimerClass
{
    std::chrono::time_point<std::chrono::steady_clock> TimerStart;  // creation time of Class, can be updated if desired
    
    double TimeoutPeriod;                                           //
public:
    uint32_t iteration;         // application defined
    double cumulativeSeconds;
    /*
     Constructors
     */
        TimerClass(double TimeoutPeriod)
        {
            this->TimerStart = this->getTime();
            this->TimeoutPeriod = TimeoutPeriod;
        }
        TimerClass()
        {
            this->TimerStart = this->getTime();
        }
    /*
     Setters
     */
        void setTimeoutPeriod(double TimeoutPeriod)
        {
            this->TimeoutPeriod = TimeoutPeriod;
        }
    
        // update reference start time
        void updateTimerReference()
        {
            this->TimerStart = this->getTime();
        }
    /*
     Getters
     */
        // grab current time
        std::chrono::time_point<std::chrono::steady_clock> getTime()
        {
            return std::chrono::steady_clock::now();
        }
    
    /*
     General Methods
     */
        // check how many seconds have passed since specificied start time
        double CheckDuration(std::chrono::time_point<std::chrono::steady_clock> start)
        {
            auto end = std::chrono::steady_clock::now();
            
            std::chrono::duration<double> elapsed_seconds = end-start;
            
            return elapsed_seconds.count() ;
        }
        // Overload - use creation time as reference
        double CheckDuration()
        {
            return this->CheckDuration(this->TimerStart);
        }
        
        // check if period is greater than "TimeoutPeriod"
        bool checkTimeout()
        {
            if( this->CheckDuration(this->TimerStart) > this->TimeoutPeriod)
            {
                this->updateTimerReference();
                return true;
            }
            
            return false;
        }
    

    

};

#endif /* Timer_h */
