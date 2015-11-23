import random

maze = [[1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
	    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
	    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
	    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
	    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
	    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
	    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
	    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]]

cells = []

def chooseStartIndex(maze):

	startRow = random.randint(0,len(maze)-1)
	startCol = random.randint(0, len(maze[0])-1)

	startIndex = [startRow, startCol]

	return startIndex

def checkAdjacency(maze, index):

	checks = 0		# returns 1 if current block is not adjacent to two visited blocks

	if index[0] > 0 and maze[index[0]-1][index[1]] == 0:
		checks += 1			# check above

	if index[0] < len(maze)-1 and maze[index[0]+1][index[1]] == 0:
		checks += 1			# check below

	if index[1] > 0 and maze[index[0]][index[1]-1] == 0:
		checks += 1			# check left

	if index[1] < len(maze[0])-1 and maze[index[0]][index[1]+1] == 0:
		checks += 1			# check right

	if checks < 2:
		return 1

	return 0

def getSurroundingCells(maze, index, cells):

	upIndex = []
	downIndex = []
	leftIndex = []
	rightIndex = []

	# UP
	if index[0] > 1:
		upIndex = [index[0]-1, index[1]]
		cells.append(upIndex)

	# DOWN
	if index[0] < len(maze)-2:
		downIndex = [index[0]+1, index[1]]
		cells.append(downIndex)

	# LEFT
	if index[1] > 1:
		leftIndex = [index[0], index[1]-1]
		cells.append(leftIndex)

	# RIGHT
	if index[1] < len(maze[0])-2:
		rightIndex = [index[0], index[1]+1]
		cells.append(rightIndex)

def clearCell(maze, startIndex):

	maze[startIndex[0]][startIndex[1]] = 0

def main(maze):

	startIndex = chooseStartIndex(maze)
	clearCell(maze, startIndex)
	getSurroundingCells(maze, startIndex, cells)

	while len(cells) > 0:

		randomIndex = random.randint(0,len(cells)-1)
		randomCell = cells[randomIndex]

		if checkAdjacency(maze, randomCell):
			clearCell(maze, randomCell)
			getSurroundingCells(maze, randomCell, cells)
		cells.remove(randomCell)

	for row in maze:
		print row


main(maze)
