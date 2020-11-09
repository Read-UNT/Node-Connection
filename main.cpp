/*
=============================================================================================================

Name             : main.cpp
Author           : Read L. Ballew Jr. & Thien-An Vu
Course Section   : CSCE 2100.001
Date             : April 30th, 2018
Description      : The purpse of this program is to find a path through a pre-determined graph, using a depth
                   first search, and a breadth first search.

=============================================================================================================
*/

#include <iostream>
#include <ostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <iomanip>
#include <time.h>
#include <chrono>
#include "boomer.h"
#define CLOCKS_PER_MS (CLOCKS_PER_SEC / 1000)

using namespace std;

vector<int> DFS(vector<vector<int> >, int, vector<edge>, int, int); //DFS search function
void DFSpathing(int, int &, vector<int> &, vector<int> &, vector<edge>, int, int, vector<int> &); //Calculates the DFS path
void checkExplored(vector<int> &, vector<int> &); //Checks explored nodes
bool checkOrder(vector<int>); //Checks order of list

vector<int> BFS(vector<vector<int> >, int, vector<edge>, int, int); //BFS search function
void BFSpathing(vector<int> &, int , vector<edge> , int , int , vector<int> &, vector<int> &, vector<int> &); //Calculates the BFS path
vector<vector<int> > adjecencyMatrix(int, vector<edge>); //Creates a vector of vectors for connections between nodes, and prints it out as well
int doesConnectionExist(vector<edge>, int, int); //Returns the time of a connection, 0 means no connection, > 0 means connection time in ms
vector<int> currentNodeEnds(vector<edge>, int); //Returns a vector list of all connections from one node to other nodes
int calcPropTime(vector<edge>, vector<int>); //Calculates the time of the connections from the list of the nodes passed to it

void header(); //Calls header function

