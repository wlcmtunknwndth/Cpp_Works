#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

using std::cout;
using std::cin;
using std::endl;

const char *data_file = "test1.txt";

enum Direction { forward, right, back, left, none };
enum Cell_type { unknown_type, free_cell, start, finish, pawn };
enum Line { row, colomn };

struct Position
{
	int x;
	int y;

	Position() : x(0), y(0) { }
	Position(int x, int y) : x(x), y(y) { };
};

// ������ �����
struct Cell
{
	Cell_type type;				// ���������� ��� ��������� �� ������ (����� ������� ������� �����)
	Position position;			// �������������� �� ����� (� �������)
	int shortest_movement = -1;
	int row_massive_number = -1;
	int colomn_massive_number = -1;

	Cell() : type(unknown_type), position(-1, -1) { }
	Cell(Cell_type type, Position position) : type(type), position(position) { }
};

class Groups
{
private:
	void CountUp_massives_in_rows()
	{
		Cell *cell;

		for (int c = 0; c < colomns; c++)
		{
			bool cell_is_engaged = true;

			for (int r = 0; r < rows; r++)
			{
				cell = &board[r][c];

				if (cell->type != pawn)
				{
					if (cell_is_engaged)
					{
						massives_in_rows[c]++;
						cell_is_engaged = false;
					}
				}
				else
				if (!cell_is_engaged) cell_is_engaged = true;
			}
		}
	}
	void CountUp_massives_in_colomns()
	{
		Cell *cell;

		for (int r = 0; r < rows; r++)
		{
			bool cell_is_engaged = true;

			for (int c = 0; c < colomns; c++)
			{
				cell = &board[r][c];

				if (cell->type != pawn)
				{
					if (cell_is_engaged)
					{
						massives_in_colomns[r]++;
						cell_is_engaged = false;
					}
				}
				else
				if (!cell_is_engaged) cell_is_engaged = true;
			}
		}
	}

	void GiveMemoryTo_cells_in_massives_in_rows()
	{
		cells_in_massives_in_rows = new int*[colomns];
		for (int i = 0; i < colomns; i++)
			cells_in_massives_in_rows[i] = new int[massives_in_rows[i]];
	}
	void GiveMemoryTo_cells_in_massives_in_colomns()
	{
		cells_in_massives_in_colomns = new int*[rows];
		for (int i = 0; i < rows; i++)
			cells_in_massives_in_colomns[i] = new int[massives_in_colomns[i]];
	}

	void CountUp_cells_in_massives_in_rows()
	{
		Cell *cell;
		int cell_amount;
		int massive_number;
		bool cell_is_engaged;

		for (int i = 0; i < colomns; i++)
		{
			cell_amount = 0;
			massive_number = 0;
			cell_is_engaged = false;

			for (int k = 0; k < rows; k++)
			{
				cell = &board[k][i];

				if (cell->type == pawn)
				{
					if (cell_amount > 0)
					{
						cells_in_massives_in_rows[i][massive_number] = cell_amount;
						cell_amount = 0;
						massive_number++;
					}
				}
				else
				{
					cell_amount++;
					if (cell_is_engaged) cell_is_engaged = false;
				}
			}

			if (cell_amount > 0)
				cells_in_massives_in_rows[i][massive_number] = cell_amount;
		}
	}
	void CountUp_cells_in_massives_in_colomns()
	{
		Cell *cell;
		int cell_amount;
		int massive_number;
		bool cell_is_engaged;

		for (int i = 0; i < rows; i++)
		{
			cell_amount = 0;
			massive_number = 0;
			cell_is_engaged = false;

			for (int k = 0; k < colomns; k++)
			{
				cell = &board[i][k];

				if (cell->type == pawn)
				{
					if (cell_amount > 0)
					{
						cells_in_massives_in_colomns[i][massive_number] = cell_amount;
						cell_amount = 0;
						massive_number++;
					}
				}
				else
				{
					cell_amount++;
					if (cell_is_engaged) cell_is_engaged = false;
				}
			}

			if (cell_amount > 0)
				cells_in_massives_in_colomns[i][massive_number] = cell_amount;
		}
	}

