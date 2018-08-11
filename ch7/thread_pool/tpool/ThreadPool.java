package tpool;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.Queue;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantLock;

public class ThreadPool {
    private static ThreadPool instance;

    private final static ReentrantLock MUTEX = new ReentrantLock();
    private final static Condition INIT_CONDITION = MUTEX.newCondition();
    private final Condition poolCondition = MUTEX.newCondition();
    private Queue<Long> availableThreads;
    private HashMap<Long, Worker> pool;
    private HashMap<Long, Condition> workerCondition;
    private int poolSize = 50;
    private static boolean initializing;
    private boolean poolConditionUsed;


    public static ThreadPool getInstance() {
        MUTEX.lock();
        try {
            if(initializing) {
               INIT_CONDITION.await();
            }
            initializing = true;

            if (instance == null) {
                instance = new ThreadPool();
            }

            INIT_CONDITION.signal();
        } catch(InterruptedException ex) {
        } finally {
            MUTEX.unlock();
        }

        return instance;
    }

    public void submit(Executable executable) {
        try {
            MUTEX.lock();
            long workerId = -1;
            try {
                System.out.println("Trying to submit worker");
                if (availableThreads.isEmpty()) {
                    System.out.println("Waiting until a worker is available");
                    poolCondition.await();
                    poolConditionUsed = true;
                    System.out.println("Worker became available");
                }
                System.out.println("Worker is ready");
                workerId = availableThreads.poll();
            } catch(InterruptedException ex) {
                System.out.println("Exception occurred: " + ex.getMessage());
            } finally {
                MUTEX.unlock();
            }
            Worker worker = pool.get(workerId);
            Condition activeWorkerCondition = workerCondition.get(workerId);
            worker.setExecutable(executable);
            System.out.println("Starting worker with id " + workerId);
            worker.notifyWorker();
            MUTEX.lock();

            try {
                activeWorkerCondition.await();
            } catch(InterruptedException ex) {
                System.out.println("Exception occurred: " + ex.getMessage());
            } finally {
                MUTEX.unlock();
            }
            System.out.println("Worker Finished Executing");
            MUTEX.lock();

            try {
                availableThreads.add(workerId);
            } catch(Exception ex) {
                System.out.println("Exception occurred: " + ex.getMessage());
            } finally {
                MUTEX.unlock();
            }

        } catch(Exception ex) {
            System.out.println("Exception occurred: " + ex.getMessage());
        } finally {
            if (poolConditionUsed) {
                poolConditionUsed = false;
                poolCondition.signal();
            }
        }
    }

    private ThreadPool() {
        MUTEX.lock();
        availableThreads = new LinkedList<>();
        pool = new HashMap<>();
        workerCondition = new HashMap<>();
        try {
            for(int i = 0; i < poolSize; i++) {
                Condition condition = MUTEX.newCondition();
                Worker worker = new Worker(MUTEX, condition);
                availableThreads.add(worker.getId());
                pool.put(worker.getId(), worker);
                workerCondition.put(worker.getId(), condition);
                worker.start();
                System.out.println("Started Worker with id " + worker.getId());
            }
        } catch(Exception ex) {
            System.err.println("Exception Occured: " + ex.getMessage());
        } finally {
            MUTEX.unlock();
        }
    }
}
