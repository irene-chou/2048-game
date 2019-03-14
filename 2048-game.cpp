#include <time.h> //time
#include <iostream>
#include <string>
#include <iomanip> //setw
#include <cstdlib> //srand
#include <conio.h> //getch
#include <windows.h> //SetColor 

using namespace std;

enum Direct { UP, DOWN, LEFT, RIGHT };

void SetColor(int color = 7)
{
	HANDLE hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole,color);
}

class grid //��l 
{
public:
	int value;	bool blocked;		
	grid(){
		value = 0;
		blocked = false;	
	}
};

class game
{
public:
	grid** board;	grid** Bboard;
    virtual void loop()  {
		addGrid(board); 
		while( true )
		{
		    if( moved ) addGrid(board);
		    drawBoard();
		    if( full ) break;
		    
		    for (int i=0; i<level; i++)
		    	for (int j=0; j<level; j++)
		    		if (board[j][i].value >= 2048) 
		    			win = true;
		    if (win == true) break;
		    
		    waitKey();
		}
		string s = "Game Over!";
		if( this->win ) s = "You win!";
		cout << s << endl << endl;
    }		
protected:
	int score;
	bool full; //�Y�L�����F 
	bool win; //��X�{2048�ɡA ��true 
	bool moved; //�w���� (�קK��J������V) 
	int level;
	