	void GiveMemoryTo_row_groups()
	{
		int massives_quantity;
		int cells_quantity;

		row_groups = new Cell***[colomns];

		for (int i = 0; i < colomns; i++)
		{
			massives_quantity = massives_in_rows[i];
			row_groups[i] = new Cell **[massives_quantity];

			for (int k = 0; k < massives_quantity; k++)
			{
				cells_quantity = cells_in_massives_in_rows[i][k];
				row_groups[i][k] = new Cell *[cells_quantity];
			}
		}
	}
	void GiveMemoryTo_colomn_groups()
	{
		int massives_quantity;
		int cells_quantity;

		colomn_groups = new Cell***[rows];

		for (int i = 0; i < rows; i++)
		{
			massives_quantity = massives_in_colomns[i];
			colomn_groups[i] = new Cell **[massives_quantity];

			for (int k = 0; k < massives_quantity; k++)
			{
				cells_quantity = cells_in_massives_in_colomns[i][k];
				colomn_groups[i][k] = new Cell *[cells_quantity];
			}
		}
	}

	void Fill_row_groups()
	{
		Cell *cell;
		int cell_amount;
		int massive_number;
		bool cell_is_engaged;

		for (int i = 0; i < colomns; i++)
		{
			cell_amount = 0;
			massive_number = 0;
			cell_is_engaged = false;

			for (int k = 0; k < rows; k++)
			{
				cell = &board[k][i];

				if (cell->type == pawn)
				{
					if (cell_amount > 0)
					{
						cell_amount = 0;
						massive_number++;
					}
				}
				else
				{
					row_groups[i][massive_number][cell_amount] = cell;
					cell_amount++;
					if (cell_is_engaged) cell_is_engaged = false;
				}
			}
		}
	}
	void Fill_colomn_groups()
	{
		Cell *cell;
		int cell_amount;
		int massive_number;
		bool cell_is_engaged;

		for (int i = 0; i < rows; i++)
		{
			cell_amount = 0;
			massive_number = 0;
			cell_is_engaged = false;

			for (int k = 0; k < colomns; k++)
			{
				cell = &board[i][k];

				if (cell->type == pawn)
				{
					if (cell_amount > 0)
					{
						cell_amount = 0;
						massive_number++;
					}
				}
				else
				{
					colomn_groups[i][massive_number][cell_amount] = cell;
					cell_amount++;
					if (cell_is_engaged) cell_is_engaged = false;
				}
			}
		}
	}

	void SetMassiveNumber_InCells()
	{
		int massives_quantity;
		int cells_quantity;

		// ��� ����� ��������
		for (int i = 0; i < rows; i++)
		{
			massives_quantity = massives_in_colomns[i];

			for (int k = 0; k < massives_quantity; k++)
			{
				cells_quantity = cells_in_massives_in_colomns[i][k];

				for (int n = 0; n < cells_quantity; n++)
					colomn_groups[i][k][n]->colomn_massive_number = k;
			}
		}

		// ��� ����� �����
		for (int i = 0; i < colomns; i++)
		{
			massives_quantity = massives_in_rows[i];

			for (int k = 0; k < massives_quantity; k++)
			{
				cells_quantity = cells_in_massives_in_rows[i][k];

				for (int n = 0; n < cells_quantity; n++)
					row_groups[i][k][n]->row_massive_number = k;
			}
		}
	}

	void ShowMassivesInRows()
	{
		cout << "massives in rows:";
		for (int i = 0; i < colomns; i++)
			cout << endl << ' ' << i << ": " << massives_in_rows[i];

		cout << endl;
	}
	void ShowMassivesInColomns()
	{
		cout << "massives in colomns:";
		for (int i = 0; i < rows; i++)
			cout << endl << ' ' << i << ": " << massives_in_colomns[i];

		cout << endl;
	}

