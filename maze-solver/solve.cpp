#include "maze.h"
#include "path.h"
#include<queue>
#include<vector>
#include<list>
#include<tuple>
#include<utility>
#include<iostream>
#include<climits>
#include<sstream>
#include<stack>
#include<queue>
using namespace std;

path solve_dfs(Maze& m, int rows, int cols);
path solve_recursive_dfs(Maze& m, int rows, int cols, point p);
path solve_bfs(Maze& m, int rows, int cols);
path solve_dijkstra(Maze& m, int rows, int cols);
path solve_tour(Maze& m, int rows, int cols);

bool isVisited(const list<point> & visited, const point p); 
void orderedInsert(list<pair<point,int>>& l, point p, int cost);


int main(int argc, char** argv)
{
    if(argc != 4)
    {
        cerr << "usage:\n"
             << "./maze option rows cols\n"
             << " options:\n"
             << "  -dfs: depth first search (backtracking)\n"
             << "  -bfs: breadth first search\n"
             << "  -dij: dijkstra's algorithm\n"
             << "  -tour: all corners tour\n"
             << "  -basic: run dfs, bfs, and dij\n"
             << "  -advanced: run dfs, bfs, dij and tour" << endl;
        return 0;
    }
    string opt(argv[1]);

    int rows, cols;
    stringstream s;
    s << argv[2] << " " << argv[3];
    s >> rows >> cols;

    // construct a new random maze;
    Maze m(rows, cols);

    // print the initial maze out
    cout << "Initial maze" << endl;
    m.print_maze(cout, opt == "-dij" || opt == "-tour");

    if(opt == "-dfs")
    {
        cout << "\nSolved dfs" << endl;
        path p = solve_dfs(m, rows, cols);
        m.print_maze_with_path(cout, p, false, false);
    }


    if(opt == "-bfs")
    {
        cout << "\nSolved bfs" << endl;
        path p = solve_bfs(m, rows, cols);
        m.print_maze_with_path(cout, p, false, false);
    }

    if(opt == "-dij")
    {
        cout << "\nSolved dijkstra" << endl;
        path p = solve_dijkstra(m, rows, cols);
        m.print_maze_with_path(cout, p, true, false);
    }

    if(opt == "-tour")
    {
        cout << "\nSolved all courners tour" << endl;
        path p = solve_tour(m, rows, cols);
        m.print_maze_with_path(cout, p, true, true);
    }
    if(opt == "-basic")
    {
        cout << "\nSolved dfs" << endl;
        path p = solve_dfs(m, rows, cols);
        m.print_maze_with_path(cout, p, false, false);

        cout << "\nSolved bfs" << endl;
        p = solve_bfs(m, rows, cols);
        m.print_maze_with_path(cout, p, false, false);

        cout << "\nSolved dijkstra" << endl;
        p = solve_dijkstra(m, rows, cols);
        m.print_maze_with_path(cout, p, true, false);
    }
    if(opt == "-advanced")
    {
        cout << "\nSolved dfs" << endl;
        path p = solve_dfs(m, rows, cols);
        m.print_maze_with_path(cout, p, false, false);

        cout << "\nSolved bfs" << endl;
        p = solve_bfs(m, rows, cols);
        m.print_maze_with_path(cout, p, false, false);

        cout << "\nSolved dijkstra" << endl;
        p = solve_dijkstra(m, rows, cols);
        m.print_maze_with_path(cout, p, true, false);

        cout << "\nSolved all courners tour" << endl;
        p = solve_tour(m, rows, cols);
        m.print_maze_with_path(cout, p, true, true);
    }
}

