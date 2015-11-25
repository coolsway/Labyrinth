#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct cellsData {
	int **cells;
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

int checkAdjacency(int maze[8][32], int numRows, int numCols, int rowNum, int colNum) {

	int checks = 0;

	if (rowNum > 0 && maze[rowNum-1][colNum] == 0) checks++;
	if (rowNum < numRows-1 && maze[rowNum+1][colNum] == 0) checks++;
	if (colNum > 0 && maze[rowNum][colNum-1] == 0) checks++;
	if (colNum < numCols-1 && maze[rowNum][colNum+1] == 0) checks++;

	if (checks < 2) return 1;

	return 0;
}

void addToCells(struct cellsData *cellsData1, int *index) {

	int size = cellsData1->length;

	for (int i = 0; i < size; i++) {
		if (cellsData1->cells[i][0] == 999 && cellsData1->cells[i][1] == 999) {
			cellsData1->cells[i][0] = index[0];
			cellsData1->cells[i][1] = index[1];
			cellsData1->length++;
			break;
		}
	}
}

void addSurroundingCells(int maze[8][32], int numRows, int numCols, struct cellsData *data, int rowNum, int colNum) {

	int *upIndex = (int *)calloc(2, sizeof(int));
	int *downIndex = (int *)calloc(2, sizeof(int));
	int *leftIndex = (int *)calloc(2, sizeof(int));
	int *rightIndex = (int *)calloc(2, sizeof(int));

	// UP
	if (rowNum > 1) {
		upIndex[0] = rowNum-1;
		upIndex[1] = colNum;
		addToCells(data, upIndex);
	}

	// DOWN
	if (rowNum < numRows-2) {
		downIndex[0] = rowNum+1;
		downIndex[1] = colNum;
		addToCells(data, downIndex);
	}

	// LEFT
	if (colNum > 1) {
		leftIndex[0] = rowNum;
		leftIndex[1] = colNum-1;
		addToCells(data, leftIndex);
	}

	// RIGHT
	if (colNum < numCols-2) {
		rightIndex[0] = rowNum;
		rightIndex[1] = colNum+1;
		addToCells(data, rightIndex);
	}

	free(upIndex);
	free(downIndex);
	free(leftIndex);
	free(rightIndex);
}

void generateMaze(int maze[8][32]) {

	struct cellsData *data = (struct cellsData*) malloc(sizeof(struct cellsData));
	data->cells = (int**) malloc(250*sizeof(int));
	data->length = 1;

	for (int i = 0; i < 250; i++) {
		data->cells[i] = (int*)calloc(2,sizeof(int));
		data->cells[i][0] = 999;
		data->cells[i][1] = 999;
	}

	int numRows = 8;
	int numCols = 32;

	// Choose starting cell
	int startRow = (rand() % (numRows-1)) + 1;
	int startCol = (rand() % (numCols-1)) + 1;

	printf("%d %d\n", startRow, startCol);

	clearCell(maze,startRow,startCol);
	addSurroundingCells(maze, numRows, numCols, data, startRow, startCol);

	while (data->length > 0) {
		printf("Before: %d\n", data->length);

		int randomIndex = rand() % data->length;
		int *randomCell = (int *) calloc(2, sizeof(int));
		randomCell[0] =  data->cells[randomIndex][0];
		randomCell[1] =  data->cells[randomIndex][1];
		
		if (checkAdjacency(maze, numRows, numCols, randomCell[0], randomCell[1]) == 1) {
			clearCell(maze, randomCell[0], randomCell[1]);
			// printf("clear\n");
			addSurroundingCells(maze, numRows, numCols, data, randomCell[0], randomCell[1]);
		}

		printf("Mid: %d\n", data->length);
		for (int i = 0; i < 2; i++) {
			printf("%d ", randomCell[i]);
		}
		printf("\n");

		// Remove cell from list of cells
		int size = data->length;

		for (int i = 0; i < size; i++) {
			if (data->cells[i][0] == randomCell[0] && data->cells[i][1] == randomCell[1]) {
				data->cells[i][0] = 999;
				data->cells[i][1] = 999;
				data->length--;
				break;
			}
		}

		printf("After: %d\n\n", data->length);

		free(randomCell);
	}

	free(data->cells);
	free(data);
}

int main() {

	srand(time(NULL));

	generateMaze(maze);

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 32; j++) {
			printf("%d ", maze[i][j]);
		}
		printf("\n");
	}
}