	void ShowCellsInMassivesInRows()
	{
		int massives_quantity;
		cout << "cells in massives in rows" << endl;

		for (int i = 0; i < colomns; i++)
		{
			massives_quantity = massives_in_rows[i];
			cout << "row " << i << ": " << massives_quantity << endl;

			for (int k = 0; k < massives_quantity; k++)
				cout << "  in massive " << k << ": " << cells_in_massives_in_rows[i][k] << " cells" << endl;
		}
	}
	void ShowRowGroups()
	{
		int massives_quantity;
		int cells_quantity;

		cout << "_ _ Row Groups _ _" << endl;
		for (int i = 0; i < colomns; i++)
		{
			massives_quantity = massives_in_rows[i];
			cout << endl << "row " << i << ": " << endl;

			for (int k = 0; k < massives_quantity; k++)
			{
				cells_quantity = cells_in_massives_in_rows[i][k];
				cout << "  massive " << k << ": " << endl;

				for (int n = 0; n < cells_quantity; n++)
					cout << "    cell " << n << " = " << row_groups[i][k][n]->type << endl;
			}
		}
	}

	void ShowCellsInMassivesInColomns()
	{
		int massives_quantity;
		cout << "cells in massives in colomns" << endl;

		for (int i = 0; i < rows; i++)
		{
			massives_quantity = massives_in_colomns[i];
			cout << "colomn " << i << ": " << massives_quantity << endl;

			for (int k = 0; k < massives_quantity; k++)
				cout << "  in massive " << k << ": " << cells_in_massives_in_colomns[i][k] << " cells" << endl;
		}
	}
	void ShowColomnGroups()
	{
		int massives_quantity;
		int cells_quantity;

		cout << "_ _ Colomn Groups _ _" << endl;
		for (int i = 0; i < rows; i++)
		{
			massives_quantity = massives_in_colomns[i];
			cout << endl << "colomn " << i << ": " << endl;

			for (int k = 0; k < massives_quantity; k++)
			{
				cells_quantity = cells_in_massives_in_colomns[i][k];
				cout << "  massive " << k << ": " << endl;

				for (int n = 0; n < cells_quantity; n++)
					cout << "    cell " << n << " = " << colomn_groups[i][k][n]->type << endl;
			}
		}
	}

	void ShowMassiveNumber_InCells()
	{
		int massives_quantity;
		int cells_quantity;
		Cell *cell;

		// ��� ����� ��������
		cout << "colomn massive's numbers in cells" << endl;
		for (int i = 0; i < rows; i++)
		{
			massives_quantity = massives_in_colomns[i];
			cout << "colomn " << i << endl;

			for (int k = 0; k < massives_quantity; k++)
			{
				cells_quantity = cells_in_massives_in_colomns[i][k];
				cout << "  massive number " << k << endl;

				for (int n = 0; n < cells_quantity; n++)
				{
					cell = colomn_groups[i][k][n];
					cout << "    cell " << n << " - position[" << cell->position.x << ',' << cell->position.y 
						<< "] row=" << cell->row_massive_number << " col=" << cell->colomn_massive_number << endl;
				}
			}
		}

		cout << endl;

		// ��� ����� �����
		cout << "row massive's numbers in cells" << endl;
		for (int i = 0; i < colomns; i++)
		{
			massives_quantity = massives_in_rows[i];
			cout << "row " << i << endl;

			for (int k = 0; k < massives_quantity; k++)
			{
				cells_quantity = cells_in_massives_in_rows[i][k];
				cout << "  massive number " << k << endl;

				for (int n = 0; n < cells_quantity; n++)
				{
					cell = row_groups[i][k][n];
					cout << "    cell " << n << " - position[" << cell->position.x << ',' << cell->position.y
						<< "] row=" << cell->row_massive_number << " col=" << cell->colomn_massive_number << endl;
				}
			}
		}
	}