//solve a maze using depth first search.
path solve_dfs(Maze& m, int rows, int cols)
{
	stack<point> s; //Acts as a breadcrumb trail through the maze.
	list<point> finalPath; //Holds the final path after dfs is run.
	point p; //Current point
	bool visited[rows][cols]; //Hold info about what squares we have visitied.

	for(int i = 0; i < rows; ++i)
	{
		for(int j = 0; j < cols; ++j)
		{
			visited[i][j] = false;
		}
	}

	//need to push init point on the stack (0,0)
	s.push(p);
	//Also the start point is always visited.
	visited[0][0] = true;

	while(p != pair(rows-1, cols-1))
	{
		//used for indexing the visited array.
		point tempUp = p + moveIn(UP);
		point tempRight = p + moveIn(RIGHT);
		point tempLeft = p + moveIn(LEFT);
		point tempDown = p + moveIn(DOWN);

		if(m.can_go_down(p.first, p.second) && !visited[tempDown.first][tempDown.second])
		{
			p = tempDown;
			s.push(p);
			visited[p.first][p.second] = true;
		}

		else if(m.can_go_left(p.first, p.second) && !visited[tempLeft.first][tempLeft.second])
		{
			p = tempLeft; 
			s.push(p);
			visited[p.first][p.second] = true;
		}

		else if(m.can_go_up(p.first, p.second) && !visited[tempUp.first][tempUp.second])
		{
			p = tempUp;
			s.push(p);
			visited[p.first][p.second] = true;
		}

		else if(m.can_go_right(p.first, p.second) && !visited[tempRight.first][tempRight.second])
		{
			p = tempRight;
			s.push(p);
			visited[p.first][p.second] = true;
		}

		//cant go anywhere? go back
		else
		{
			if(!s.empty())
			{
				s.pop();
				p = s.top();
			}
		}
	}

	//recreate the path through the maze.
	while(!s.empty())
	{
		finalPath.push_front(s.top());
		s.pop();
	}

    return finalPath;
}

//solve a maze using the breadth first search algorithm.
path solve_bfs(Maze& m, int rows, int cols)
{
	queue<point> q; //queue that lets us know what point to visit next
	point p; //current point
	vector<vector<point>> path; //2d vector that keeps track of the point we came from
	list<point> finalPath; //Will hold the shortest path after algorithm has run.
	bool visited[rows][cols]; //keep track of the points we have visited.

	path.resize(rows, vector<point>(cols, p));
	for(int i = 0; i < rows; ++i)
	{
		for(int j = 0; j < cols; ++j)
		{
			visited[i][j] = false;
		}
	}

	//initial square (0,0)
	q.push(p);
	visited[0][0] = true;

	//Stop when we find the end. Alternatively, we could go until the queue
	//is empty but we only care about solving the maze so the way it is being done
	//is more efficient.
	while(p != pair(rows-1, cols-1))
	{
		p = q.front();
		q.pop();

		if(m.can_go_up(p.first, p.second))
		{
			point temp = p + moveIn(UP);
			if(visited[temp.first][temp.second] == false)
			{
				q.push(temp);
				visited[temp.first][temp.second] = true;
				path[temp.first][temp.second] = p;
			}
		}

		if(m.can_go_right(p.first, p.second))
		{
			point temp = p + moveIn(RIGHT);
			if(visited[temp.first][temp.second] == false)
			{
				q.push(temp);
				visited[temp.first][temp.second] = true;
				path[temp.first][temp.second] = p;
			}
		}

		if(m.can_go_down(p.first, p.second))
		{
			point temp = p + moveIn(DOWN);
			if(visited[temp.first][temp.second] == false)
			{
				q.push(temp);
				visited[temp.first][temp.second] = true;
				path[temp.first][temp.second] = p;
			}
		}

		if(m.can_go_left(p.first, p.second))
		{
			point temp = p + moveIn(LEFT);
			if(visited[temp.first][temp.second] == false)
			{
				q.push(temp);
				visited[temp.first][temp.second] = true;
				path[temp.first][temp.second] = p;
			}
		}
	}

	//recreate the shortest path using the path vector.
	//First add the end point of the maze, since in the path array, the
	//end point actually tells us the point we came from.
	finalPath.push_front(pair(rows-1, cols-1));
	p = path[rows-1][cols-1];
	finalPath.push_front(p);

	while(p != pair(0,0))
	{
		p = path[p.first][p.second];
		finalPath.push_front(p);
	}

    	return finalPath;
}

//insert points into the list based on its cost. This allows dijkstras algorithm
//to prioritize picking the points that have the most promising costs first.
void orderedInsert(list<pair<point,int>>& l, point p, int cost)
{
	list<pair<point,int>>::iterator it;
	bool flag = true;

	if(l.empty())
	{
		l.push_back(pair(p, cost));
	}

	else
	{
		it = l.begin();
		for(auto const& x : l)
		{
			//add to the list ordered by cost.
			if(x.second > cost)
			{	
				l.insert(it, pair(p, cost));
				flag = false;
			}
			++it;
		}
		
		//if the cost of our new point is larger than every other cost
		//currently in the queue, flag will still be true and we add
		//to the end of the list.
		if(flag)
		{
			l.push_back(pair(p, cost));
		}
	}
}

