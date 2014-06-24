#include <QtGui>
#include "maze.h"

maze::maze(int _mazeWidth, int _mazeHeight, int _currentAlgo, bool _needToBePassed, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	mazeWidth = _mazeWidth;
	mazeHeight = _mazeHeight;
	givePass = _needToBePassed;
	currentAlgo = algorythm (_currentAlgo);
	
	vertexCount = (mazeWidth+1) * (mazeHeight+1);
	vertexSpace = 20;

	srand(time(NULL));

	mazeMatrix = matrix = vector<vector<int>> ( vertexCount,vector<int>(vertexCount,NULL) );
	visited = vector<bool>(vertexCount,false);

	if(currentAlgo == DFS)
	{
		buildMatrix();
		// depth-first search here
		for(int i=0;i<visited.size();i++)
			if(!visited[i])
				dfs(i);
	}
	else if(currentAlgo == HEURISTIC)
	{
		heuriscticBuild();
	}

	if(givePass)
		makePath();

	this->resize(mazeWidth*vertexSpace+55,mazeHeight*vertexSpace+15);
}

maze::~maze()
{

}

// calculates number of vertex from position
int maze::getVertexNumber(int currentXPos, int currentYPos)
{
	return currentYPos*(mazeWidth+1)+currentXPos;
}

// calculates vertex position from number in matrix
void maze::getVertexCoords(int number, int &x, int &y)
{
	x=number%(mazeWidth+1);
	y=number/(mazeWidth+1);
}

void maze::buildMatrix()
{
	int vertex=0,vTop=0,vBottom=0,vLeft=0,vRight=0;

	for(int i = 1; i < mazeWidth; i++)			
	{
		for(int j = 1; j < mazeHeight; j++)
		{
			vertex = getVertexNumber(i,j);		

			vLeft	= getVertexNumber(i-1,j);	
			vRight	= getVertexNumber(i+1,j);	
			vTop	= getVertexNumber(i,j-1);	
			vBottom = getVertexNumber(i,j+1);	

			matrix[vertex]	[vLeft]		= 
			matrix[vertex]	[vRight]	= 
			matrix[vertex]	[vTop]		= 
			matrix[vertex]	[vBottom]	= 
			matrix[vLeft]	[vertex]	= 
			matrix[vRight]	[vertex]	= 
			matrix[vTop]	[vertex]	= 
			matrix[vBottom]	[vertex]	= 1;
		}
	}	
}

// add a random path from entrance to exit
void maze::makePath()
{
	enum direction {RIGHT,DOWN};
	direction currentDirection=DOWN;

	int currentX=0,currentY=0,nextX=0,nextY=0;	// cell-of-maze coordinates
	int currentVertex=0,nextVertex=0;

	for(int i=0;i<mazeWidth-1 + mazeHeight-1;i++)
	{
		currentVertex=0;
		nextVertex=0;

		if(currentX == mazeWidth-1)
			currentDirection = DOWN;
		else if(currentY == mazeHeight-1)
			currentDirection = RIGHT;
		else
			currentDirection = direction(rand()%2);
	
		if(currentDirection==RIGHT)
		{
			nextX++;
			currentVertex = getVertexNumber(currentX+1,currentY+1);
			nextVertex = getVertexNumber(currentX+1,currentY+1-1);
			currentX=nextX;
		}
		else if(currentDirection=DOWN)
		{
			nextY++;
			currentVertex = getVertexNumber(currentX+1,currentY+1);
			nextVertex = getVertexNumber(currentX+1-1,currentY+1);
			currentY=nextY;
		}

		mazeMatrix[currentVertex][nextVertex] = 
		mazeMatrix[nextVertex][currentVertex] = NULL;
	}
}

void maze::dfs(int start)
{
	int vertex=0;
	visited[start]=true;
	for(vertex=1;vertex<vertexCount;vertex++)
	{
		if(matrix[start][vertex])
		{
			mazeMatrix[start][vertex] = mazeMatrix[vertex][start] = rand()%2;
			if(!visited[vertex])					
				dfs(vertex);
		}
	}
}

