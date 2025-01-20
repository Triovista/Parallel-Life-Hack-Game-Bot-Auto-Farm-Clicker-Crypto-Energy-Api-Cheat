namespace GameOfLife.Scripts.Model
{
    public class SequentialAlgorithm: GoLRules
	{
		public override void EvalGrid(ref int[,] cells)
		{
			var columns = cells.GetLength(0);
			var rows = cells.GetLength(1);

			var next = new int[columns, rows];

			for (int x = 0; x < columns; x++)
			{
				for (int y = 0; y < rows; y++)
				{
					next[x, y] = EvalCell(cells, x, y);
				}
			}
			cells = next;
		}

	}
}