//solve a maze using dijkstra's algorithm.
path solve_dijkstra(Maze& m, int rows, int cols)
{
	//the queue holds the point and the cost of the point
	list<pair<point,int>> l;

	//2d vector that is the size of the maze the tells us how we got to each square.
	vector<vector<point>> path;

	//2d vector that is the size of the maze that tells us how much it costs to get from the start to that square.
	vector<vector<int>> costs;

	list<point> finalPath; //Holds the final path after dijkstra's is run
	bool visited[rows][cols]; //Holds info about what squares we have visited.
	point p; //Current point

	path.resize(rows, vector<point>(cols, p));

	//We want the costs array to be initialzied to infinity, Since an int
	//is finite the closest thing is to set it to the max value of an int.
	costs.resize(rows, vector<int>(cols, 2147483647));

	for(int i = 0; i < rows; ++i)
	{
		for(int j = 0; j < cols; ++j)
		{
			visited[i][j] = false;
		}
	}

	//initial square (0,0)
	l.push_back(pair(p, 0));
	costs[0][0] = 0;
	
	//Stop when we find the end. Alternatively, we could go until the queue
	//is empty but we only care about solving the maze so the way it is being done
	//is more efficient.
	while(p != pair(rows-1, cols-1))
	{
		p = l.front().first;
		l.pop_front();
		visited[p.first][p.second] = true;

		if(m.can_go_up(p.first, p.second))
		{
			point temp = p + moveIn(UP);
			int cost = m.cost(p, UP); //cost to move up

			if(visited[temp.first][temp.second] == false)
			{
				//cost to move up + sum of costs to get to the
				//point we are currently at.
				cost = cost + costs[p.first][p.second];
				if(cost < costs[temp.first][temp.second])
				{
					orderedInsert(l, temp, cost);
					costs[temp.first][temp.second] = cost;
					path[temp.first][temp.second] = p;
				}
			}
		}

		if(m.can_go_right(p.first, p.second))
		{
			point temp = p + moveIn(RIGHT);
			int cost = m.cost(p, RIGHT);

			if(visited[temp.first][temp.second] == false)
			{
				cost = cost + costs[p.first][p.second];
				if(cost < costs[temp.first][temp.second])
				{
					orderedInsert(l, temp, cost);
					costs[temp.first][temp.second] = cost;
					path[temp.first][temp.second] = p;
				}
			}
		}

		if(m.can_go_down(p.first, p.second))
		{
			point temp = p + moveIn(DOWN);
			int cost = m.cost(p, DOWN);

			if(visited[temp.first][temp.second] == false)
			{
				cost = cost + costs[p.first][p.second];
				if(cost < costs[temp.first][temp.second])
				{
					orderedInsert(l, temp, cost);
					costs[temp.first][temp.second] = cost;
					path[temp.first][temp.second] = p;
				}
			}
		}

		if(m.can_go_left(p.first, p.second))
		{
			point temp = p + moveIn(LEFT);
			int cost = m.cost(p, LEFT);

			if(visited[temp.first][temp.second] == false)
			{
				cost = cost + costs[p.first][p.second];
				if(cost < costs[temp.first][temp.second])
				{
					orderedInsert(l, temp, cost);
					costs[temp.first][temp.second] = cost;
					path[temp.first][temp.second] = p;
				}
			}
		}
	}

	//recreate the path.
	//Since path holds information about the previous point, the last point
	//is actually not in the array so we need to add that first manually.
	//Then we can use the path array to find the path we took through the maze.
	finalPath.push_front(pair(rows-1, cols-1));
	p = path[rows-1][cols-1];
	finalPath.push_front(p);

	while(p != pair(0,0))
	{
		p = path[p.first][p.second];
		finalPath.push_front(p);
	}

    	return finalPath;
}

//Didin't do the advances assignment.
path solve_tour(Maze& m, int rows, int cols)
{
    return list<point>();
}
