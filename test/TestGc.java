import java.util.ArrayList;
import java.util.Collection;

public class TestGc {
  private static int MB = 1024 * 1024;
  private static int numThreads = 4;

  public static void main(String[] args) {
    Runnable r = new Runnable() {
      public void run() {
        for (int i = 0; i < 1; i++) {
          try {
            Integer[] data = new Integer[MB / numThreads / 4];
          } catch (OutOfMemoryError e) {
            System.out.println(e);
          }
        }
      }
    };

    Thread[] workers = new Thread[numThreads];
    for (int i = 0; i < numThreads; i++) {
      workers[i] = new Thread(r);
    }
    for (int i = 0; i < numThreads; i++) {
      workers[i].start();
    }
  }
}