	Cell ** GetRowGroup(Cell *cell)
	{
		if (cell->row_massive_number != -1)
			return row_groups[cell->position.y][cell->row_massive_number];
		else
			return nullptr;
	}

	Cell ** GetColomnGroup(Cell *cell)
	{
		if (cell->colomn_massive_number == -1)
			return nullptr;
		else
			return colomn_groups[cell->position.x][cell->colomn_massive_number];
	}

	//cell - ����� ������ �� ������
	int GetRowGroupLength(Cell *cell)
	{
		if (cell->row_massive_number == -1)
			return 0;
		else
			return cells_in_massives_in_rows[cell->position.y][cell->row_massive_number];
	}

	//cell - ����� ������ �� ������
	int GetColomnGroupLength(Cell *cell)
	{
		if (cell->colomn_massive_number == -1)
			return 0;
		else
			return cells_in_massives_in_colomns[cell->position.x][cell->colomn_massive_number];
	}

public:
	int rows;
	int colomns;
	int *massives_in_rows;
	int *massives_in_colomns;
	int **cells_in_massives_in_rows;
	int **cells_in_massives_in_colomns;

	int current_movement = 0;

	Cell ****row_groups;
	Cell ****colomn_groups;
	Cell **board;

	Groups() { }
	Groups(int rows, int colomns, Cell **board)
	{
		this->rows = rows;
		this->colomns = colomns;
		this->board = board;

		massives_in_rows = new int[colomns];
		massives_in_colomns = new int[rows];

		for (int i = 0; i < colomns; i++)
			massives_in_rows[i] = 0;

		for (int i = 0; i < rows; i++)
			massives_in_colomns[i] = 0;
	}

	void DistinguishGoups()
	{
		row_groups = new Cell ***[colomns];
		colomn_groups = new Cell ***[rows];

		CountUp_massives_in_rows();
		CountUp_massives_in_colomns();

		GiveMemoryTo_cells_in_massives_in_rows();
		GiveMemoryTo_cells_in_massives_in_colomns();

		CountUp_cells_in_massives_in_rows();
		CountUp_cells_in_massives_in_colomns();

		GiveMemoryTo_row_groups();
		GiveMemoryTo_colomn_groups();

		Fill_row_groups();
		Fill_colomn_groups();

		SetMassiveNumber_InCells();
	}

	void ShowGroups()
	{
		cout << endl;
		//ShowMassivesInRows();
		cout << endl;
		ShowMassivesInColomns();
		cout << endl;
		//ShowCellsInMassivesInRows();
		cout << endl;
		ShowCellsInMassivesInColomns();
		cout << endl;
		cout << endl;
		//ShowRowGroups();
		cout << endl;
		cout << endl;
		ShowColomnGroups();
		cout << endl;
		ShowMassiveNumber_InCells();
	}

	class CellList
	{
	public:
		Cell *cell = nullptr;
		CellList *next_cell = nullptr;
		bool changed = false;

		CellList() { }

		CellList(Cell *cell)
		{
			this->cell = cell;
		}
	};

	CellList *row_cells = nullptr;
	CellList *colomn_cells = nullptr;
	int row_cells_length;
	int colomn_cells_length;

	void FindShortestWay(Cell *start_cell, Cell *finish_cell)
	{
		if (colomns + rows > 600)
			cout << "���� ���� ������ ��� 300 �� 300, �� �������� ����� ���� ����������." << endl;

		row_cells = new CellList(start_cell);
		colomn_cells = new CellList(start_cell);

		while (SelectedCells_IsExist())
		{
			current_movement++;
			Spread_Greed();

			if (finish_cell->shortest_movement != -1)
				break;
		}
	}

