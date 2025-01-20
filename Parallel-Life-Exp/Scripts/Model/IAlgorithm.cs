namespace GameOfLife.Scripts.Model
{
    public interface IAlgorithm
    {
        void EvalGrid(ref int[,] cells);
    }
}
