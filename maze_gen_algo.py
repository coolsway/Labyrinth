import random

maze = [[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
	    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
	    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
	    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
	    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
	    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
	    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
	    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]]

onesList = []

def chooseStartIndex(maze):

	startRow = random.randint(0,len(maze)-1)
	startCol = random.randint(0, len(maze[0])-1)

	startIndex = [startRow, startCol]

	return startIndex	

def checkAdjacency(currentIndex, maze):

	checks = 0		# returns 1 if current block is not adjacent to two visited blocks

	if currentIndex[0] > 0 and maze[currentIndex[0]-1][currentIndex[1]] == 1:
		checks += 1			# check above

	if currentIndex[0] < len(maze)-1 and maze[currentIndex[0]+1][currentIndex[1]] == 1:
		checks += 1			# check below

	if currentIndex[1] > 0 and maze[currentIndex[0]][currentIndex[1]-1] == 1:
		checks += 1			# check left

	if currentIndex[1] < len(maze[0])-1 and maze[currentIndex[0]][currentIndex[1]+1] == 1:
		checks += 1			# check right

	if checks < 2:
		return 1

	return 0


def chooseNextIndex(maze, currentIndex):

	nextIndex = currentIndex

	while nextIndex == currentIndex or checkAdjacency(nextIndex, maze) == 0:

		direc = random.randint(0,3)

		if direc == 0 and currentIndex[0]-1 >= 0:				# UP
			nextIndex = [currentIndex[0]-1, currentIndex[1]]
		elif direc == 1 and currentIndex[0]+1 <= len(maze)-1:			# DOWN
			nextIndex = [currentIndex[0]+1, currentIndex[1]]
		elif direc == 2 and currentIndex[1]-1 >= 0:			# LEFT
			nextIndex = [currentIndex[0], currentIndex[1]-1]
		elif direc == 3 and currentIndex[1]+1 <= len(maze[0])-1:			# RIGHT
			nextIndex = [currentIndex[0], currentIndex[1]+1]

		#if (nextIndex[0] >= 0 and nextIndex[0] <= len(maze)-1) and (nextIndex[1] >= 0 and nextIndex[1] <= len(maze[0])-1) and maze[nextIndex[0]][nextIndex[1]] != 1:
			#break
				
	return nextIndex

def returnOpenBlocks(onesList, maze, direc):

	openBlocksList = onesList

	if direc == 0:						# MOVE EVERY BLOCK UP
		for index in openBlocksList:
			if index[1] < len(maze)-1:
				index[1] += 1

	elif direc == 1:						# MOVE EVERY BLOCK DOWN
		for index in openBlocksList:
			if index[1] > 0:
				index[1] -= 1

	elif direc == 2:						# MOVE EVERY BLOCK LEFT
		for index in openBlocksList:
			if index[0] < len(maze[0])-1:
				index[0] += 1

	elif direc == 3:						# MOVE EVERY BLOCK RIGHT
		for index in openBlocksList:
			if index[0] > 0:
				index[0] -= 1

	for index in openBlocksList:
		if checkAdjacency(index, maze) == 0:
			openBlocksList.remove(index)

	return openBlocksList
	
def generateMaze(maze, index):

	nextIndex = chooseNextIndex(maze, index)

	# while checkAdjacency(nextIndex, maze) == 0:
	# 	nextIndex = chooseNextIndex(maze, index)

	if checkAdjacency(nextIndex, maze):

		maze[nextIndex[0]][nextIndex[1]] = 1
		onesList.append(nextIndex)

		# deadEnd = random.randint(0,9) 		#10 % dead end chance
		# if deadEnd == 1:
		# # 	#print "dead", nextIndex
		# # 	#randomly choose an unvisited block that is adjacent to only one visited block
		# # 	# sweeponesList(onesList, maze)
		# 	nextDir = random.randint(0,3)
		# 	openBlocksList = returnOpenBlocks(onesList, maze, nextDir)
		# 	randIndex = random.randint(0,len(openBlocksList)-1)
		# 	nextIndex = openBlocksList[randIndex]
		# 	maze[nextIndex[0]][nextIndex[1]] = 1
		# 	onesList.append(startIndex)
			# nextIndex = chooseNextIndex(maze, index)
			#print "new", nextIndex

		generateMaze(maze, nextIndex)

	# Make sure there is no extra empty white space

	# randomly choose an unvisited block that is adjacent to only one visited block

	#mark last block as "end"


def main():

	startIndex = chooseStartIndex(maze)
	maze[startIndex[0]][startIndex[1]] = 1
	onesList.append(startIndex)
	generateMaze(maze, startIndex)

	# while len(onesList) < 20:

	# 	randIndex = random.randint(0,len(onesList)-1)
	# 	nextIndex = onesList[randIndex]
	# 	nextIndex = chooseNextIndex(maze, nextIndex)
	# 	maze[nextIndex[0]][nextIndex[1]] = 1
	# 	onesList.append(nextIndex)
	# 	generateMaze(maze, nextIndex)

	for row in maze:
		print row

	# print 
	# print onesList

main()

