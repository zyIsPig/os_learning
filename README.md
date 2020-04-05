# os_learning

### pthread_cond_wait()&pthread_cond_signal()
pthread_cond_wait doesn't contain the reference of mutex, so it can't release the mutex. It only seed a signal to wake up the 
waiting thread. pthread_cond_wait unlocks the mutex just before it sleeps (as you note), but then it reaquires the mutex (which may require waiting) when it is signalled, before it wakes up. 
So if the signalling thread holds the mutex (the usual case), the waiting thread will not proceed until the signalling thread also unlocks the mutex.
[reference](https://stackoverflow.com/questions/16522858/understanding-of-pthread-cond-wait-and-pthread-cond-signal/16524148)