	void Spread_Greed()
	{
		SelectCells();													// �������� ������ � ������� ������� ����������� ��������� ������
		SetShortestMovements_And_ThrowOut_NotChangedCells(row_cells);	// �������� ���. ����������� �� ������� ���� ������� ������ ������������ � �������� ����� ������ ��� ����������
		SetShortestMovements_And_ThrowOut_NotChangedCells(colomn_cells);
	}

	void SelectCells()
	{
		CellList *new_row_cells = SelectRowCells();
		CellList *new_colomn_cells = SelectColomnCells();

		FreeCellList(row_cells);
		FreeCellList(colomn_cells);

		row_cells = new_colomn_cells;
		colomn_cells = new_row_cells;
	}

	void FreeCellList(CellList *&cell_list)
	{
		CellList *next_cell;
		while (cell_list != nullptr)
		{
			next_cell = cell_list->next_cell;
			delete cell_list;
			cell_list = next_cell;
		}
	}

	CellList *SelectRowCells()
	{
		CellList *new_cell_list = nullptr;
		CellList *last_cell = nullptr;
		Cell **cell_mass;
		int size;

		while (row_cells != nullptr)
		{
			if (row_cells->cell->row_massive_number == -1)
			{
				row_cells = row_cells->next_cell;
				continue;
			}

			cell_mass = GetRowGroup(row_cells->cell);

			if (cell_mass == nullptr)
				continue;

			size = GetRowGroupLength(row_cells->cell);

			for (int i = 0; i < size; i++)
			{
				if (new_cell_list == nullptr)
				{
					new_cell_list = new CellList;
					new_cell_list->cell = cell_mass[i];
					last_cell = new_cell_list;
				}
				else
				{
					last_cell->next_cell = new CellList(cell_mass[i]);
					last_cell = last_cell->next_cell;
				}
			}

			row_cells = row_cells->next_cell;
		}
		return new_cell_list;
	}

	CellList *SelectColomnCells()
	{
		CellList *new_cell_list = nullptr;
		CellList *last_cell = nullptr;
		Cell **cell_mass;
		int size;

		while (colomn_cells != nullptr)
		{
			if (colomn_cells->cell->colomn_massive_number == -1)
			{
				colomn_cells = colomn_cells->next_cell;
				continue;
			}

			cell_mass = GetColomnGroup(colomn_cells->cell);

			if (cell_mass == nullptr)
				continue;

			size = GetColomnGroupLength(colomn_cells->cell);

			for (int i = 0; i < size; i++)
			{
				if (new_cell_list == nullptr)
				{
					new_cell_list = new CellList;
					new_cell_list->cell = cell_mass[i];
					last_cell = new_cell_list;
				}
				else
				{
					last_cell->next_cell = new CellList(cell_mass[i]);
					last_cell = last_cell->next_cell;
				}
			}

			colomn_cells = colomn_cells->next_cell;
		}
		return new_cell_list;
	}

	bool SelectedCells_IsExist()
	{
		return row_cells != nullptr || colomn_cells != nullptr;
	}

	void SetShortestMovements_And_ThrowOut_NotChangedCells(CellList *&cell_list)
	{
		CellList *prev_cell_list = nullptr;
		CellList *current_cell_list = cell_list;

		while (current_cell_list != nullptr)
		{
			if (current_cell_list->cell->shortest_movement > current_movement ||
				current_cell_list->cell->shortest_movement == -1)
			{
				current_cell_list->cell->shortest_movement = current_movement;
				prev_cell_list = current_cell_list;
				current_cell_list = current_cell_list->next_cell;
			}
			else
			{
				if (prev_cell_list == nullptr)
				{
					cell_list = current_cell_list->next_cell;
					delete current_cell_list;
					current_cell_list = cell_list;
				}
				else
				{
					prev_cell_list->next_cell = current_cell_list->next_cell;
					delete current_cell_list;
					current_cell_list = prev_cell_list->next_cell;
				}
			}
		}
	}