int main()
{
    header();

    //Declaring all important information
    int numberOfNodes;

    vector<edge> allEdges;
    vector<path> allPaths;

    string fileName;
    ifstream fileInput;
    int fileLineCounter = 0;
    int endEdgeCheck = 0;

    vector<vector<int> > aMatrix;
    vector<int> finalPath;

    //Reading in the file

    cout << "Enter file name: ";

    getline(cin, fileName);

    fileInput.open(fileName.c_str()); //Opening file

    if(fileInput.fail())
    {
        cout << "EXIT FAILURE -- Could not open file" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "Reading from file..." << endl << endl;

    int x;

    while(!fileInput.eof()) //Reading all lines of the file input
    {
        fileInput >> x;

        if(fileLineCounter == 0)
        {
            numberOfNodes = x;
            fileInput >> x;
        }

        if(endEdgeCheck == 0)
        {
            if(x == -1)
            {
                endEdgeCheck++;
            }
            else
            {
                tempEdge.valueOne = x;
                fileInput >> tempEdge.valueTwo;
                fileInput >> tempEdge.time;
                allEdges.push_back(tempEdge);
            }
        }

        else
        {
            tempPath.start = x;
            fileInput >> tempPath.end;
            allPaths.push_back(tempPath);
        }

        fileLineCounter++;
    }

    //Test output
    cout << "Number of nodes: " << numberOfNodes << endl << endl;
    cout << "Connection List: " << endl;
    for(unsigned i = 0; i < allEdges.size(); i++)
    {
        cout << "Value One: " << allEdges[i].valueOne << "  Value Two: " << allEdges[i].valueTwo << "  Time: " << allEdges[i].time << endl;
    }
    cout << endl << "Path List: " << endl;
    for(unsigned i = 0; i < allPaths.size(); i++)
    {
        cout << "Start Point: " << allPaths[i].start << "  End Point: " << allPaths[i].end << endl;
    }

    aMatrix = adjecencyMatrix(numberOfNodes, allEdges); //Calling the adjecency matrix printing funciton

    //Running the searches

    int choice = -1;
    while(choice != 0)
    {
        int choiceCount = -1;
        cout << endl << endl << setw(20) << "Path Selection Menu" << endl;
        cout << " 0:  Quit" << endl;
        for(unsigned i = 0; i < allPaths.size(); i++)
        {
            cout << setw(2) << i + 1 << ":  " << allPaths[i].start << " -> " << allPaths[i].end << endl;
            choiceCount = i + 1;
        }
        cout << " Enter selection: ";
        cin >> choice;

        if(choice == 0)
        {
            cout << "You have chosen to quit the program." << endl;
            return 0;
        }

        while(choice < 0 || choice > choiceCount)
        {
            cout << " You have chosen an invalid selection. Please try again." << endl;
            cout << " Enter selection: ";
            cin >> choice;

            if(choice == 0)
            {
                cout << " You have chosen to quit the program." << endl;
                return 0;
            }
        }

        cout << endl;
        //cout << "Testing path selection " << choice << "..." << endl << endl;

        //Calculates DFS path
        finalPath = DFS(aMatrix, numberOfNodes, allEdges, allPaths[choice - 1].start, allPaths[choice - 1].end);

        //Go through the list, until we find the end value, then we print out only the beginning to the end value
        //If the end value is not found in the list, there is no path
        int endPos = -1;
        for(unsigned int i = 0; i < finalPath.size(); i++)
        {
            if(finalPath[i] == allPaths[choice - 1].end)
            {
                endPos = i;
                finalPath.erase(finalPath.begin() + (endPos + 1), finalPath.begin() + finalPath.size());
            }
        }

        if(endPos == -1)
        {
            cout << setw(19) << "       Path: No path found." << endl;
        }
        else
        {
            cout << setw(19) << "Path: ";
            for(unsigned int i = 0; i < finalPath.size(); i++)
            {
                cout << finalPath[i];
                if(i < finalPath.size() - 1)
                {
                    cout << " -> ";
                }
            }

            //Calculates propagation time
            cout << endl << setw(19) << "Propagation time: " << calcPropTime(allEdges, finalPath) << " ms" << endl;
        }

        finalPath = BFS(aMatrix, numberOfNodes, allEdges, allPaths[choice - 1].start, allPaths[choice - 1].end);

        //Go through the list, until we find the end value, then we print out only the beginning to the end value
        //If the end value is not found in the list, there is no path
        endPos = -1;
        for(unsigned int i = 0; i < finalPath.size(); i++)
        {
            if(finalPath[i] == allPaths[choice - 1].end)
            {
                endPos = i;
                finalPath.erase(finalPath.begin() + (endPos + 1), finalPath.begin() + finalPath.size());
            }
        }

        if(endPos == -1)
        {
            cout << setw(19) << "       Path: No path found." << endl;
        }
        else
        {
            cout << setw(19) << "Path: ";
            for(unsigned int i = 0; i < finalPath.size(); i++)
            {
                cout << finalPath[i];
                if(i < finalPath.size() - 1)
                {
                    cout << " -> ";
                }
            }

            //Calculates propagation time
            cout << endl << setw(19) << "Propagation time: " << calcPropTime(allEdges, finalPath) << " ms" << endl << endl;
        }
    }

    return 0;
}

/*
======================================================================================================================================
Function : Depth First Search
Parameters : Vector of vector of integers - creates matrix, integer - number of nodes in total, vector of connections, 
             integer - beginning node, integer ending node
Return : Vector of integers - final path for DFS
Description : This function creates variables needed for the recursive calls the DFS pathing function.
======================================================================================================================================
*/
vector<int> DFS(vector<vector<int> > matrix, int nodes, vector<edge> edges, int beginning, int ending)
{
    auto DFStime = chrono::high_resolution_clock::now();
    cout << "            Depth First Search" << endl;
    cout << "            ------------------" << endl;
    vector<int> stack;
    vector<int> explored;
    vector<int> permaExplored;
    int x = 0;
    int permaBegin = beginning;

    explored.push_back(beginning);

    DFSpathing(permaBegin, x, stack, explored, edges, beginning, ending, permaExplored);

    auto end = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration_cast<chrono::microseconds>(DFStime - end);
    cout << fixed;
    cout << setprecision(4);
    cout << setw(19);
    cout << "DFS timing: " << (elapsed.count() * -.001) << " milliseconds" << endl;

    return explored;
}

/*
======================================================================================================================================
Function : Determines DFS Path
Parameters : Integer - permanent beginning node, Integer - saftey net for number of runs, vector of integers - vector acts as a stack, 
            Vector of integers - explored nodes, vector of connections, integer - beginning node, integer - ending node, 
            vector of integers - permanent node visited list
Return : Nothing, void function.
Description : This function determines the correct path using a DFS search style.
======================================================================================================================================
*/
void DFSpathing(int permaBegin, int &xx, vector<int> &stacks, vector<int> &explored, vector<edge> edges, int begin, int end, vector<int> &permaExplored)
{
    //Infinitely prints out the same number you start with at the top of the stack after one entry of children
    //cout << "begin = " << begin << endl

    xx++;
    if(xx > edges.size())
    {
        return;
    }

    //Doesn't push back explored if the number is already there
    int markForExploration = -1;
    for(unsigned int i = 0; i < explored.size(); i++)
    {
        if(begin == explored[i])
        {
            break;
        }
        else
        {
            markForExploration = begin;
        }
    }
    
    if(markForExploration != -1)
    {
        explored.push_back(markForExploration); //Put the starting point into the explored (result list) list
        permaExplored.push_back(markForExploration); //Placing the entry into a permanently visited list
    }

    //Quitting condition
    if(begin == end)
    {
        return;
    }

    vector<int> unexplored;
    unexplored = currentNodeEnds(edges, begin); //All nodes that connect to the starting point
    int topOfStack = -1;
    int sortingVariable;

    while(checkOrder(unexplored)) //Returns true if something still needs to be sorted
    {
        int tempVal;
        for(unsigned int i = 0; i < unexplored.size(); i++)
        {
            tempVal = i;
            for(unsigned int j = i + 1; j < unexplored.size(); j++)
            {
                if(unexplored[tempVal] > unexplored[j])
                {
                    tempVal = j;
                }
            }

            sortingVariable = unexplored[tempVal];
            unexplored[tempVal] = unexplored[i];
            unexplored[i] = sortingVariable;
        }
    }

    checkExplored(explored, unexplored); //Erases values in the unexplored list found in the explored list
    checkExplored(permaExplored, unexplored); //Erases values in the unexplored list found in the permaExplored list

    //Erasing the last element in explored if it has no kids
    while(unexplored.size() == 0)
    {
        //cout << "erasing element that has no kids: " << explored.back() << endl;

        if(explored.back() == permaBegin)
        {
            //cout << "Entry node being erased, no path" << endl;
            return;
        }

        explored.erase(explored.begin() + (explored.size() - 1)); //Erases the last element

        begin = explored.back(); //Updates begin to the new end of the chain

        unexplored = currentNodeEnds(edges, begin); //Re-creates unexplored list w/ updated begin

        while(checkOrder(unexplored)) //Sorts the new list
        {
            int tempVal;
            for(unsigned int i = 0; i < unexplored.size(); i++)
            {
                tempVal = i;
                for(unsigned int j = i + 1; j < unexplored.size(); j++)
                {
                    if(unexplored[tempVal] > unexplored[j])
                    {
                        tempVal = j;
                    }
                }

                sortingVariable = unexplored[tempVal];
                unexplored[tempVal] = unexplored[i];
                unexplored[i] = sortingVariable;
            }
        }

        checkExplored(explored, unexplored); //Erases values in the unexplored list found in the explored list
        checkExplored(permaExplored, unexplored); //Erases values in the unexplored list found in the permaExplored list
    }

    //Pushes the largest current child that is in the unexplored list, and not on the stack already
    for(unsigned int i = 0; i < unexplored.size(); i++)
    {
        int checkStack = 0;
        for(unsigned int j = 0; j < stacks.size(); j++)
        {
            if(unexplored[i] == stacks[j])
            {
                checkStack++;
            }
        }

        if(checkStack == 0 && i == (unexplored.size() - 1))
        {
            //cout << "adding element to the stack: " << unexplored[i] << endl;
            stacks.push_back(unexplored[i]);
        }
    }

    //If the stack is empty, quit
    if(stacks.size() == 0)
    {
        //cout << "stack is empty" << endl;
        return;
    }

    //Taking the top of the stack, and recursively calling the DFS pathing function
    topOfStack = stacks.back();
    stacks.pop_back();
    DFSpathing(permaBegin, xx, stacks, explored, edges, topOfStack, end, permaExplored);
}

/*
======================================================================================================================================
Function : Erase Explored Nodes
Parameters : Vector of integers - node list 1, vector of integers - node list 2
Return : Nothing, void function.
Description : This function erases elements from the second list that exist in the first list.
======================================================================================================================================
*/
void checkExplored(vector<int> &exploredList, vector<int> &inexplored)
{
    for(unsigned int i = 0; i < exploredList.size(); i++)
    {
        for(unsigned int j = 0; j < inexplored.size(); j++)
        {
            if(exploredList[i] == inexplored[j])
            {
                inexplored.erase(inexplored.begin() + j);
            }
        }   
    }
}

/*
======================================================================================================================================
Function : Check Vector Order
Parameters : Vector of integers - checks order (orders smallest to largest)
Return : Boolean - True if nodes are in order, false while they are not in order.
Description : This function checks that the nodes given are in order in the vector.
======================================================================================================================================
*/
bool checkOrder(vector<int> testVec)
{
    for(unsigned int i = 0; i < testVec.size(); i++)
    {
        for(unsigned int j = i; j < testVec.size(); j++)
        {
            if(testVec[i] > testVec[j])
            {
                return true;
            }
        }
    }

    return false;
}

/*
======================================================================================================================================
Function : Breadth First Search
Parameters : Vector of vector of integers - creates matrix, integer - number of nodes in total, vector of connections, 
             integer - beginning node, integer ending node
Return : Vector of integers - final path for BFS
Description : This function creates variables needed for the recursive calls the BFS pathing function.
======================================================================================================================================
*/
vector<int> BFS(vector<vector<int> > matrix, int nodes, vector<edge> edges, int beginning, int ending)
{
    auto BFStime = chrono::high_resolution_clock::now();
    cout << endl << endl << "            Breadth First Search" << endl;
    cout <<                 "            --------------------" << endl;
    vector<int> totalPath;
    vector<int> queue;
    vector<int> visited;
    vector<int> permaVisit;
    int permaBegin = beginning;

    queue.push_back(beginning);

    BFSpathing(permaVisit, permaBegin, edges, beginning, ending, visited, queue, totalPath);

    auto end = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration_cast<chrono::microseconds>(BFStime - end);
    cout << fixed;
    cout << setprecision(4);
    cout << setw(19);
    cout << "BFS timing: " << (elapsed.count() * -.001) << " milliseconds" << endl;

    return totalPath;
}

/*
======================================================================================================================================
Function : Determines BFS Path
Parameters : Vector of integers - permanent visited node list, integer - permanent beginning node, vector of connections, 
             integer - beginning node, integer - ending node, vector of integers - temporary visited nodes, 
             vector of integers - acts as a queue, vector of integers - acts as final path
Return : Nothing, void function.
Description : This function determines the correct path using a BFS search style.
======================================================================================================================================
*/
void BFSpathing(vector<int> &permafrust, int permafrost, vector<edge> connections, int begin, int end, vector<int> &visit, vector<int> &queue, vector<int> &path)
{
    //Pushing back the new number only if it is not already in the path
    int pathCheck = -1;
    for(unsigned int i = 0; i < path.size(); i++)
    {
        if(begin == path[i])
        {
            break;
        }
        else
        {
            pathCheck = begin;
        }
    }

    if(pathCheck != -1)
    {
        permafrust.push_back(pathCheck);
    }

    //Quitting condition
    if(begin == end)
    {
        return;
    }

    int toBePassed = -1;
    vector<int> tempConnections;
    //Add all node connections to temporary vector
    tempConnections = currentNodeEnds(connections, begin);

    int sortingVariable;
    while(checkOrder(tempConnections)) //Sorts the new list
    {
        int tempVal;
        for(unsigned int i = 0; i < tempConnections.size(); i++)
        {
            tempVal = i;
            for(unsigned int j = i + 1; j < tempConnections.size(); j++)
            {
                if(tempConnections[tempVal] > tempConnections[j])
                {
                    tempVal = j;
                }
            }
            sortingVariable = tempConnections[tempVal];
            tempConnections[tempVal] = tempConnections[i];
            tempConnections[i] = sortingVariable;
        }
    }

    checkExplored(path, tempConnections); //Erases values from tempConections found in the path
    checkExplored(permafrust, tempConnections); //Erases values from tempConnections found in permafrust (permanent visited list)

    int position = -1;
    while(tempConnections.size() == 0) //if the current element has no children, erase it
    {
        for(unsigned int i = 0; i < path.size(); i++)
        {
            if(path[i] == begin)
            {
                position = i;
            }
        }
    
        if(path[position] == permafrost) //prevents start node from being erased
        {
            //Entry node being erased, quitting the search
            return;
        }

        path.erase(path.begin() + position); //erasing

        if(position >= path.size()) //If the path position erased is the last position, set new position to be last position
        {   
            begin = path[position - 1];
        }
        else
        {
            begin = path[position];
        }

        tempConnections = currentNodeEnds(connections, begin);//Updates the list

        while(checkOrder(tempConnections)) //Sorts the new list
        {
            int tempVal;
            for(unsigned int i = 0; i < tempConnections.size(); i++)
            {
                tempVal = i;
                for(unsigned int j = i + 1; j < tempConnections.size(); j++)
                {
                    if(tempConnections[tempVal] > tempConnections[j])
                    {
                        tempVal = j;
                    }
                }
                sortingVariable = tempConnections[tempVal];
                tempConnections[tempVal] = tempConnections[i];
                tempConnections[i] = sortingVariable;
            }
        }

        checkExplored(path, tempConnections);
        checkExplored(permafrust, tempConnections);
    }

    //Place the smallest child that has not been visited into the queue, also has to not be in the queue already
    int checkQueue = 0;
    for(unsigned int j = 0; j < queue.size(); j++)
    {
        if(tempConnections[0] == queue[j])
        {
            checkQueue++;
        }
    }

    if(checkQueue == 0)
    {
        queue.push_back(tempConnections[0]);
    }

    //Quit condition -- If the queue is empty, we are done.
    if(queue.size() == 0)
    {
        return;
    }
 
    //Grab the front of the queue for the recursive call
    toBePassed = queue.front();
    queue.erase(queue.begin());
    path.push_back(toBePassed);

    //Recursive call
    BFSpathing(permafrust, permafrost, connections, toBePassed, end, visit, queue, path);
}

/*
======================================================================================================================================
Function : Creates Adjecency Matrix
Parameters : Integer - number of nodes, vector of connections
Return : Adjecency Matrix
Description : This function creates and prints an adjecency matrix.
======================================================================================================================================
*/
vector<vector<int> > adjecencyMatrix(int nodes, vector<edge> edges)
{
    vector<vector<int> > theMatrix;
    cout << endl;
    cout << "    ";
    for(int i = 0; i < nodes; i++)
    {
        cout << setw(3) << i + 1 << "  ";
    }

    cout << endl;
    cout << "  +";
    for(int i = 0; i < nodes; i++)
    {
        cout << setw(5) << "---";
    }
    cout << "  +";
    cout << endl;

    theMatrix.resize(nodes);
    for(unsigned int i = 0; i < theMatrix.size(); i++)
    {   
        theMatrix[i].resize(nodes);
    }

    for(int i = 1; i <= nodes; i++)
    {
        cout << setw(2) << i;
        cout << "| ";
        for(int j = 1; j <= nodes; j++)
        {
            cout << setw(3) << doesConnectionExist(edges, i, j) << "  ";
            theMatrix[i - 1][j - 1] = doesConnectionExist(edges, i, j);

            if(j == nodes)
            {
                cout << " |";
            }
        }

        cout << endl;
    }

    cout << "  +";
    for(int i = 0; i < nodes; i++)
    {
        cout << setw(5) << "---";
    }
    cout << "  +";
    cout << endl;

    cout << endl;

    return theMatrix;
}

/*
======================================================================================================================================
Function : Does Connection Exist
Parameters : Vector of connections, integer - test node 1, integer test node 2
Return : Nothing, void function.
Description : This function determines whether or not test node 1 & 2 share a connection.
======================================================================================================================================
*/
int doesConnectionExist(vector<edge> testList, int test1, int test2)
{
    for(unsigned i = 0; i < testList.size(); i++)
    {
        if(testList[i].valueOne == test1 && testList[i].valueTwo == test2)
        {
            return testList[i].time;
        }

        else if(testList[i].valueOne == test2 && testList[i].valueTwo == test1)
        {
            return testList[i].time;
        }
    }

    return 0;
}

/*
======================================================================================================================================
Function : Current Node Ends
Parameters : Vector of connections, integer - test node 1
Return : Vector of integers - list of nodes that connect to node 1
Description : This function returns the connected nodes of the selected node
======================================================================================================================================
*/
vector<int> currentNodeEnds(vector<edge> testList, int test1)
{
    vector<int> tempList;

    for(unsigned i = 0; i < testList.size(); i++)
    {
        if(testList[i].valueOne == test1)
        {
            tempList.push_back(testList[i].valueTwo);
        }

        if(testList[i].valueTwo == test1)
        {
            tempList.push_back(testList[i].valueOne);
        }
    }

    return tempList;
}

/*
======================================================================================================================================
Function : Calculates Propagation Time
Parameters : Vector of connections, vector of integers - path
Return : Integer - total propagation time of path
Description : This function determines the propagation time of the path.
======================================================================================================================================
*/
int calcPropTime(vector<edge> edg, vector<int> path)
{
    int totalPropTime = 0;
    //Calculate propagation time in milliseconds
    for(unsigned int i = 0; i < path.size(); i++)
    {
        if(i == path.size() - 1)
        {
            break;   
        }
        else
        {
            totalPropTime += doesConnectionExist(edg, path[i], path[i + 1]);
        }
    }

    return totalPropTime ;
}

/*
======================================================================================================================================
Function : Header
Parameters : None
Return : Nothing, void function.
Description : This function places a header wherever called.
======================================================================================================================================
*/
void header()
{
    cout << "+- - - - - - - - - - - - - - - - - - - - - - - +\n";
    cout << "|       Computer Science and Engineering       |\n";
    cout << "|      CSCE 2100 - Computer Foundations 1      |\n";
    cout << "|   Read Ballew rlb0422 readballew@my.unt.edu  |\n";
    cout << "|   Thien-An Vu tdv0025 thien-anvu@my.unt.edu  |\n";
    cout << "+- - - - - - - - - - - - - - - - - - - - - - - +\n";

    return;
}