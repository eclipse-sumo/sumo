/****************************************************************************/
/// @file    FXWorkerThread.h
/// @author  Michael Behrisch
/// @date    2014-07-13
/// @version $Id$
///
// A thread class together with a pool and a task for parallelized computation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2004-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

#ifndef FXWorkerThread_h
#define FXWorkerThread_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <list>
#include <vector>
#include <fx.h>
#include <FXThread.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class FXWorkerThread
 * @brief A thread repeatingly calculating incoming tasks
 */
class FXWorkerThread : public FXThread {

public:
    /**
     * @class FXWorkerThread::Task
     * @brief Abstract superclass of a task to be run with an index to keep track of pending tasks
     */
    class Task {
    public:
        /// @brief Desctructor
        virtual ~Task() {};

        /** @brief Abstract method which in subclasses should contain the computations to be performed.
         *
         * If there is data to be shared among several tasks (but not among several threads) it can be put in the
         *  a thread class subclassing the FXWorkerThread. the instance of the thread is then made available
         *  via the context parameter.
         *
         * @param[in] context The thread which runs the task
         */
        virtual void run(FXWorkerThread* context) = 0;

        /** @brief Sets the running index of this task.
         *
         * Every task receive an index which is unique among all pending tasks of the same thread pool.
         *
         * @param[in] newIndex the index to assign
         */
        void setIndex(const int newIndex) {
            myIndex = newIndex;
        }
    private:
        /// @brief the index of the task, valid only after the task has been added to the pool
        int myIndex;
    };

    /**
     * @class FXWorkerThread::Pool
     * @brief A pool of worker threads which distributes the tasks and collects the results
     */
    class Pool {
    public:
        /** @brief Constructor
         *
         * May initialize the pool with a given number of workers.
         *
         * @param[in] numThreads the number of threads to create
         */
        Pool(int numThreads = 0) : myRunningIndex(0), myNumFinished(0) {
            while (numThreads > 0) {
                new FXWorkerThread(*this);
                numThreads--;
            }
        }

        /** @brief Destructor
         *
         * Stopping and deleting all workers by calling clear.
         */
        virtual ~Pool() {
            clear();
        }

        /** @brief Stops and deletes all worker threads.
         */
        void clear() {
            for (std::vector<FXWorkerThread*>::iterator it = myWorkers.begin(); it != myWorkers.end(); ++it) {
                delete *it;
            }
            myWorkers.clear();
        }

        /** @brief Adds the given thread to the pool.
         *
         * @param[in] w the thread to add
         */
        void addWorker(FXWorkerThread* const w) {
//            if (myWorkers.empty()) std::cout << "created pool at " << SysUtils::getCurrentMillis() << std::endl;
            myWorkers.push_back(w);
        }

        /** @brief Gives a number to the given task and assigns it to a randomly chosen worker.
         *
         * @param[in] t the task to add
         */
        void add(Task* const t) {
            t->setIndex(myRunningIndex++);
            myWorkers[myRunningIndex % myWorkers.size()]->add(t);
        }

        /** @brief Adds the given task to the list of finished tasks and assigns it to a randomly chosen worker.
         *
         * Locks the internal mutex and counts the finished tasks. This is to be called by the worker thread only.
         *
         * @param[in] t the task to add
         */
        void addFinished(Task* const t) {
            myMutex.lock();
            myNumFinished++;
            myFinishedTasks.push_back(t);
            myCondition.signal();
            myMutex.unlock();
        }

        /// @brief waits for all tasks to be finished
        void waitAll() {
            myMutex.lock();
            while (myNumFinished < myRunningIndex) {
                myCondition.wait(myMutex);
            }
//            if (myRunningIndex > 0) std::cout << "finished waiting for " << myRunningIndex << " tasks at " << SysUtils::getCurrentMillis() << std::endl;
            for (std::list<Task*>::iterator it = myFinishedTasks.begin(); it != myFinishedTasks.end(); ++it) {
                delete *it;
            }
            myFinishedTasks.clear();
            myRunningIndex = 0;
            myNumFinished = 0;
            myMutex.unlock();
        }

        /** @brief Checks whether there are currently more pending tasks than threads.
         *
         * This is only a rough estimate because the tasks are already assigned and there could be an idle thread even though the
         *  number of tasks is large.
         *
         * @return whether there are enough tasks to let all threads work
         */
        bool isFull() const {
            return myRunningIndex - myNumFinished >= size();
        }

        /** @brief Returns the number of threads in the pool.
         *
         * @return the number of threads
         */
        int size() const {
            return (int)myWorkers.size();
        }

        /// @brief locks the pool mutex
        void lock() {
            myPoolMutex.lock();
        }

        /// @brief unlocks the pool mutex
        void unlock() {
            myPoolMutex.unlock();
        }

    private:
        /// @brief the current worker threads
        std::vector<FXWorkerThread*> myWorkers;
        /// @brief the internal mutex for the task list
        FXMutex myMutex;
        /// @brief the pool mutex for external sync
        FXMutex myPoolMutex;
        /// @brief the semaphore to wait on for finishing all tasks
        FXCondition myCondition;
        /// @brief list of finished tasks
        std::list<Task*> myFinishedTasks;
        /// @brief the running index for the next task
        int myRunningIndex;
        /// @brief the number of finished tasks (is reset when the pool runs empty)
        int myNumFinished;
    };

public:
    /** @brief Constructor
     *
     * Adds the thread to the given pool and starts it.
     *
     * @param[in] pool the pool for this thread
     */
    FXWorkerThread(Pool& pool): FXThread(), myPool(pool), myStopped(false), myCounter(0) {
        pool.addWorker(this);
        start();
    }

    /** @brief Destructor
     *
     * Stops the thread by calling stop.
     */
    virtual ~FXWorkerThread() {
        stop();
    }

    /** @brief Adds the given task to this thread to be calculated
     *
     * @param[in] t the task to add
     */
    void add(Task* t) {
        myMutex.lock();
        myTasks.push_back(t);
        myCondition.signal();
        myMutex.unlock();
    }

    /** @brief Main execution method of this thread.
     *
     * Checks for new tasks, calculates them and puts them in the finished list of the pool until being stopped.
     *
     * @return always 0
     */
    FXint run() {
        while (!myStopped) {
            myMutex.lock();
            while (!myStopped && myTasks.empty()) {
                myCondition.wait(myMutex);
            }
            if (myStopped) {
                myMutex.unlock();
                break;
            }
            Task* t = myTasks.front();
            myTasks.pop_front();
            myMutex.unlock();
            t->run(this);
            myCounter++;
//            if (myCounter % 1000 == 0) std::cout << (size_t)this << " ran " << myCounter << " tasks " << std::endl;
            myPool.addFinished(t);
        }
//        std::cout << "ran " << myCounter << " tasks " << std::endl;
        return 0;
    }

    /** @brief Stops the thread
     *
     * The currently running task will be finished but all further tasks are discarded.
     */
    void stop() {
        myMutex.lock();
        myStopped = true;
        myCondition.signal();
        myMutex.unlock();
        join();
    }

private:
    /// @brief the pool for this thread
    Pool& myPool;
    /// @brief the mutex for the task list
    FXMutex myMutex;
    /// @brief the semaphore when waiting for new tasks
    FXCondition myCondition;
    /// @brief the list of pending tasks
    std::list<Task*> myTasks;
    /// @brief whether we are still running
    bool myStopped;
    /// @brief counting completed tasks for debugging / profiling
    int myCounter;
};


#endif
