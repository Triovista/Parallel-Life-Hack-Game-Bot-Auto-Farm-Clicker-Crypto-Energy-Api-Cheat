using Godot;
using System.Collections.Generic;

public class GoLCell : Node2D
{
	private Dictionary<int, Color> _colorMap = new Dictionary<int, Color>
		{
			{ 0, Color.Color8(0, 0, 0) },
			{ 1, Color.Color8(0, 200, 0) }
		};
	
	private Vector2 _size;
	
	public int CellState { get; set; }

	public GoLCell(Vector2 size, int state, int x, int y)
	{
		_size = size;
		CellState = state;
	}

	public GoLCell()
	{
		_size = new Vector2(5, 5);
		CellState = 0;
		Position = new Vector2(10, 10);
	}
	
	public override void _Ready()
	{ }

	public override void _Draw()
	{
		DrawRect(new Rect2(Vector2.Zero, _size), _colorMap[CellState]);
	}
}