	void ShowCellList(CellList *cell_list)
	{
		int i = 0;
		CellList *current_cell = cell_list;

		while (current_cell != nullptr)
		{
			cout << current_cell->cell->position.x << ' ' << current_cell->cell->position.y << " (" << current_cell->cell->shortest_movement << ')' << endl;
			i++;
			current_cell = current_cell->next_cell;
		}
		cout << "����� �����: " << i << endl;
	}

	void ShowSelectedCells()
	{
		cout << "row cells:" << endl;
		ShowCellList(row_cells);

		cout << endl;
		cout << "colomn cells:" << endl;
		ShowCellList(colomn_cells);
	}
};

// ����� � ��������
class Board
{
private:
	enum read_errors
	{
		no_error,
		bad_rows,
		bad_colomns,
		cant_open_file,
		bad_board,
		unknown_cell_type,
		lack_rows_or_colomns,
		excess_rows_or_colomns,
		start_already_exist,
		finish_already_exist,
		start_cell_not_exist,
		finish_cell_not_exist
	};

	int rows;								// ����� ����� �����
	int colomns;							// ����� �������� �����
	Cell **board;							// ����� � ��������
	read_errors read_error = no_error;		// ������ ��� ������ �������� ������
	int movement;
	int shortest_way;

	// ������������� ������ ����� �������� ������ � ��� ������ (��� � ��� ���������)
	//		ch - ������������� ������	
	Cell_type CharToCellType(char ch)
	{
		switch (ch)
		{
		case '.': return free_cell;
		case 'P': return pawn;
		case 'S': return start;
		case 'F': return finish;
		default: return unknown_type;
		}
	}

	char CellToShortestMovement(Cell cell)
	{
		switch (cell.type)
		{
		case free_cell:
			if (cell.shortest_movement == -1)
				return '-';
			else
				return 48 + cell.shortest_movement;

		case pawn: return 'x';
		case start:	return 'S';
		case finish: return 'F';
		default: return 'W';
		}
	}

public:
	Groups groups;
	Cell *current_cell = nullptr;						// ������� ������ �����
	Cell *start_cell = nullptr;						// ������ ������
	Cell *finish_cell = nullptr;

	void InitGroups()
	{
		groups = Groups(rows, colomns, board);
	}

	Board() : rows(0), colomns(0), current_cell(nullptr), movement(0), shortest_way(-1) { }