	void undo(grid** board, grid** Bboard) {
		for( int i = 0; i < level; i++ )
		    for( int j = 0; j < level; j++ ) {
		    	board[j][i] = Bboard[j][i];
			}    	
	}	 
    virtual void drawBoard() = 0;
    virtual void waitKey() //���ݪ��a��J��V 
    {
		moved = false; //�|������ 
		char c; 
		cout << "(W)Up (S)Down (A)Left (D)Right   (R)Undo (Q)Quit\n"; 
		c = getch(); 
		switch( c )	{
		    case 'W': backup(board, Bboard); move( UP, board, moved, score );break;
		    case 'A': backup(board, Bboard); move( LEFT, board, moved, score ); break;
		    case 'S': backup(board, Bboard); move( DOWN, board, moved, score ); break;
		    case 'D': backup(board, Bboard); move( RIGHT, board, moved, score ); break;
		    
		    case 'R': undo(board, Bboard); break;
		    case 'Q': 
				cout << "Do you really want to leave?(Y/N)\n";
				char c;	cin >> c;
				if (c == 'Y') full = true;
		}
		for( int i = 0; i < level; i++ )
		    for( int j = 0; j < level; j++ )
				board[j][i].blocked = false;
		
    }
    void backup(grid** board, grid** Bboard) 
	{
		for( int i = 0; i < level; i++ )
		    for( int j = 0; j < level; j++ ) {
		    	Bboard[j][i] = board[j][i];
			}		
	} 
    void addGrid(grid** board) // add number 2 or 4 in an empty grid 
    {
		for( int i = 0; i < level; i++ )
		    for( int j = 0; j < level; j++ )
				if( !board[j][i].value ) {
				    int a, b;
				    do
				    { a = rand() % level; b = rand() % level; }
				    while( board[a][b].value ); //������Ū���A�A�i��U�@�B 
		 
				    int s = rand() % 100;
				    if( s > 89 ) board[a][b].value = 4; //��10%�����v�X�{4 
				    else board[a][b].value = 2;
				    if( canMove(board) ) return;
				}
		full = true;
    }	
    bool canMove(grid** board) // if there is at least an empty grid, or there are combinable grids, return true 
    {
		for( int i = 0; i < level; i++ )
		    for( int j = 0; j < level; j++ )
			if( !board[j][i].value ) return true;
	 
		for( int i = 0; i < level; i++ )
		    for( int j = 0; j < level; j++ ) { //�Y���۾F��Ӭ۵�(�i�H�X��) 
				if( canCombine( j + 1, i, board[j][i].value, board ) ) return true;
				if( canCombine( j - 1, i, board[j][i].value, board ) ) return true;
				if( canCombine( j, i + 1, board[j][i].value, board ) ) return true;
				if( canCombine( j, i - 1, board[j][i].value, board ) ) return true;
		    }
		return false;
    }
    bool canCombine( int x, int y, int v, grid** board ) 
    {
		if( x < 0 || x > level-1 || y < 0 || y > level-1 ) return false; //�Y�W�L��� 
		return board[x][y].value == v; // 
    }
    virtual void move( Direct d, grid** board, bool& moved, int& score ) // the user key to move
    {
		switch( d )	{
		    case UP:
		    	for( int x = 0; x < level; x++ )	{
				    int y = 1;
				    while( y < level ) {
						if( board[x][y].value ) 	moveVert( x, y, -1, board, moved, score ); 
						y++;
					}
				}
				break;
		    case DOWN:
				for( int x = 0; x < level; x++ )	{
				    int y = level-2;
				    while( y >= 0 ) { 	
						if( board[x][y].value ) 	moveVert( x, y, 1, board, moved, score ); 
						y--;
					}
				}
				break;
		    case LEFT:
				for( int y = 0; y < level; y++ )	{
				    int x = 1;
				    while( x < level ) {
						if( board[x][y].value ) 	moveHori( x, y, -1, board, moved, score ); 
						x++;
					}
				}
				break;
		    case RIGHT:
				for( int y = 0; y < level; y++ )	{
				    int x = level-2;
				    while( x >= 0 ) {
						if( board[x][y].value ) 	moveHori( x, y, 1, board, moved, score ); 
						x--;
					}
				}
		}
    }    
    void moveVert( int x, int y, int d, grid** board, bool& moved, int& score ) // vertically move 
    {
		if( board[x][y + d].value && board[x][y + d].value == board[x][y].value && !board[x][y].blocked && !board[x][y + d].blocked  ) //�ؼЮ椣��0�A�B�M����۵��A�X�� 
		{
		    board[x][y].value = 0;
		    board[x][y + d].value *= 2;
		    score += board[x][y + d].value;
		    board[x][y + d].blocked = true; //���^�X���A�ؼЮ椣��A�M��L�X�� 
		    moved = true;
		}
		else if( !board[x][y + d].value && board[x][y].value ) //���ʨ�Ů� 
		{
		    board[x][y + d].value = board[x][y].value;
		    board[x][y].value = 0;
		    moved = true;
		}
		if( d > 0 ) { if( y + d < level-1 ) moveVert( x, y + d,  1, board, moved, score ); }
		else        { if( y + d > 0 ) moveVert( x, y + d, -1, board, moved, score ); }
    }
    void moveHori( int x, int y, int d, grid** board, bool& moved, int& score ) // horizontally move
    {
		if( board[x + d][y].value && board[x + d][y].value == board[x][y].value && !board[x][y].blocked && !board[x + d][y].blocked  )	{
		    board[x][y].value = 0;
		    board[x + d][y].value *= 2;
		    score += board[x + d][y].value;
		    board[x + d][y].blocked = true;
		    moved = true;
		}
		else if( !board[x + d][y].value && board[x][y].value )	{
		    board[x + d][y].value = board[x][y].value;
		    board[x][y].value = 0;
		    moved = true;
		}
		if( d > 0 ) { if( x + d < level-1 ) moveHori( x + d, y,  1, board , moved, score); }
		else        { if( x + d > 0 ) moveHori( x + d, y, -1, board, moved, score  ); }
    }
};		

class game1 : public game // single-player mode 
{
private:
	void drawBoard() {
		system( "cls" ); //�M�ŵ��� 
		cout << "SCORE: " << score << endl << endl;
		
		//just for color
		int max = 0;
		for( int i = 0; i < level; i++ ) 
			for( int j = 0; j < level; j++ ) 
				if (board[j][i].value > max) max = board[j][i].value;
		
		for( int i = 0; i < level; i++ )
		{
			for (int j=0; j < level-1; j++) {
				cout << "+------";
				if (j == level-2) 
					cout << "+------+" << endl << "| ";
			}
		    
		    //print + color
		    for( int j = 0; j < level; j++ ) {
				if( !board[j][i].value ) cout << setw( 4 ) << " ";
				else {
					if (board[j][i].value == max) SetColor(14); 
					else if (board[j][i].value == max/2) SetColor(10);
					else if (board[j][i].value == max/4) SetColor(11);
					else SetColor(15);
					cout << setw( 4 ) << board[j][i].value;
					SetColor();
				}
				cout << " | ";
		    }
		    
		    cout << endl;
		}
		for (int j=0; j < level-1; j++) {
			cout << "+------";
			if (j == level-2) 
				cout << "+------+" << endl << endl;
		} 
    }
public:
	game1(int input_lv) {
		level = input_lv;

		board = new grid*[level];
		Bboard = new grid*[level];
		for (int i=0; i<level; i++) {	board[i] = new grid[level];	Bboard[i] = new grid[level];	}
			
		for (int i=0; i<level; i++)
			for (int j=0; j<level; j++) {	board[i][j].value = 0;	}
				
		score = 0;	full = false;	win = false;	moved = true;
	}
};

