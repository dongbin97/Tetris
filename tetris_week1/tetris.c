﻿#include "tetris.h"

static struct sigaction act, oact;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
	nextBlock[2]=rand()%7;

	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' ');
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
	DrawBox(9, WIDTH + 10, 4, 8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(15,WIDTH+10);
	printw("SCORE");
	DrawBox(16,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(17,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
	for (i = 0; i < 4; i++) {
		move(10 + i, WIDTH + 13);
		for (j = 0; j < 4; j++) {
			if (block[nextBlock[2]][0][i][j] == 1) {
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	int y, x;
	for (y = 0; y < 4; y++) {
		for (x = 0; x < 4; x++) {
			if (block[currentBlock][blockRotate][y][x] == 1) {
				if (f[blockY + y][blockX + x] == 1)
					return 0;
				if (blockY + y >= HEIGHT)
					return 0;
				if (blockX + x<0 || blockX+x>=WIDTH)
					return 0;
			}
		}
	}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH], int command, int currentBlock, int blockRotate, int blockY, int blockX) {

	//이전 블록 정보를 찾는다.
	int prevY = blockY, prevX = blockX, prevrt = blockRotate;
	int prevshadowY;
	switch (command) {
	case KEY_UP:
		prevrt = (prevrt +3) % 4;
		break;
	case KEY_DOWN:
		prevY -= 1;
		break;
	case KEY_RIGHT:
		prevX -= 1;
		break;
	case KEY_LEFT:
		prevX += 1;
		break;
	default:
		break;
	}

	//이전 그림자의 y좌표를 찾는다
	prevshadowY = prevY;
	while (CheckToMove(field, currentBlock, prevrt, prevshadowY, prevX))
		prevshadowY++;
	prevshadowY -= 1;

	// 이전 블록과 그림자 정보를 지운다.
	int y, x;
	for (y = 0; y < 4; y++) {
		for (x = 0; x < 4; x++) {
			if (block[currentBlock][prevrt][y][x] == 1) {
				if (prevY + y >= 0) {
					move(prevY + y + 1, prevX + x + 1);
					printw(".");
				}
				if (prevshadowY + y >= 0) {
					move(prevshadowY + y + 1, prevX + x + 1);//커서를 이전 그림자위 위치에 두고
					printw(".");
				}
			}
		}
	}
	
	// 새로운 블록과 그림자 정보를 그린다. 
	DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
	move(HEIGHT,WIDTH+10);//커서를 필드 밖으로 빼준다
}

void BlockDown(int sig){
	if (CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX)) {
		
		blockY += 1;
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
	}
	else {
			if (blockY == -1)	gameOver = 1;
			score+=AddBlockToField(field,nextBlock[0],blockRotate,blockY,blockX);
			score+=DeleteLine(field);

			nextBlock[0] = nextBlock[1];
			nextBlock[1] = nextBlock[2];
			nextBlock[2] = rand() % 7;
			blockRotate = 0; blockY = -1; blockX = (WIDTH / 2) - 2;
			DrawField();
			DrawNextBlock(nextBlock);
			PrintScore(score);
			
	}
	timed_out = 0;//1초가 다 지났다
	return;
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	
	//Block이 추가된 영역의 필드값을 바꾼다.
	int touched=0;
	int y, x;
	for (y = 0; y < 4; y++) 
		for (x = 0; x < 4; x++) 
			if (block[currentBlock][blockRotate][y][x] == 1) {
				f[blockY + y][blockX + x] = 1;
				if (f[blockY + y + 1][blockX + x] == 1 || blockY+y+1==HEIGHT)
					touched++;
			}
	return touched*10;
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	//필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
	int y, x,j,i,deleteline=0;
	for (y = HEIGHT; y >=0; y--) {
		for (x = 0; x < WIDTH; x++) {
			if (f[y][x] == 0)
				break;
		}
		if (x == WIDTH) {
			deleteline++;
			for (j = y; j >= 0; j--)
				for (i = 0; i < WIDTH; i++) {
					if (j == 0)
						f[j][i] = 0;
					else
						f[j][i] = f[j - 1][i];
				}
			y++;
		}
	} 
	return deleteline * deleteline * 100;

}

///////////////////////////////////////////////////////////////////////////

void DrawShadow(int y, int x, int blockID,int blockRotate){
	while (CheckToMove(field, blockID, blockRotate, y, x))
		y++; //CheckToMove함수를 이용해 아래로 내려갈 수 있는지 확인하면서 최대 내려갈 수 있는 y좌표 확인
	DrawBlock(y-1, x, blockID, blockRotate, '/');//y가 최대 내려갈 수 있는 좌표보다 1 크므로 y-1로 해서 DrawBlock
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate) {
	DrawBlock(y, x, blockID, blockRotate, ' '); //새로운 위치의 블록을 그려준다
	DrawShadow(y, x, blockID, blockRotate); //새로운 위치의 그림자를 그려준다
}

void createRankList(){
	// user code
}

void rank(){
	// user code
}

void writeRankFile(){
	// user code
}

void newRank(int score){
	// user code
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수

	// user code

	return max;
}

void recommendedPlay(){
	// user code
}
