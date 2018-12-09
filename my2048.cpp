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

class grid //格子 
{
public:
	int value;
	bool blocked;		
	grid(){
		value = 0;
		blocked = false;	
	}
};

class game
{
public:
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
		if( this->win ) s = "You've made it!";
		cout << s << endl << endl;
    }	

	grid** board;
	grid** Bboard;
protected:
	int score;
	bool full; //若盤面滿了 
	bool win; //當出現2048時， 為true 
	bool moved; //已移動 (避免輸入錯的方向) 
	int level;
	
	void undo(grid** board, grid** Bboard) {
		for( int i = 0; i < level; i++ )
		    for( int j = 0; j < level; j++ ) {
		    	board[j][i] = Bboard[j][i];
			}    	
	}	 
    virtual void drawBoard() = 0;
    virtual void waitKey() //等待玩家輸入方向 
    {
    	
		moved = false; //尚未移動 
		char c; 
		cout << "(W)Up (S)Down (A)Left (D)Right \n\n(Q)Quit"; 
		c = getch(); 
		switch( c )	{
		    case 'W': move( UP, board, moved, score );break;
		    case 'A': move( LEFT, board, moved, score ); break;
		    case 'S': move( DOWN, board, moved, score ); break;
		    case 'D': move( RIGHT, board, moved, score ); break;
		    
		    case 'R': undo(board, Bboard); break;
		    case 'Q': full = true;
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
    void addGrid(grid** board) //增加一個2或4在空格 
    {
		for( int i = 0; i < level; i++ )
		    for( int j = 0; j < level; j++ )
				if( !board[j][i].value ) {
				    int a, b;
				    do
				    { a = rand() % level; b = rand() % level; }
				    while( board[a][b].value ); //直到找到空的格，再進行下一步 
		 
				    int s = rand() % 100;
				    if( s > 89 ) board[a][b].value = 4; //有10%的機率出現4 
				    else board[a][b].value = 2;
				    if( canMove(board) ) return;
				}
		full = true;
    }	
    bool canMove(grid** board) //若還有空的格子，或可以合併，為true 
    {
		for( int i = 0; i < level; i++ )
		    for( int j = 0; j < level; j++ )
			if( !board[j][i].value ) return true;
	 
		for( int i = 0; i < level; i++ )
		    for( int j = 0; j < level; j++ ) { //若有相鄰兩個相等(可以合併) 
				if( canCombine( j + 1, i, board[j][i].value, board ) ) return true;
				if( canCombine( j - 1, i, board[j][i].value, board ) ) return true;
				if( canCombine( j, i + 1, board[j][i].value, board ) ) return true;
				if( canCombine( j, i - 1, board[j][i].value, board ) ) return true;
		    }
		return false;
    }
    bool canCombine( int x, int y, int v, grid** board )
    {
		if( x < 0 || x > level-1 || y < 0 || y > level-1 ) return false; //若超過邊界 
		return board[x][y].value == v; // 
    }
    virtual void move( Direct d, grid** board, bool& moved, int& score )
    {
    	backup(board, Bboard);
		switch( d )	{
		    case UP:
		    	for( int x = 0; x < level; x++ )	{
				    int y = 1;
				    while( y < level ) {
						if( board[x][y].value ) 
							moveVert( x, y, -1, board, moved, score ); 
						y++;
					}
				}
				break;
		    case DOWN:
				for( int x = 0; x < level; x++ )	{
				    int y = level-2;
				    while( y >= 0 ) { 
						if( board[x][y].value ) 
							moveVert( x, y, 1, board, moved, score ); 
						y--;
					}
				}
				break;
		    case LEFT:
				for( int y = 0; y < level; y++ )	{
				    int x = 1;
				    while( x < level ) {
						if( board[x][y].value ) 
							moveHori( x, y, -1, board, moved, score ); 
						x++;
					}
				}
				break;
		    case RIGHT:
				for( int y = 0; y < level; y++ )	{
				    int x = level-2;
				    while( x >= 0 ) {
						if( board[x][y].value ) 
							moveHori( x, y, 1, board, moved, score ); 
						x--;
					}
				}
		}
    }    
    void moveVert( int x, int y, int d, grid** board, bool& moved, int& score ) //垂直移動 
    {
		if( board[x][y + d].value && board[x][y + d].value == board[x][y].value && !board[x][y].blocked && !board[x][y + d].blocked  ) //目標格不為0，且和本格相等，合併 
		{
		    board[x][y].value = 0;
		    board[x][y + d].value *= 2;
		    score += board[x][y + d].value;
		    board[x][y + d].blocked = true; //此回合內，目標格不能再和其他合併 
		    moved = true;
		}
		else if( !board[x][y + d].value && board[x][y].value ) //移動到空格 
		{
		    board[x][y + d].value = board[x][y].value;
		    board[x][y].value = 0;
		    moved = true;
		}
		if( d > 0 ) { if( y + d < level-1 ) moveVert( x, y + d,  1, board, moved, score ); }
		else        { if( y + d > 0 ) moveVert( x, y + d, -1, board, moved, score ); }
    }
    void moveHori( int x, int y, int d, grid** board, bool& moved, int& score ) //水平移動 
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

class game1 : public game //單人模式 
{
private:
	void drawBoard() {
		system( "cls" ); //清空視窗 
		cout << "SCORE: " << score << endl << endl;
		
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
		    
		    for( int j = 0; j < level; j++ ) {
				if( !board[j][i].value ) cout << setw( 4 ) << " ";
				else {
					if (board[j][i].value == max) SetColor(14); 
					else if (board[j][i].value == max/2) SetColor(10);
					else if (board[j][i].value == max/4) SetColor(11);
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

class game2 : public game //雙人模式
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
		while( true )
		{
		    if( moved ) addGrid(board); 
			if( moved2 ) addGrid2(board2);
		    drawBoard(); 
		    if( full || full2) break;
		    
		    for (int i=0; i<level; i++)
		    	for (int j=0; j<level; j++) {
		    		if (board[j][i].value >= 2048) 
		    			win = true;		    		
		    		if (board2[j][i].value >= 2048) 
		    			win2 = true;			    		
				}
		    if (win || win2) break;
		    
		    waitKey();
		}
		string s;
		if (full2 || win) s = "Player1 win!                        Player2 lose!";
		else if (full || win2) s = "Player1 lose!                        Player2 win!";
		cout << s << endl << endl;
    }
    void move( Direct d, grid** board, bool& moved, int& score )
    {
    	backup(board, Bboard);
    	backup(board2, Bboard2);
		switch( d )	{
		    case UP:
		    	for( int x = 0; x < level; x++ )	{
				    int y = 1;
				    while( y < level ) {
						if( board[x][y].value ) 
							moveVert( x, y, -1, board, moved, score ); 
						y++;
					}
				}
				break;
		    case DOWN:
				for( int x = 0; x < level; x++ )	{
				    int y = level-2;
				    while( y >= 0 ) { 
						if( board[x][y].value ) 
							moveVert( x, y, 1, board, moved, score ); 
						y--;
					}
				}
				break;
		    case LEFT:
				for( int y = 0; y < level; y++ )	{
				    int x = 1;
				    while( x < level ) {
						if( board[x][y].value ) 
							moveHori( x, y, -1, board, moved, score ); 
						x++;
					}
				}
				break;
		    case RIGHT:
				for( int y = 0; y < level; y++ )	{
				    int x = level-2;
				    while( x >= 0 ) {
						if( board[x][y].value ) 
							moveHori( x, y, 1, board, moved, score ); 
						x--;
					}
				}
		}
    }    
    grid **board2;
    grid **Bboard2;
private:
	bool moved2, win2, full2;
	int score2;
	
	void drawBoard() {
		system( "cls" ); //清空視窗 
		cout << "SCORE1: " << score << "                          SCORE2: " << score2 <<endl << endl;
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
					cout << setw( 4 ) << board2[j][i].value;
					SetColor();
				}
				cout << " | ";
		    }
					    
		    cout << endl;
		}
		for (int j=0; j < level-1; j++) {
			cout << "+------";
			if (j == level-2) 
				cout << "+------+" << "      ";
		} 
		for (int j=0; j < level-1; j++) {
			cout << "+------";
			if (j == level-2) 
				cout << "+------+" << endl << endl;
		} 	
    }
	void waitKey() //等待玩家輸入方向 
    {
		moved = false; //尚未移動 
		moved2 = false;
		char c; 
		cout << "(W)Up (S)Down (A)Left (D)Right     (I)Up (K)Down (J)Left (L)Right \n\n"; 
		cout << "(R)Undo                            (U)Undo\n\n";
		cout << "(Q)Quit\n";
		c = getch(); 
		switch( c )	{
		    case 'W': move( UP, board, moved, score );break;
		    case 'A': move( LEFT, board, moved, score ); break;
		    case 'S': move( DOWN, board, moved, score ); break;
		    case 'D': move( RIGHT, board, moved, score );break;
		    
		    case 'I': move( UP, board2, moved2, score2 );break;
		    case 'J': move( LEFT, board2, moved2, score2 ); break;
		    case 'K': move( DOWN, board2, moved2, score2 ); break;
		    case 'L': move( RIGHT, board2, moved2, score2 ); break;	    
		    
		    case 'R': undo( board, Bboard); break;
		    case 'U': undo( board2, Bboard2); break;
		    case 'Q': full = true;
		}
		for( int i = 0; i < level; i++ )
		    for( int j = 0; j < level; j++ ) {
		    	board[j][i].blocked = false;
		    	board2[j][i].blocked = false;
			}
    }
    void addGrid2(grid**board2) //增加一個2或4在空格 
    {
		for( int i = 0; i < level; i++ )
		    for( int j = 0; j < level; j++ )
				if( !board2[j][i].value ) {
				    int a, b;
				    do
				    { a = rand() % level; b = rand() % level; }
				    while( board2[a][b].value ); //直到找到空的格，再進行下一步 
		
				    int s = rand() % 100;
				    if( s > 89 ) board2[a][b].value = 4; //有10%的機率出現4 
				    else board2[a][b].value = 2;
				    if( canMove(board2) ) return;
				}
		full2 = true;
    }	
};

int main()
{
	char ans;
	cout << "要開始新局嗎? (Y/N) \n";
	cin >> ans;		
	while (ans == 'Y' || ans == 'y')	{
		int mode;
		cout << "請輸入遊戲模式(1:單人/2:雙人) \n";
		cin >> mode ;
		if (mode == 1)	{
			int level;
			SetColor(15);
			cout << "遊戲說明：輸入等級n，即n*n的遊戲盤面。\n當遊戲盤面無法再移動時則遊戲結束，達到2048則過關。\n"; 
			cout << "遊戲盤面中，最大值的數字為黃色，(最大值/2)的數字為綠色\n，(最大值/4)的數字為藍色\n\n";
			SetColor();
			cout << "請輸入遊戲等級(建議等級：4~10)\n";
			cin >> level;
			if (level > 10) {
				cout << "超過遊戲等級上限，請重新輸入\n";
				cin >> level;  
			} 
			
		    srand( static_cast<int>( time( NULL ) ) );
		    
		    game1* g1; 
		    g1 = new game1(level);
			g1->loop();			
		}
		else if (mode == 2)	{
			SetColor(15);
			cout << "遊戲說明：雙方皆有4*4的遊戲盤面。\n當遊戲盤面無法再移動時或另一方達到2048時，由另一方獲勝。\n";
			cout << "雙方的遊戲盤面中，最大值的數字為黃色，(最大值/2)的數字為綠色\n，(最大值/4)的數字為藍色\n\n";
			SetColor();
		    srand( static_cast<int>( time( NULL ) ) );
		    game2 g2; 
			g2.loop();			
		}
		else if (mode == 11) {
			int level;
			cout << "請輸入遊戲等級(建議等級：4~10)\n";
			cin >> level;
			
		    srand( static_cast<int>( time( NULL ) ) );
		    
		    game1* g1; 
		    g1 = new game1(level);
		    cout << "請設定盤面\n";
		    for (int i=0; i<level; i++)
		    	for (int j=0; j<level; j++)
		    		cin >> g1->board[j][i].value;		    
			g1->loop();	
		}
		else if (mode == 22) { //設定雙人 
		    srand( static_cast<int>( time( NULL ) ) );
		    game2 g2; 
		    cout << "請設定盤面\n";
		    for (int i=0; i<4; i++)
		    	for (int j=0; j<4; j++)
		    		cin >> g2.board[j][i].value;
		    for (int i=0; i<4; i++)
		    	for (int j=0; j<4; j++)
		    		cin >> g2.board2[j][i].value;
			g2.loop();				
		}
		else cout << "輸入的遊戲模式錯誤\n"; 
		ans = 0;
		cout << "要開始新局嗎? (Y/N) \n";
		cin >> ans;		
	}
	cout << "Thank you for playing! \n";
	return 0;
}