class game2 : public game // double-player mode
{
public:
	game2() {
		level = 4;
		board = new grid*[level];
		board2 = new grid*[level];
		Bboard = new grid*[level];
		Bboard2 = new grid*[level];
		for (int i=0; i<level; i++) {
			board[i] = new grid[level];		board2[i] = new grid[level];
			Bboard[i] = new grid[level];		Bboard2[i] = new grid[level];
		}
			
		for (int i=0; i<level; i++)
			for (int j=0; j<level; j++) { board[i][j].value = 0;	board2[i][j].value = 0;	}
				
		score = 0;	full = false;	win = false;	moved = true;	
		score2 = 0;	full2 = false;	win2 = false;	moved2 = true;
	}
    void loop()  {
		addGrid( board ); addGrid2( board2 );
		bool empty1 = false, empty2 = false;
		while( true )
		{
		    if( moved ) addGrid(board); 
			if( moved2 ) addGrid2(board2);
		    drawBoard(); 
		    
		    empty1 = true; empty2 = true;
		    for (int i=0; i<level; i++)
		    	for (int j=0; j<level; j++) {
		    		if (board[j][i].value >= 2048) 
		    			win = true;		    		
		    		if (board2[j][i].value >= 2048) 
		    			win2 = true;			    
					if (board[j][i].value) empty1 = false;
					if (board2[j][i].value) empty2 = false;
				}
			if( full || full2 || win || win2 || empty1 || empty2) break;
		    
		    waitKey();
		}
		string s;
		if (full2 || win || empty2) s = "Player1 win!                        Player2 lose!";
		else if (full || win2 || empty1) s = "Player1 lose!                        Player2 win!";
		cout << s << endl << endl;
    }
    
    grid **board2;    grid **Bboard2;
protected:
	bool moved2, win2, full2;	int score2;
	
