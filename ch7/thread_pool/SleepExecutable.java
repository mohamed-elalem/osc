import tpool.Executable;

import java.util.Random;

public class SleepExecutable implements Executable {
    @Override
    public void execute() {
        try {
            long sleeping = new Random().nextInt(10000);
            System.out.println("Sleeping for " + sleeping + " millis");
            Thread.sleep(sleeping);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}
