namespace GameOfLife.Scripts.Model
{
	public abstract class GoLRules : IAlgorithm
	{
		public abstract void EvalGrid(ref int[,] cells);

		public int EvalCell(int[,] board, int x, int y)
		{
			int width = board.GetLength(0);
			int height = board.GetLength(1);
			int neighbors = 0;

			for (int i = -1; i <= 1; i++)
			{
				for (int j = -1; j <= 1; j++)
				{
					int neighborX = (x + i + width) % width;
					int neighborY = (y + j + height) % height;
					neighbors += board[neighborX, neighborY];
				}
			}

			neighbors -= board[x, y];

			if (board[x, y] == 1 && neighbors < 2) return 0;
			if (board[x, y] == 1 && neighbors > 3) return 0;
			if (board[x, y] == 0 && neighbors == 3) return 1;
			return board[x, y];
		}
	}
}