void maze::heuriscticBuild()
{
	for(int i=0;i<vertexCount/mazeHeight;i++)
		makeEdge( getVertexNumber(1+rand()%(mazeWidth-1),1+rand()%(mazeHeight-1)) , getVertexNumber(rand()%(mazeWidth),rand()%(mazeHeight)) );
}

void maze::makeEdge(int start, int finish)
{
	int edge = 1;
	
	enum direction {STAY,LEFT,RIGHT,UP,DOWN};
	direction currentDirection = STAY;

	int nextX=0,nextY=0,endX=0,endY=0;
	// current couple of vertex for building wall
	int currentVertex=start,nextVertex=start;
	
	getVertexCoords(start,nextX,nextY);
	getVertexCoords(finish,endX,endY);
	
	vector<direction> permitDirections(NULL);
	
	if(nextX < endX)	permitDirections.push_back(RIGHT);
	else if(nextX > endX) permitDirections.push_back(LEFT);

	if(nextY < endY)	permitDirections.push_back(DOWN);
	else if(nextY > endY) permitDirections.push_back(UP);

	while(currentVertex!=finish)
	{
		if(permitDirections.size()>1)
			if(nextX == endX)
				permitDirections.erase(permitDirections.begin());
			else if(nextY == endY)
				permitDirections.pop_back();

		if(permitDirections.size())
			currentDirection = permitDirections.at(rand() % permitDirections.size());
		
		if(currentDirection == LEFT)
			nextX--;
		else if(currentDirection == RIGHT)
			nextX++;
		else if(currentDirection == UP)
			nextY--;
		else if(currentDirection == DOWN)
			nextY++;

		nextVertex = getVertexNumber(nextX,nextY);
		
		mazeMatrix[currentVertex][nextVertex] = 
		mazeMatrix[nextVertex][currentVertex] = edge;

		currentVertex = nextVertex;
	}
}

void maze::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setViewport( 25, 5, (mazeWidth+1)*vertexSpace, mazeHeight*vertexSpace );
	
	drawMaze(&painter);
}

void maze::drawMaze(QPainter *painter)
{
	QPen pen(palette().windowText() , 2.5);
	QColor color(Qt::black);
	
	painter->setPen(pen);
	painter->setBrush(palette().window());
	// magic required
	int magicDelta = 5;
	
	int x=painter->viewport().x();
	int y=painter->viewport().y();
	int w=painter->viewport().width()+magicDelta;
	int h=painter->viewport().height()+magicDelta;

	painter->drawLine(x,y,w,y);					// top border 
	painter->drawLine(x,y+vertexSpace,x,h);		// left border 
	painter->drawLine(x,h,w,h);					// bottom border 
	painter->drawLine(w,y,w,h-vertexSpace);		// right border 

	for(int i=1;i<mazeWidth;i++)				// vertexes 
	{
		for(int j=1;j<mazeHeight;j++)
		{
			painter->drawLine(x+i*vertexSpace-2,y+j*vertexSpace,x+i*vertexSpace+2,y+j*vertexSpace);
			painter->drawLine(x+i*vertexSpace,y+j*vertexSpace-2,x+i*vertexSpace,y+j*vertexSpace+2);
		}
	}

	int xVertex1=0,xVertex2=0,yVertex1=0,yVertex2=0;	// vertex coords
	for(int i=1;i<mazeMatrix.size()-1;i++)
	{
		for(int j=i;j<mazeMatrix[0].size()-1;j++)
		{
			if(mazeMatrix[i][j])						// if edge exists
			{
				getVertexCoords(i,xVertex1,yVertex1);	
				getVertexCoords(j,xVertex2,yVertex2);	
				if((xVertex1==xVertex2) || (yVertex1==yVertex2))	// если ребро соединяет вершины строго по горизонтали или вертикали - 
				{
					painter->drawLine(x+xVertex1*vertexSpace,y+yVertex1*vertexSpace,x+xVertex2*vertexSpace,y+yVertex2*vertexSpace);	
				}
			}
		}
	}
}