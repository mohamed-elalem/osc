import tpool.ThreadPool;

public class Main {

    public static void main(String[] args) {
        ThreadPool threadPool = ThreadPool.getInstance();
        while(true) {
            new Thread() {
                public void run() {
                    SleepExecutable executable = new SleepExecutable();
                    threadPool.submit(executable);
                }
            }.start();
            try {
                Thread.sleep(50);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}