	// ��������� �������� ������ ������
	//		data_file - ��� ����� � ��������� �������
	int LoadData(const char *data_file)
	{
		std::ifstream file(data_file);
		try
		{
			// ��������� ����
			if (!file)
				throw cant_open_file;

			// ��������� ���-�� ��������
			file >> rows;
			if (!file)
				throw bad_colomns;
			if (rows > 500000 || rows < 1)
				throw bad_colomns;

			// ��������� ���-�� �����
			file >> colomns;
			if (!file)
				throw bad_rows;
			if (colomns > 500000 || colomns < 1)
				throw bad_rows;

			board = new Cell *[rows];
			for (int row = 0; row < rows; row++)
			{
				board[row] = new Cell[colomns];
			}

			bool finishIsExist = false;
			bool startIsExist = false;
			char ch;
			Cell_type cell_type = Cell_type::unknown_type;
			int r = 0, c = 0;
			// ��������� ������ �����
			for (r = 0; r < rows; r++)
			{
				ch = file.get();
				if (ch != '\n')
				{
					//ShowRowAndColomn(c, r);
					cout << "������ " << r << ", ������� " << c << endl;
					throw excess_rows_or_colomns;
				}

				for (c = 0; c < colomns; c++)
				{
					ch = file.get();

					if (!file)
					{
						cout << "������ " << r + 1 << ", ������� " << c + 1 << endl;
						throw bad_board;
					}

					cell_type = CharToCellType(ch);
					if (cell_type == Cell_type::unknown_type)
					{
						cout << "������ " << r + 1 << ", ������� " << c + 1 << endl;
						throw unknown_cell_type;
					}

					// �������� ���������� ������
					if (cell_type == Cell_type::finish)
					{
						if (finishIsExist)
						{
							cout << "������ " << r + 1 << ", ������� " << c + 1 << endl;
							throw finish_already_exist;
						}
						else
						{
							finishIsExist = true;
							finish_cell = &board[r][c];
						}
					}

					board[r][c] = { cell_type, { c, r } };

					// �������� ���������� ������ �����
					if (cell_type == Cell_type::start)
					{

						if (startIsExist)
						{
							cout << "������ " << r + 1 << ", ������� " << c + 1 << endl;
							throw start_already_exist;
						}
						else
						{
							current_cell = &board[r][c];
							current_cell->shortest_movement = 0;
							start_cell = current_cell;
							startIsExist = true;
						}
					}
				}	// end for (c = 0; c < colomns; c++)
			}		// end for (r = 0; r < rows; r++)

			// �������� ������������ ���������� � ��������� ����� � ��������
			if (r < rows - 1 || c < colomns - 1)
			{
				cout << "�������� " << rows << " ����� � " << colomns << " ��������." << endl
					<< "� ������� " << rows << " ����� � " << colomns << " ��������.";
				throw lack_rows_or_colomns;
			}

			// ��������� ���� �� ����� ��������� �������� ��� (����� ��������� ������, �������� � ���������)
			ch = file.get();
			while (ch == '\n' || ch == ' ' || ch == '\t')
				ch = file.get();
			if (!file.eof())
			{
				cout << "������ " << r + 1 << ", ������� " << c + 1 << endl;
				throw excess_rows_or_colomns;
			}

			// �������� ������� ������
			if (start_cell == nullptr)
				throw start_cell_not_exist;

			// �������� ������� ������
			if (finish_cell == nullptr)
				throw finish_cell_not_exist;
			
		}
		catch (read_errors error)
		{
			switch (error)
			{
			case cant_open_file: cout << "������. �� �������� ������� ���� " << data_file << endl; break;
			case bad_rows: cout << "������. �� ������ ����� ����� � ����� " << data_file << endl; break;
			case bad_colomns: cout << "������. �� ������ ����� �������� � ����� " << data_file << endl; break;
			case bad_board: cout << "������ ������ ������ ����� �� ����� " << data_file << endl; break;
			case unknown_cell_type: cout << "������. �� ������������ ��� ������." << endl; break;
			case lack_rows_or_colomns: break;
			case excess_rows_or_colomns: cout << "������. � ����� " << data_file << " ������������ ������ ������ ��� �������." << endl; break;
			case start_already_exist: cout << "������. � ����� ����� ����� ��������� ������� �����." << endl; break;
			case finish_already_exist: cout << "������. � ����� ������ ������ ������." << endl; break;
			case start_cell_not_exist: cout << "������. ����������� ������ ������." << endl; break;
			case finish_cell_not_exist: cout << "������. ����������� ������ ������." << endl; break;
			default: cout << "����������� ������" << endl; break;
			}

			read_error = error;
			return 1;
		}

		file.close();
		//cout << "������ ����� " << data_file << " ������� ��������." << endl;
		return 0;
	}

	void InvertBoard()
	{
		Cell **new_board;

		new_board = new Cell *[colomns];
		for (int colomn = 0; colomn < colomns; colomn++)
		{
			new_board[colomn] = new Cell[rows];
		}

		for (int c = 0; c < colomns; c++)
		for (int r = 0; r < rows; r++)
		{
			new_board[c][r] = board[r][c];

			if (board[r][c].position.y == start_cell->position.y &&
				board[r][c].position.x == start_cell->position.x)
				start_cell = &new_board[c][r];

			if (board[r][c].position.y == finish_cell->position.y &&
				board[r][c].position.x == finish_cell->position.x)
				finish_cell = &new_board[c][r];
		}

		DeleteBoard();

		int tmp_rows = rows;
		rows = colomns;
		colomns = tmp_rows;
		board = new_board;
	}

