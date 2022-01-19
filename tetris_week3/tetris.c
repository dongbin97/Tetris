#include "tetris.h"

static struct sigaction act, oact;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	recRoot = (RecNode*)malloc(sizeof(RecNode));
	recRoot->lv =0;
	recRoot->score = 0;
	recRoot->recField = field;
	CreateTree(recRoot);


	srand((unsigned int)time(NULL));

	while(!exit){
		createRankList();
		clear();
		
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_RECOMMENDEDPLAY:recommendedPlay(); break;
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
	for(int k=0;k<VISIBLE_BLOCK-1;k++)
		nextBlock[k] = rand() % 7;
	
	modified_recommend(recRoot);

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

			
			for (int i = 0; i < VISIBLE_BLOCK - 1; i++)
				nextBlock[i] = nextBlock[i + 1];
			nextBlock[VISIBLE_BLOCK - 1] = rand() % 7;

			
			modified_recommend(recRoot);
			//recommend(recRoot);

			blockRotate = 0; blockY = -1; blockX = (WIDTH / 2) - 2;
			DrawNextBlock(nextBlock);
			PrintScore(score);
			DrawField();
			DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
			
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

/////////////////////////1주차 과제////////////////////////////////////////

void DrawShadow(int y, int x, int blockID,int blockRotate){
	while (CheckToMove(field, blockID, blockRotate, y, x))
		y++; //CheckToMove함수를 이용해 아래로 내려갈 수 있는지 확인하면서 최대 내려갈 수 있는 y좌표 확인
	DrawBlock(y-1, x, blockID, blockRotate, '/');//y가 최대 내려갈 수 있는 좌표보다 1 크므로 y-1로 해서 DrawBlock
}


/////////////////////////2주차 실습//////////////////////////////////////////

void createRankList(){
	FILE *fp;
	if ((fp = fopen("rank.txt", "r")) == NULL) {
		fp = fopen("rank.txt", "w");
		fprintf(fp, "0\n");
		fclose(fp);
		ranking = NULL;
		return;
	}
	else {

		char str[NAMELEN];
		int scor;
		int count;
		fscanf(fp, "%d", &ranknum);

		ranking = NULL;
		Node* newnode;

		for (count = 0; count < ranknum; count++) {
			fscanf(fp, "%s %d", str, &scor);
			newnode = CreateNode(str, scor);
			MakeList(&ranking, newnode);
		}
		fclose(fp);
	}
}

Node* CreateNode(char* str, int scor) {
	Node* temp = (Node*)malloc(sizeof(Node));
	temp->link = NULL;
	strcpy(temp->name, str);
	temp->score = scor;

	return temp;
}

void MakeList(Node** head, Node* newnode) {
	if (*head == NULL)
		*head = newnode;
	else {
		Node* temp = *head;
		while (temp->link != NULL)
			temp = temp->link;
		temp->link = newnode;
	}
}

void rank(){
	clear();
	printw("1. list ranks from X to Y\n"); 
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");

	int command;
	command = wgetch(stdscr);
	switch (command) {
	case '1': PrintXtoY(); break;
	case '2': SearchName(); break;
	case '3': DeleteRank(); break;
	default: break;
	}
}

void writeRankFile(){
	FILE* fp;
	fp = fopen("rank.txt","w");

	fprintf(fp, "%d\n", ranknum);

	Node* temp=ranking;

	while (temp) {
		fprintf(fp, "%s %d\n", temp->name, temp->score);
		temp = temp->link;
	}
	fclose(fp);
}

void Insert(Node** head, Node* newnode) {

	Node* current;
	Node* previous;

	if (*head == NULL) { //empty 노드일 경우 첫번째 노드가 생성될 때
		*head = newnode;
		//newnode->link = NULL;
	}
	else {
		current = *head;
		previous = NULL;

		while (current != NULL) {
			if (newnode->score < current->score) {
				previous = current;
				current = current->link;
			}
			else break;
		}
		if (previous != NULL) { //리스트의 중간에 newnode 삽입
			newnode->link = previous->link;
			previous->link = newnode;
		}
		else { //리스트 제일 처음에 newnode를 삽입
			*head = newnode;
			newnode->link = current;
		}
	}
}

void newRank(int score){
	clear();

	char username[NAMELEN];
	Node* newnode;

	printw("your name: ");
	echo();
	scanw("%s", username);
	noecho();

	newnode = CreateNode(username, score);

	Insert(&ranking, newnode);
	ranknum++;
	writeRankFile();

	return;
}

void PrintXtoY() {
	int X = 1, Y = ranknum, count = 0;
	echo();
	printw("X: ");
	scanw("%d", &X);
	printw("Y: ");
	scanw("%d", &Y);
	noecho();

	printw("       name      |   score   \n");
	printw("-----------------------------\n");

	if (X > Y) {
		printw("search failure : no rank in the list");
		refresh();
		getch();
		return;
	}

	Node* temp = ranking;
	count++;
	while (temp){
			if (X <= count && count <= Y)
				printw(" %-16s| %d\n", temp->name, temp->score);
			else if (count > Y)
				break;
			temp = temp->link;
			count++;
	}
	refresh();
	getch();
	return;
}
////////////////////// 2주차 과제///////////////////////////////////////
void SearchName() {

	char str[NAMELEN];
	int flag = 0;

	printw("Input the name: "); //탐색할 이름을 입력받음
	echo();
	scanw("%s", str);
	noecho();

	printw("       name      |   score   \n");
	printw("-----------------------------\n");

	Node* temp = ranking;

	while (temp) {
		if (strcmp(temp->name, str) == 0) { // temp의 name과 str이 일치하면
			printw(" %-16s| %d\n", temp->name, temp->score); //해당 데이터를 출력
			flag = 1; //해당 이름을 찾았음을 알려줌
		}
		temp = temp->link;
	}
	if(!(flag)) printw("search failure : no information in the list\n"); //해당이름을 못찾으면 메시지 출력

	refresh();
	getch();
	return;
}

void DeleteRank() {
	int rankno;
	int count;

	printw("Input the rank: ");
	echo();
	scanw("%d", &rankno); //삭제할 순위를 입력받음
	noecho();

	if (rankno <1 || rankno>ranknum) { //입력받은 순위가 범위를 벗어난 경우
		printw("\nsearch failure :the rank not in the list");
		refresh();
		getch();
		return;
	}
	else if (rankno == 1) // 입력받은 순위가 1인경우
		ranking = ranking->link;
	else { //그 이외의 경우
		Node* current = ranking;
		Node* previous;
		for (count = 1; count < rankno; count++) { //해당 순위를 찾아감
			previous = current;
			current = current->link;
		}
		previous->link = current->link; //current가 삭제할 순위이므로 삭제
		free(current);
	}
	ranknum--; // 삭제를 완료했으므로 ranknum을 1감소
	
	printw("\nresult: the rank deleted\n"); //완료메시지 출력

	writeRankFile(); //갱신된 정보를 rank.txt에 저장한다.
	
	refresh();
	getch();
	return;
}

/////////////////////////3주차 실습/////////////////////////////////
void DrawRecommend(int y, int x, int blockID,int blockRotate){
		DrawBlock(y, x, blockID, blockRotate, 'R');
}

int getrotate(int block) {
	switch (block) {
	case 0:
	case 5:
	case 6: return 2;
	case 1:
	case 2:
	case 3: return 4;
	case 4: return 1;
	}
}

void fieldcopy(char from[HEIGHT][WIDTH], char to[HEIGHT][WIDTH]) {
	int i, j;
	for (i = 0; i < HEIGHT; i++)
		for (j = 0; j < WIDTH; j++)
			to[i][j] = from[i][j];
}

void CreateTree(RecNode* root) {
	int i;
	RecNode** child = root->child;
	for (i = 0; i < CHILDREN_MAX; ++i) {
		child[i] = (RecNode*)malloc(sizeof(RecNode));
		child[i]->lv = root->lv + 1;
		child[i]->recField = (char(*)[WIDTH])malloc(sizeof(char) * HEIGHT * WIDTH);
		if (child[i]->lv < VISIBLE_BLOCK)
			CreateTree(child[i]);
	}
}
void GetXBound(int blocknum, int rotate, int* Xstart, int* Xfinish) {
	int y, x, start = 3, finish = 0;

	for (y = 0; y < BLOCK_HEIGHT; y++) {
		for (x = 0; x < BLOCK_WIDTH; x++) {
			if (block[blocknum][rotate][y][x] == 1) break;
		}
		if (x < start) start = x;
	}
	*Xstart = 0-start;

	for (y = 0; y < BLOCK_HEIGHT; y++) {
		for (x = BLOCK_WIDTH - 1; x >= 0; x--) {
			if (block[blocknum][rotate][y][x] == 1) break;
		}
		if (x > finish) finish = x;
	}
	*Xfinish = WIDTH - (finish + 1);
	return;
}
int modified_recommend(RecNode* root) {
	int level = root->lv;
	int recr, recx, recy;
	int max = 0;
	int i = 0;
	RecNode** child = root->child;
	int Xstart, Xfinish;
	int touch = 0, dline = 0;
	int nodescore;
	nodecount = 1;

	int rotate = getrotate(nextBlock[level]);

	for (recr = 0; recr < rotate; recr++) {

		GetXBound(nextBlock[level], recr, &Xstart, &Xfinish);
		for (recx =Xstart; recx <= Xfinish; recx++) {
			nodecount++;
			//root의 필드 복사
			fieldcopy(root->recField, child[i]->recField);
			//현재 recx에서 갈수 있는 위치 확인
			recy = 0;
			while (CheckToMove(child[i]->recField, nextBlock[level], recr, recy, recx))
				recy++;
			recy--;
			//필드에 쌓였을때의 점수계산
			touch = AddBlockToField(child[i]->recField, nextBlock[level], recr, recy, recx);
			dline= DeleteLine(child[i]->recField);
			child[i]->score = root->score + touch + dline;

			if (child[i]->lv = VISIBLE_BLOCK) {
				nodescore = child[i]->score;
			}
			else
				nodescore = recommend(child[i]);

			if (max < nodescore) {
				max = nodescore;
				if (level == 0) { //최종 추천값
					recommendX = recx;
					recommendY = recy;
					recommendR = recr;
				}
			}
			
			i++;
		}
	}
	
	return max;
}

int recommend(RecNode* root) {
	int level = root->lv;
	int recr, recx, recy;
	int max = 0;
	int i = 0;
	RecNode** child = root->child;
	int Xstart, Xfinish;
	int touch = 0, dline = 0;
	int nodescore;

	for (recr = 0; recr < 4; recr++) {

		GetXBound(nextBlock[level], recr, &Xstart, &Xfinish);
		for (recx = Xstart; recx <= Xfinish; recx++) {
			//root의 필드 복사
			fieldcopy(root->recField, child[i]->recField);
			//현재 recx에서 갈수 있는 위치 확인
			recy = 0;
			while (CheckToMove(child[i]->recField, nextBlock[level], recr, recy, recx))
				recy++;
			recy--;
			//필드에 쌓였을때의 점수계산
			touch = AddBlockToField(child[i]->recField, nextBlock[level], recr, recy, recx);
			dline = DeleteLine(child[i]->recField);
			child[i]->score = root->score + touch + dline;

			if (child[i]->lv = VISIBLE_BLOCK) {
				nodescore = child[i]->score;
			}
			else
				nodescore = recommend(child[i]);

			if (max < nodescore) {
				max = nodescore;
				if (level == 0) { //최종 추천값
					recommendX = recx;
					recommendY = recy;
					recommendR = recr;
				}
			}
			i++;
		}
	}

	return max;
}
void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate) {
	DrawRecommend(recommendY, recommendX, nextBlock[0], recommendR);
	DrawBlock(y, x, blockID, blockRotate, ' '); //새로운 위치의 블록을 그려준다
	DrawShadow(y, x, blockID, blockRotate); //새로운 위치의 그림자를 그려준다
}
void autoplay() {
	int i,j,k;
	DrawBlockWithFeatures(-1, WIDTH / 2 - 2, nextBlock[0], 0);
		if (CheckToMove(field, nextBlock[0], recommendR, recommendY, recommendX) == 0) {
			gameOver = 1;
			return;
		}
		for (i = 0; i < 4; i++) {
			if (blockRotate != recommendR) {
				ProcessCommand(KEY_UP);
			}
			else break;
		}

		for (j = 0; j < WIDTH; j++) {
			if (blockX < recommendX) {
				ProcessCommand(KEY_RIGHT);
			}
			else if (blockX > recommendX) {
				ProcessCommand(KEY_LEFT);
			}
			else break;
		}
		for (k = 0; k < HEIGHT; k++) {
			if (blockY < recommendY) {
				ProcessCommand(KEY_DOWN);
			}
			else break;
		}
}
void recommendedPlay(){
	int command;
	clear();
	time_t start, finish;
	double duration;
	act.sa_handler = BlockDown;
	sigaction(SIGALRM, &act, &oact);
	InitTetris();
	start = time(NULL);
	do {
		if (timed_out == 0) {
			alarm(1);
			timed_out = 1;
		}

		command = GetCommand();
		autoplay();

		if (ProcessCommand(command) == QUIT) {
			alarm(0);
			DrawBox(HEIGHT / 2 - 1, WIDTH / 2 - 5, 1, 10);
			move(HEIGHT / 2, WIDTH / 2 - 4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	} while (!gameOver);
	finish = time(NULL);
	duration = (double)difftime(finish,start);
	alarm(0);
	getch();
	DrawBox(HEIGHT / 2 - 1, WIDTH / 2 - 5, 1, 10);
	move(HEIGHT / 2, WIDTH / 2 - 4);
	printw("GameOver!!");

	refresh();
	getch();
	clear();
	printw("Score : %d  \n",score);
    printw("Time : %lf seconds\n",duration);
	printw("Score/Time : %lf\n", (double)score / duration);
	printw("Space : %d bytes \n", nodecount * sizeof(RecNode*));
	printw("Score/Space : %lf\n", ((double)score / (nodecount * sizeof(RecNode*))));
	nodecount = 0;
	refresh();
	getch();
}
