using GameOfLife.Scripts.Model;
using Godot;
using System;
public class GoLBoard : Node2D
{
	private CellularAutomaton cellularAutomaton;
	[Export] public int BoardX { get; set; }
	[Export] public int BoardY { get; set; }
	[Export] public int CellSize { get; set; }
	[Export] public int CellBorder { get; set; }
	[Export] public float Delay = 0.5f;

	private GoLCell[,] cells;
	private GoLCell previousCell = null;
	public bool IsPaused = true;
	public bool IsSequential = true;
	private float animTimer = 0f;
	public int GenerationsPerTick = 1;
	private IAlgorithm rules;
	
	private Button playButton;
	private Button algoButton;

	public override void _Ready()
	{
		playButton = GetNode<Button>($"../Controls/PlayButton");
		algoButton = GetNode<Button>($"../Controls/AlgoButton");

		cellularAutomaton = new CellularAutomaton(new int[BoardX,BoardY], new SequentialAlgorithm());
		var tileSizeVector = new Vector2(CellSize, CellSize);
		cells = new GoLCell[BoardX, BoardY];
		for (int i = 0; i < BoardX; ++i)
		{
			for (int j = 0; j < BoardY; j++)
			{
				var cell = new GoLCell(tileSizeVector, 0, i, j);
				cell.Position = new Vector2(i * (CellSize + CellBorder) + CellBorder, j * (CellSize + CellBorder) + CellBorder);
				cells[i, j] = cell;
				AddChild(cell);
			}
		}
	}

	private void UpdateBoard(int steps)
	{
		cellularAutomaton.NextNSteps(steps);
		for (int i = 0; i < BoardX; i++)
		{
			for (int j = 0; j < BoardY; j++)
			{
				cells[i, j].CellState = cellularAutomaton.Cells[i, j];
				cells[i, j].Update();
			}
		}
	}

	public override void _Input(InputEvent @event)
	{
		if (@event is InputEventMouseMotion mmEvent)
		{
			if (mmEvent.ButtonMask == (int)ButtonList.Left)
			{
				var startX = GlobalPosition.x;
				var startY = GlobalPosition.y;
				var mx = mmEvent.GlobalPosition.x;
				var my = mmEvent.GlobalPosition.y;
				var relativeX = mx - startX - CellBorder;
				var relativeY = my - startY - CellBorder;
				var x = (int)relativeX / (CellBorder + CellSize);
				var y = (int)relativeY / (CellBorder + CellSize);

				if (x >= 0 && x < BoardX && y >= 0 && y < BoardY)
				{
					var cell = cells[x, y];
					if (cell != previousCell)
					{
						var c = cell.CellState == 0 ? 1 : 0;
						cell.CellState = c;
						cellularAutomaton.Cells[x, y] = c;
						GD.Print("X: ", x, "; Y: ", y, "; val: ", cellularAutomaton.Cells[x, y]);
						cell.Update();
						previousCell = cell;
					}
				}
			}
		}
		else if (@event is InputEventMouseButton mbEvent)
		{
			if (mbEvent.ButtonIndex == (int)ButtonList.Left && mbEvent.Pressed)
			{
				var startX = GlobalPosition.x;
				var startY = GlobalPosition.y;
				var mx = mbEvent.GlobalPosition.x;
				var my = mbEvent.GlobalPosition.y;
				var relativeX = mx - startX - CellBorder;
				var relativeY = my - startY - CellBorder;
				var x = (int)relativeX / (CellBorder + CellSize);
				var y = (int)relativeY / (CellBorder + CellSize);

				if (x >= 0 && x < BoardX && y >= 0 && y < BoardY)
				{
					var cell = cells[x, y];
					var c = cell.CellState == 0 ? 1 : 0;
					cell.CellState = c;
					cellularAutomaton.Cells[x, y] = c;
					GD.Print("X: ", x, "; Y: ", y, "; val: ", cellularAutomaton.Cells[x, y]);
					cell.Update();
					previousCell = cell;
				}
			}
		}
	}

	//  Called every frame. 'delta' is the elapsed time since the previous frame.
	
	public override void _Process(float delta)
	{
	  if (IsPaused) return;
	  if (Delay < delta)
	  {
		  GD.Print("The delta is " + delta + " sec, needed delay: " + 
			  Delay + " sec, current algorithm: " + cellularAutomaton.Algorithm.GetType());
	  }
	  if (animTimer <= 0f)
	  {
		  UpdateBoard(GenerationsPerTick);
		  animTimer = Delay;
		  return;
	  }
	  animTimer -= delta;
	}

	public void PauseGBoard()
	{
		IsPaused = !IsPaused;

		if (IsPaused)
		{
			playButton.Text = "Paused";
		}
		else
		{
			playButton.Text = "Playing";
		}
	}
	
	public void ChangeAlgo()
	{
		IsSequential = !IsSequential;
		if (IsSequential)
		{
			cellularAutomaton.Algorithm = new SequentialAlgorithm();
			algoButton.Text = "Sequential";
		}
		else
		{
			cellularAutomaton.Algorithm = new ParallelAlgorithm();
			algoButton.Text = "Parallel";
		}
		
	}

	public void RandomizeBoard()
	{
		Random rand = new Random();

		for (int i = 0; i < BoardX; i++)
		{
			for (int j = 0; j < BoardY; j++)
			{
				int randomState = rand.Next(0, 2);
				cells[i, j].CellState = randomState;
				cellularAutomaton.Cells[i, j] = randomState;
				cells[i, j].Update();
			}
		}
	}
}
