namespace GameOfLife.Scripts.Model
{
	public class CellularAutomaton
	{
		public int GenerationNum;
		public IAlgorithm Algorithm { get; set; }
		public int[,] Cells;

		public readonly int Columns;
		public readonly int Rows;
		
		public CellularAutomaton(int[,] cells, IAlgorithm algo)
		{
			Cells = cells;
			GenerationNum = 0;
			Algorithm = algo;
			
			Columns = cells.GetLength(0);
			Rows = cells.GetLength(1);
		}

		public void NextStep()
		{
			Algorithm.EvalGrid(ref Cells);
			GenerationNum++;
		}

		public void NextNSteps(int n)
		{
			for (int i = 0; i < n; ++i)
			{
				Algorithm.EvalGrid(ref Cells);
			}
			GenerationNum += n;
		}
	}
}