	void drawBoard() {
		system( "cls" ); //�M�ŵ��� 
		cout << "SCORE1: " << setw( 5 ) << score << "                       SCORE2: " << setw( 5 ) << score2 <<endl << endl;
		int max = 0;
		for( int i = 0; i < level; i++ ) 
			for( int j = 0; j < level; j++ ) {
				if (board[j][i].value > max) max = board[j][i].value;
				if (board2[j][i].value > max) max = board2[j][i].value;
			}

		for( int i = 0; i < level; i++ )
		{
			for (int j=0; j < level-1; j++) {
				cout << "+------";
				if (j == level-2) 
					cout << "+------+" << "      ";
			} 
			for (int j=0; j < level-1; j++) {
				cout << "+------";
				if (j == level-2) 
					cout << "+------+" << endl << "| ";
			} 
					   
		    for( int j = 0; j < level; j++ ) {
				if( !board[j][i].value ) cout << setw( 4 ) << " ";
				else {
					if (board[j][i].value == max) SetColor(14); 
					else if (board[j][i].value == max/2) SetColor(10);
					else if (board[j][i].value == max/4) SetColor(11);
					else SetColor(15);
					cout << setw( 4 ) << board[j][i].value;
					SetColor();
				}
				cout << " | ";
		    }
		    cout << "     | ";
		    for( int j = 0; j < level; j++ ) {
				if( !board2[j][i].value ) cout << setw( 4 ) << " ";
				else {
					if (board2[j][i].value == max) SetColor(14); 
					else if (board2[j][i].value == max/2) SetColor(10);
					else if (board2[j][i].value == max/4) SetColor(11);
					else SetColor(15);
					cout << setw( 4 ) << board2[j][i].value;
					SetColor();
				}
				cout << " | ";
		    }
					    
		    cout << endl;
		}
		for (int j=0; j < level-1; j++) {
			cout << "+------";
			if (j == level-2) 	cout << "+------+" << "      ";
		} 
		for (int j=0; j < level-1; j++) {
			cout << "+------";
			if (j == level-2) 	cout << "+------+" << endl << endl;
		} 	
    }
	virtual void waitKey() // wait for user to key 
    {
		moved = false; //�|������ 
		moved2 = false;
		char c; 
		cout << "(W)Up (S)Down (A)Left (D)Right     (I)Up (K)Down (J)Left (L)Right \n\n"; 
		cout << "(R)Undo (Q)Quit                    (U)Undo (Q)Quit\n\n";
		c = getch(); 
		switch( c )	{
		    case 'W': backup(board, Bboard); move( UP, board, moved, score ); break;
		    case 'A': backup(board, Bboard); move( LEFT, board, moved, score ); break;
		    case 'S': backup(board, Bboard); move( DOWN, board, moved, score ); break;
		    case 'D': backup(board, Bboard); move( RIGHT, board, moved, score );break;
		    
		    case 'I': backup(board2, Bboard2); move( UP, board2, moved2, score2 );break;
		    case 'J': backup(board2, Bboard2); move( LEFT, board2, moved2, score2 ); break;
		    case 'K': backup(board2, Bboard2); move( DOWN, board2, moved2, score2 ); break;
		    case 'L': backup(board2, Bboard2); move( RIGHT, board2, moved2, score2 ); break;	    
		    
		    case 'R': undo( board, Bboard); break;
		    case 'U': undo( board2, Bboard2); break;
		    case 'Q': 
				cout << "Do you really want to leave?(Y/N)\n";
				char c;	cin >> c;
				if (c == 'Y') full = true;
		}
		for( int i = 0; i < level; i++ )
		    for( int j = 0; j < level; j++ ) {
		    	board[j][i].blocked = false;
		    	board2[j][i].blocked = false;
			}
    }
    void addGrid2(grid**board2) //�W�[�@��2��4�b�Ů� 
    {
		for( int i = 0; i < level; i++ )
		    for( int j = 0; j < level; j++ )
				if( !board2[j][i].value ) {
				    int a, b;
				    do
				    { a = rand() % level; b = rand() % level; }
				    while( board2[a][b].value ); //������Ū���A�A�i��U�@�B 
		
				    int s = rand() % 100;
				    if( s > 89 ) board2[a][b].value = 4; //��10%�����v�X�{4 
				    else board2[a][b].value = 2;
				    if( canMove(board2) ) return;
				}
		full2 = true;
    }	
};
class game22 : public game2
{
public:
private:
	virtual void waitKey() //���ݪ��a��J��V 
    {
		moved = false; //�|������ 
		moved2 = false;
		char c; 
		cout << "(W)Up (S)Down (A)Left (D)Right     (I)Up (K)Down (J)Left (L)Right \n\n"; 
		cout << "(R)Undo (Q)Quit                    (U)Undo (Q)Quit\n\n";
		c = getch(); 
		switch( c )	{
		    case 'W': backup(board, Bboard); move( UP, board, moved, score );  break;
		    case 'A': backup(board, Bboard); move( LEFT, board, moved, score ); break;
		    case 'S': backup(board, Bboard); move( DOWN, board, moved, score ); break;
		    case 'D': backup(board, Bboard); move( RIGHT, board, moved, score ); interact();break;
		    
		    case 'I': backup(board2, Bboard2); move( UP, board2, moved2, score2 ); break;
		    case 'J': backup(board2, Bboard2); move( LEFT, board2, moved2, score2 ); interact2(); break;
		    case 'K': backup(board2, Bboard2); move( DOWN, board2, moved2, score2 ); break;
		    case 'L': backup(board2, Bboard2); move( RIGHT, board2, moved2, score2 ); break;	    
		    
		    case 'R': undo( board, Bboard); break;
		    case 'U': undo( board2, Bboard2); break;
		    case 'Q': 
				cout << "Do you realliy want to leave?(Y/N)\n";
				char c;	cin >> c;
				if (c == 'Y' || c == 'y') full = true;
		}
		for( int i = 0; i < level; i++ )
		    for( int j = 0; j < level; j++ ) {
		    	board[j][i].blocked = false;
		    	board2[j][i].blocked = false;
			}
    }
	void interact()
	{
		for (int y=0; y<level; y++) {
			if( board[3][y].value && board[3][y].value == board2[0][y].value && !board[3][y].blocked && !board2[0][y].blocked  ) {
				board2[0][y].value = 0;
				board[3][y].value *= 2;
				score += board[3][y].value;
			}
		}
	}
	void interact2()
	{
		for (int y=0; y<level; y++) {
			if( board2[0][y].value && board2[0][y].value == board[3][y].value && !board2[0][y].blocked && !board[3][y].blocked  ) {
				board[3][y].value = 0;
				board2[0][y].value *= 2;
				score += board2[0][y].value;
			}
		}
	}	
};


