using GameOfLife.Scripts.Model;
using NUnit.Framework;
using System;
using System.Diagnostics;

namespace Benchmark
{
    [TestFixture]
	class BenchmarkProgram
	{
		[Test]
		public static void Benchmark()
		{
			 //var workerThrds = 0;
			 //var completionPortThrds = 0;
			 //ThreadPool.GetAvailableThreads(out workerThrds, out completionPortThrds);
			 //
			 //var threadnums = new int[] {  2, 4, 6, 8, 10, 12, 14, 16, completionPortThrds };
			 //var tasknums = new int[] { 2, 4, 6, 8, 10, 12, 14, 16, 20, 25, 30 };

			 //foreach (int threadnum in threadnums)
			 //{
             //   var seq1 = BenchmarkCheck(5, 100, 1000,
             //      new ParallelAlgorithm() { ParallelThreads = threadnum, Tasks = 12 });
             //   Console.WriteLine(seq1 + " at tasks " + 30 + " and threads " + threadnum);
             //}

            //foreach (int tasknum in tasknums)
            //{
            //    var seq1 = BenchmarkCheck(5, 100, 1000,
            //        new ParallelAlgorithm() { ParallelThreads = 2, Tasks = tasknum });
            //    Console.WriteLine(seq1 + " at tasks " + tasknum + " and threads " + 2);
            //}

            var sizes = new int[] { 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000 };
            foreach (int siz in sizes)
            {
            	var seq = BenchmarkCheck(5, 100, siz, new SequentialAlgorithm());
            	var parPool = BenchmarkCheck(5, 100, siz,
            		new ParallelAlgorithm() { Tasks = 12, ParallelThreads = 2 });
            	Console.WriteLine("Speedup is " + (seq / (double)parPool));
            }
        }

		private static long BenchmarkCheck(int iterations, int generations, int size, IAlgorithm rules)
		{
			GC.Collect();
			var ca = new CellularAutomaton(new int[size, size], rules);

			//починаємо відлік часу
            Stopwatch sw = Stopwatch.StartNew();
			for (int i = 0; i < iterations; i++)
			{
				ca.NextNSteps(generations);
			}
			sw.Stop();
			var averageTime = sw.ElapsedMilliseconds / iterations;

            Console.WriteLine($"Size: {size}*{size}, gens: {generations}, algo: {rules.GetType()} " + averageTime + " ms");
			return averageTime;
		}

	}

}
