import io.threadcso._

object Main {
  def server(in: ?[Int], out: ![Int]) = proc {
    val t0 = System.nanoTime()
    var i = 0
    while (i < 20000) {
      in?()
      out!6
      i += 1
    }
    val t1 = System.nanoTime()
    println((t1 - t0) / 1000000000.0)
  }

  def client(in: ?[Int], out: ![Int]) = proc {
    val t0 = System.nanoTime()
    var i = 0
    while (i < 20000) {
      out!5
      in?()
      i += 1
    }
    val t1 = System.nanoTime()
    println((t1 - t0) / 1000000000.0)
  }

  def main(args: Array[String]) = {
    val a, b = OneOne[Int]
    (server(a, b) || client(b, a))()
  }
}