int main()
{
	char ans;
	cout << "Welcome to play 2048!\nDo you want to start a new game? (Y/N) \n";
	cin >> ans;		
	while (ans != 'N')	{
		int mode;
		cout << "Please choose game mode (1: 1-player / 2: 2-player) \n";
		cin >> mode ;
		if (mode == 1)	{
			int level;
			SetColor(15);
			cout << "Enter level n, then you will get the gameboard in n*n.\n\n"; 
			SetColor();
			cout << "Enter the level you want (recommend�G4 ~ 9): \n";
			cin >> level;
			if (level > 9) {
				cout << "Over the limit, please enter again\n";
				cin >> level;  
			} 
			
		    srand( static_cast<int>( time( NULL ) ) );
		    
		    game1* g1; 
		    g1 = new game1(level);
			g1->loop();			
			delete [] g1;
		}
		else if (mode == 2)	{
			SetColor(15);
			cout << "Interaction mode: You can get the other's number.'\n\nDo you want to play the interaction mode?(Y/N)\n";
			char c;	cin >> c;
			SetColor();
			
			if (c == 'Y' || c == 'y') {
			    srand( static_cast<int>( time( NULL ) ) );
			    game22* g22;
				g22 = new game22; 
				g22->loop();		
				delete [] g22;			
			}
			else if (c == 'N' || c == 'n') {
			    srand( static_cast<int>( time( NULL ) ) );
			    
			    game2* g2;
				g2 = new game2;
				g2->loop();	
				delete [] g2;				
			}	
			else cout << "Wrong mode!\n";		
		}
		/* TEST MODE FOR CREATOR
		else if (mode == 10) { 
			int level;
			cout << "�п�J�C������(��ĳ���šG4~9)\n";
			cin >> level;
			
		    srand( static_cast<int>( time( NULL ) ) );
		    
		    game1* g1; 
		    g1 = new game1(level);
		    cout << "�г]�w�L��\n";
		    for (int i=0; i<level; i++)
		    	for (int j=0; j<level; j++)
		    		cin >> g1->board[j][i].value;		    
			g1->loop();
			delete [] g1;	
		}
		else if (mode == 20) { // 
		    srand( static_cast<int>( time( NULL ) ) );
		    game2* g2; 
		    g2 = new game2;
		    cout << "�г]�w�L��\n";
		    for (int i=0; i<4; i++)
		    	for (int j=0; j<4; j++)
		    		cin >> g2->board[j][i].value;
		    for (int i=0; i<4; i++)
		    	for (int j=0; j<4; j++)
		    		cin >> g2->board2[j][i].value;
			g2->loop();
			delete [] g2;				
		}
		else if (mode == 220) { // 
		    srand( static_cast<int>( time( NULL ) ) );
		    game22* g22; 
		    g22 = new game22;
		    cout << "�г]�w�L��\n";
		    for (int i=0; i<4; i++)
		    	for (int j=0; j<4; j++)
		    		cin >> g22->board[j][i].value;
		    for (int i=0; i<4; i++)
		    	for (int j=0; j<4; j++)
		    		cin >> g22->board2[j][i].value;
			g22->loop();
			delete [] g22;				
		}
		*/
		else cout << "Wrong mode!\n"; 
		ans = 0;
		cout << "Do you want to start a new game? (Y/N) \n";
		cin >> ans;		
	}
	cout << "Thank you for playing! \n";
	return 0;
}
