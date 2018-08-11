package tpool;

import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantLock;

public class Worker implements Runnable, Taskable {

    private Executable executable;

    private final Thread THREAD;
    private ReentrantLock mutex;
    private Condition condition;
    private Condition poolCondition;

    public Worker(ReentrantLock mutex, Condition poolCondition) {
        THREAD = new Thread(this);
        this.mutex = mutex;
        this.condition = mutex.newCondition();
        this.poolCondition = poolCondition;
    }

    @Override
    public synchronized void setExecutable(Executable executable) {
        this.executable = executable;
    }

    @Override
    public void run() {
        while(true) {
            mutex.lock();
            try {
                condition.await();
            } catch(InterruptedException ex) {
                System.out.println("Exception occurred: " + ex.getMessage());
            }
            finally {
                mutex.unlock();
            }

            executable.execute();

            mutex.lock();
            try {
                poolCondition.signal();
            } finally {
                mutex.unlock();
            }
        }
    }

    public long getId() {
        return THREAD.getId();
    }

    public void start() throws InterruptedException {
        THREAD.start();
    }

    public void notifyWorker() {
        mutex.lock();
        try {
            condition.signal();
        } catch(Exception ex) {
            System.out.println("Exception occurred: " + ex.getMessage());
        } finally {
            mutex.unlock();
        }
    }
}
