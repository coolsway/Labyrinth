#include <stdio.h>
#include <stdlib.h>

struct cellsData {
	int *cells;
	int size;
	int length;
};

int maze[8][32] = {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
				   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
				   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
				   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
				   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
				   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
				   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
				   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

void clearCell(int maze[8][32], int rowNum, int colNum) {

	maze[rowNum][colNum] = 0;
}

void addToCells(struct cellsData *cellsData1, int *index) {

	for (int i = 0; i < cellsData1->size; i++) {
		if (cellsData1->cells[i] == 0) {
			cellsData1->cells[i] = *index;
			free(index);
			cellsData1->length++;
		}
	}
}

void addSurroundingCells(int maze[8][32], int numRows, int numCols, struct cellsData *cellsData1, int rowNum, int colNum) {

	int *upIndex = (int *)calloc(2, sizeof(int));
	int *downIndex = (int *)calloc(2, sizeof(int));
	int *leftIndex = (int *)calloc(2, sizeof(int));
	int *rightIndex = (int *)calloc(2, sizeof(int));

	// UP
	if (rowNum > 1) {
		upIndex[0] = rowNum-1;
		upIndex[1] = colNum;
		addToCells(cellsData1, upIndex);
	}

	// DOWN
	if (rowNum < numRows-2) {
		downIndex[0] = rowNum+1;
		downIndex[1] = colNum;
		addToCells(cellsData1, downIndex);
	}

	// LEFT
	if (colNum > 1) {
		leftIndex[0] = rowNum-1;
		leftIndex[1] = colNum;
		addToCells(cellsData1, leftIndex);
	}

	// RIGHT
	if (colNum < numCols-2) {
		rightIndex[0] = rowNum+1;
		rightIndex[1] = colNum;
		addToCells(cellsData1, rightIndex);
	}
}

int checkEmpty(int *cells) {

	for (int i = 0; i < sizeof(cells)/sizeof(cells[0]); i++) {
		if (cells[i] != 0) return 0;
	}

	return 1;
}

int checkAdjacency(int maze[8][32], int numRows, int numCols, int rowNum, int colNum) {

	int checks = 0;

	if (rowNum > 0 && maze[rowNum-1][colNum] == 0) checks++;
	if (rowNum < numRows-1 && maze[rowNum+1][colNum] == 0) checks++;
	if (colNum > 0 && maze[rowNum][colNum-1] == 0) checks++;
	if (colNum < numCols-1 && maze[rowNum][colNum+1] == 0) checks++;

	if (checks < 2) return 1;

	return 0;
}

void generateMaze(int maze[8][32]) {

	struct cellsData *cellsData1 = (struct cellsData*) malloc(sizeof(struct cellsData));
	cellsData1->cells = (int *) calloc(1, sizeof(int));
	cellsData1->size = sizeof(maze[8][32]);
	cellsData1->length = 0;

	// int numRows = sizeof(maze)/sizeof(maze[0]);
	// int numCols = sizeof(maze[0])/sizeof(maze[0][0]);

	int numRows = 8;
	int numCols = 32;

	// Choose starting cell
	int startRow = rand() % numRows;
	int startCol = rand() % numCols;

	maze[startRow][startCol] = 0;
	// clearCell(maze, startRow, starteCol);
	addSurroundingCells(maze, numRows, numCols, cellsData1, startRow, startCol);

	do {
		int randomIndex = rand() % cellsData1->length;
		int randomCell[2] = {cellsData1->cells[randomIndex]};

		// Copy index from list of cells into randomCell

		// for (int i = 0; i < 2; i++) {
		// 	randomCell[i] = cellsData1->cells[randomIndex[i]];
		// }
		
		if (checkAdjacency(maze, numRows, numCols, randomCell[0], randomCell[1])) {
			maze[randomCell[0]][randomCell[1]] = 0;
			// clearCell(maze, randomCell[0], randomCell[1]);
			addSurroundingCells(maze, numRows, numCols, cellsData1, randomCell[0], randomCell[1]);
		}

		// Remove cell from list of cells
		for (int i = 0; i < cellsData1->size; i++) {
			if (cellsData1->cells[i] == randomCell) {
				cellsData1->cells[i] = 0;
				cellsData1->length--;
			}
		}

	} while (!checkEmpty(cellsData1->cells));
}

int main() {

	generateMaze(maze);

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 32; j++) {
			printf("%d ", maze[i][j]);
		}
		printf("\n");
	}

}