from maze import giveMeTheMaze
maze = giveMeTheMaze()
maze_2d = [row.split() for row in maze.strip().split("\n")]

# Define rows and cols properly
rows, cols = len(maze_2d), len(maze_2d[0])

# Define start and end
start = (0, 0)
end = (rows - 1, cols - 1)

maze_2d[start[0]][start[1]] = 'S'
maze_2d[end[0]][end[1]] = 'E'

def reconstruct_path(came_from, start, cur_node, maze):
    path = []
    while cur_node:
        path.append(cur_node)
        cur_node = came_from.get(cur_node)
    path.reverse()

    for current, next_ in zip(path, path[1:]):
        if current[0] < next_[0]:  
            maze[current[0]][current[1]] = 'v'
        elif current[0] > next_[0]:  
            maze[current[0]][current[1]] = '^'
        elif current[1] < next_[1]:  
            maze[current[0]][current[1]] = '>'
        elif current[1] > next_[1]:  
            maze[current[0]][current[1]] = '<'

    return path if path[0] == start else None

def deception(came_from, start, end, maze):
        cam = "S"
        path = reconstruct_path(came_from, start, end, maze)
        for current, next in zip(path, path[1:]):
            now = maze[current[0]][current[1]]
            nach = maze[next[0]][next[1]]

            if now == nach:
                    cam+="S"
            if now == 'v' and nach == '>':
                    cam+="L"
            if now == 'v' and nach == '<':
                    cam+="R"
            if now == '>' and nach == 'v':
                    cam+="R"
            if now == '>' and nach == '^':
                    cam+="L"
            if now == '<' and nach == 'v':
                    cam+="L"
            if now == '<' and nach == '^':
                    cam+="R"
            if now == '^' and nach == '>':
                    cam+="R"
            if now == '^' and nach == '<':
                    cam+="L"
                    
        return cam

def is_sequence(maze, start, end):
    rows, cols = len(maze), len(maze[0])
    visited = [[False for _ in range(cols)] for _ in range(rows)]
    queue = ([start])
    came_from = {start: None}

    directions = [(0, 1), (1, 0), (0, -1), (-1, 0)]  # Right, Down, Left, Up

    while queue:
        r, c = queue.pop(0)
        if (r, c) == end:
            path = deception(came_from, start, end, maze)        
            return path
        visited[r][c] = True

        for dr, dc in directions:
            nr, nc = r + dr, c + dc
            if 0 <= nr < rows and 0 <= nc < cols and not visited[nr][nc] and maze[nr][nc] != 'X':
                queue.append((nr, nc))
                came_from[(nr, nc)] = (r, c) 

    return False


print(is_sequence(maze_2d, start, end))