	void DeleteBoard()
	{
		for (int r = 0; r < rows; r++)
			delete []board[r];

		delete []board;
	}

	bool CellBelongsMass(Position position)
	{
		if (position.x >= rows || position.x < 0 ||
			position.y >= colomns || position.y < 0)
			return false;
		else
			return true;
	}

	bool CellHaveNoPawn(Cell *cell)
	{
		return (cell->type != pawn) ? true : false;
	}

	void ShowShortestMovements()
	{
		for (int c = 0; c < colomns; c++)
		{
			cout << endl << "  ";

			for (int r = 0; r < rows; r++)
			if (current_cell->position.x == r && current_cell->position.y == c)
				cout << 'S';
			else
				cout << CellToShortestMovement(board[r][c]);
		}
		cout << endl;
	}

	void ShowCurrentInfo(int step_count)
	{
		cout << "������� ������� " << current_cell->position.x << "  " << current_cell->position.y << endl;
		cout << "��� " << step_count << endl;
		cout << "����������� ����: " << shortest_way << endl;
		cout << endl;
	}

	// �������� �����
	void ShowBoard()
	{
		for (int r = 0; r < rows; r++)
		{
			cout << endl << "  ";

			for (int c = 0; c < colomns; c++)
				cout << CellToChar(board[r][c]);
		}
		cout << endl;
	}

	void ShowInvertedBoard()
	{
		for (int r = 0; r < rows; r++)
		{
			cout << endl << "  ";

			for (int c = 0; c < colomns; c++)
				cout << CellToChar(board[r][c]);
		}
	}

	char CellToChar(Cell cell)
	{
		switch (cell.type)
		{
		case Cell_type::finish: return 'F';
		case Cell_type::free_cell: return '.';
		case Cell_type::pawn: return 'P';
		case Cell_type::start: return 'S';

		}
	}

};

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	Board board;
	int steps_to_finish;

	int error;
	error = board.LoadData(data_file);

	if (error != 0)
	{
		system("pause");
		return 0;
	}

	board.InvertBoard();
	board.InitGroups();
	board.groups.DistinguishGoups();

	steps_to_finish = 0;

	board.groups.FindShortestWay(board.start_cell, board.finish_cell);
	cout << endl;
	steps_to_finish = board.finish_cell->shortest_movement;
	if (steps_to_finish == -1)
		cout << "����� ���������� ������� �� ������." << endl;
	else if (steps_to_finish >= 0)
		cout << "����������� ���������� ����� �� ������ �� ������: " << steps_to_finish << endl;
	else
		cout << "������ " << steps_to_finish;

	cout << endl;

	system("pause");
	return 0;
}


/*
400 480
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P..F....P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
.......P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P.........P.PPP...P.P....P.P.......P...P........P.P...P............P..P.P.P..P..P..
.P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P..........P.P....P.P..P.P...P..PP............P.PP....PPPP....P....PPP.P.....PP.P.........
.P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P...P...P.......P.......P.P.........PP....P.PP..P.P.........PPP.....P..PP...P...P..
P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....P.....P...P.PPPP.P...P..P....P...P..PP...P....P....P......P...P......P.....P....
P.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPPP.PP.........P...........P.P.......................PP.P...P......P...P......PPPP
P..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PPP..P..P...P.P......P...............P..P.....PP.P.P.............PPP.....P...PP.PP
..P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P..........P.........P..PP.P.P..P.P.....P.....P.P..P.P....PP..P.P..PP..P....P...P........
.......P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP...........P..P.P..P.P.PP......P..P....P......P......P.....................PPPPP....
PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P.......PPPP...P...........P....PPP...PPP......P.P...PP.....P........P...P.PPP..P....S..
...P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP........P...P.....P.........P..P...............P.....P............P.P.....P...PP.....
*/