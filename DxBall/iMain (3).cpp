#define _CRT_SECURE_NO_WARNINGS
#include "iGraphics.h"
#include<time.h>

#define MAXIMUM_BRICKS 80
#define nBullet 15
#define nBall 10

FILE *fp;

//integers

int bricksPerRow;	//initilize them in main depending on level
int numOfRows;
int gapBtwnTwoBricks;
int gapBtwnTwoRows;
int brickW;//width
int brickL;//length
int firstBrickX;
int firstBrickY;
int iniflag=1;		//1=cover 2=levels 3=score 4=help 5=level 1 6=level 2
int level1flag = 0;
int level2flag = 0;
int level3flag = 0;
int startFlag = 0;
int loopf = 0;
int leaderboard=0;
int leaderboard1=0;
int score[5] ;
int ballR = 6;		//ball radius
int paddleL = 80;
int paddleX,paddleY;
int life=3;
int start=0;
int scr=0;			
int count = 1;
int pause = 0;
int magnet = 0;
int bullet = 0;
int mball = 0;		//multipleballflag
int lp = 0;

//srting
char scrstr[12];	//curent scr
char scorestr[12];	//highscore

//arrays

int bricks[MAXIMUM_BRICKS][2];	//colmn 1: x clmn 2: y 
int brickFlags[MAXIMUM_BRICKS];	// if 1= normal brick ,2=2hit brick,10=unbreakable brick
int pwrUp[9][4] = {};         //x,y,flag,count time of effect
int bullets[nBullet][4] = { };//x,y,bullet1 flag,bullet 2 flag
int balls[nBall][5] = {};//x,y,flag,dx,dy
//0 = bigpaddle 1=littlePaddle 2=big ball 3=xtra life 4= fast ball 
//5 = slow ball 6 = bullet  7 = magnet 8 = many ball


// functions
void levelChecker(void);
void drawLevel(void);
void drawBullet(void);
void highScore(void);
void dxball(void);
void moveBall(int *ballX, int *ballY,int *dx,int *dy,int n);
void initialize(void);
void sort(void);
void showScore(void);



void initializeArrayOfBricksnFlags(int flag)//flag =1 normal brick,2=2hit brick ,3=unreakable brick
{
	int i;
	for ( i=0; i<bricksPerRow*numOfRows; i++)
	{
		bricks[i][0] = firstBrickX + (brickL+gapBtwnTwoBricks)*(i%bricksPerRow);
		bricks[i][1] = firstBrickY - (brickW+gapBtwnTwoRows)*(i/bricksPerRow);
		brickFlags[i] = flag;
	}
	
	for (int i=bricksPerRow*numOfRows; i<MAXIMUM_BRICKS; i++)
	{
		bricks[i][0] = 700;
		bricks[i][1] = 700;

		brickFlags[i] = 0;
	}

}

void specialBricks(int a[80][2],int n)// a = array of flag , n = num of elements in a
{
	int i;
	for (i = 0; i<n; i++)
	{
		brickFlags[(a[i][0])] = a[i][1];//calculate brick from 0
	}
}

void drawBackground(int x)//x 01= game over 11 = level background 21 = win
{
	if (x==01)// lost
	{
		iSetcolor(0.05, 0, 0.2);
		iFilledRectangle(0, 0, 600, 700);
		iSetcolor(1, .5, .8);
		iFilledRectangle(0, 450, 600, 100);
		iSetcolor(1, 1, 1);
		iText(200, 510, "GAME OVER!!!", GLUT_BITMAP_HELVETICA_18);
		iSetcolor(0.5, 5, 0.05);
		iRectangle(200, 250, 100, 50);
		iSetcolor(1, 1,1);
		iText(170, 470, "YOUR SCORE IS", GLUT_BITMAP_HELVETICA_18);
		iText(360, 470, scrstr, GLUT_BITMAP_HELVETICA_18);
		iText(220, 270, "HOME", GLUT_BITMAP_HELVETICA_18);
	
	}
	else if (x==11)//start 
	{
		iSetcolor(0,0.2,0.4);
		iFilledRectangle(0,0,100,700);//side bar
		iFilledRectangle(100,650,500,50);//upper bar

		iSetcolor(0.1,0.7,0.4);
		if(iniflag==5)		   iText(270,660,"LEVEL - 01",GLUT_BITMAP_HELVETICA_18);
		else if (iniflag == 6) iText(270, 660, "LEVEL - 02", GLUT_BITMAP_HELVETICA_18);
		else if (iniflag == 7) iText(270, 660, "LEVEL - 03", GLUT_BITMAP_HELVETICA_18);
		iSetcolor(0.8,0.1,0);
		iText(10,650,"LIFE",GLUT_BITMAP_HELVETICA_18);
		iSetcolor(0.8,0.1,0);
		sprintf_s(scrstr,"%d",scr);
		iText(10,550,"SCORE",GLUT_BITMAP_HELVETICA_18);
		iText(10,520,scrstr,GLUT_BITMAP_HELVETICA_18);

	}
	else if (x==21)// won
	{
		iSetcolor(0.05, 0, 0.2);
		iFilledRectangle(0, 0, 600, 700);
		iSetcolor(1, .5, .8);
		iFilledRectangle(0, 450, 600, 100);
		iSetcolor(1, 1, 1);
		iText(170, 510, "CONGRATULATIONS!!! YOU WON!!!", GLUT_BITMAP_HELVETICA_18);
		iSetcolor(0.5, 5, 0.05);
		iRectangle(200, 250, 100, 50);
		iSetcolor(1, 1, 1);
		iText(170, 470, "YOUR SCORE IS", GLUT_BITMAP_HELVETICA_18);
		iText(360, 470, scrstr, GLUT_BITMAP_HELVETICA_18);
		iText(220, 270, "HOME", GLUT_BITMAP_HELVETICA_18);
	

	}
}

void displayLife()
{
	iSetcolor(1,0,1);
	for(int i=0; i < life; i++)
	{
		if (i>2 && i < 6)
			iFilledCircle(10 + (i-3) * 30, 600, 10, 6);
		else
			iFilledCircle(10+i*30,630,10,6);
	}
}

void drawBricks(int a[][2],int flags[],int bPerRow,int numOR, int bLength, int bWidth)
{
	int i;
	for(i=0;i<bPerRow*numOR;i++) //bricks per row, num of rows,b=bricks
	{
		if (flags[i]==1)
	{
		if(i/bPerRow==0 )
		{
			iSetcolor(1,0,0);
		}
		else if (i/bPerRow==1)
		{
			iSetcolor(0,1,0);
		}
		else if (i/bPerRow==2)
		{
			iSetcolor(0,0,1);
		}
		else if (i/bPerRow==3)
		{
			iSetcolor(1,1,0);
		}
		else if (i/bPerRow==4)
		{
			iSetcolor(1,0,1);
		}
		else if (i/bPerRow==5)
		{
			iSetcolor(0,1,1);
		}
		else if (i / bPerRow == 6)
		{
			iSetcolor(0.8, 0, 0.4);
		}
		else if (i / bPerRow == 7)
		{
			iSetcolor(0.8, 1, 0.4);
		}
		else if (i / bPerRow == 8)
		{
			iSetcolor(1,.4, 0.4);
		}
		iFilledRectangle(a[i][0],a[i][1],bLength,bWidth);
	}
		else if (flags[i]==2)//2 hit brick
		{
			iSetcolor(.5,0,0);
			iFilledRectangle(a[i][0],a[i][1],bLength,bWidth);
		}
		else if(flags[i]==10)//unbreakable
		{
			iSetcolor(.3, .3, .3);
			iFilledRectangle(a[i][0],a[i][1],bLength,bWidth);
		}
		
	}
}


int straightLine(int x1,int y1,int x2,int y2,int testX1,int testY1)
{	//it tests if a point is in same or opposite side with(0,0) of a straight line
	int a,b,c;
	a=y1-y2;	b=x2-x1;	c=-x1*(y1-y2)+y1*(x1-x2);
	
	 return (a*testX1+b*testY1+c)*(a*0+b*0+c) ;//+ve same side -ve opposite side
}

void activatePwrUps(int x)
{
	if(x==0)		//large paddle
	{	paddleL = 100;	
	}
	else if(x==1)	//small paddle
	{	paddleL=60;
	}
	else if(x==2)	//big ball
	{	ballR = 9;
	}
	else if(x==8)	//xtra life
	{ 
		if (life<6)
		life = life +1;
	}
	else if(x==4)//fast ball
	{}
	else if(x==5)//slow ball
	{}
	else if(x==6)//bullet
	{
		bullet = 1;
	}
	else if(x==7)//magnet
	{}
	else if(x==3)//multiple ball
	{	
		mball = 1;
		int ballcount= 0;
		for (int i = 0; i < nBall; i++)
		{
			if (ballcount<3 && balls[i][2] == 0)
			{
				balls[i][2] = 1;
				balls[i][0] = paddleX;
				balls[i][1] = (80 + ballR);
				ballcount++;
				if (ballcount == 1){ balls[i][3] = -8;	balls[i][4] = 8; }
				else if (ballcount == 2)	{ balls[i][3] = 8;  balls[i][4] = 6; }
				else if (ballcount == 3)		{ balls[i][3] = -6; balls[i][4] = 8; }
			}
		}
		if (count < 7) count += 3;

	}
}

void deactivatePwrUps(int x)
{
	if (x==0)// large paddle
	{
		paddleL = 80;	
	}
	else if(x==1)//small paddle
	{
		paddleL =80;
	}
	else if(x==2)//big ball
	{
		ballR = 6;
	}
	else if(x==8)//xtra life
	{}
	else if(x==4)//fast ball
	{}
	else if(x==5)//slow ball
	{}
	else if(x==6)//fire
	{
		bullet = 0;
		for (int i = 0; i < nBullet; i++)
		{
			bullets[i][0] = 0;	//x
			bullets[i][1] = 0;	//y
			bullets[i][2] = 0;	//1st flag
			bullets[i][3] = 0;	//2nd flag
		}
	}
	else if(x==7)//magnet
	{
		magnet = 0;
	}
	else if(x==3)//multiple ball
	{
		mball = 0;
		/*
		for (int i = 0; i < nBall; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				balls[i][j] = 0;
			}
		}

		balls[0][0] = paddleX;//ballx
		balls[0][1] = (80 + ballR);//bally
		balls[0][2] = 0;//flag
		balls[0][3] = 8;//dx
		balls[0][4] = 8;//dy*/
	}
	pwrUp[x][3] = 0;//countdown
}

void setChangesToDefault()//disable all xtra effects of pwrups at once--- useful when ball drops
{
	for(int i=0;i<9;i++)
	{
		deactivatePwrUps(i);
		pwrUp[i][2]=0;	//pwrup drawing flag set off
		pwrUp[i][3]=0;	// time counting set to zero
	}
}

void dxball(void)
{
	int m;
	if (mball == 0)	m = 1;
	else m = nBall;

	if (start == 0)
	{
		balls[0][2] = 1;
		balls[0][0] = paddleX;
		balls[0][1] = (80 + 1+ ballR);
	}

	for (int i = 0; i < m; i++)
	{
		if (balls[i][2] == 1)
		{
			startFlag = 1;	//atleast 1 ball is alive
			break;
		}	
		else startFlag = 0;
	}
	if (startFlag == 0 && !loopf)
	{ 
			start = 0;
			life--; 
			loopf = 1;
		setChangesToDefault();
	}

	if ( start == 1)
	{
		if (pause == 1 || magnet == 1)
			;
		else
		{
			
			for (int i = 0; i < m; i++)
			{
				
				if (balls[i][2] == 1)
				{
					moveBall(&balls[i][0], &balls[i][1], &balls[i][3], &balls[i][4],i);
				}
			}
		}
	}
}

void moveBall(int *ballX, int *ballY,int *dx,int *dy,int n)
{

	 if (start == 1)//if game is being played
	{
		if ((*ballY <= 80 + ballR) && 
			(	(paddleX - *ballX > (paddleL / 2)) || (*ballX - paddleX > (paddleL / 2))	))
		{	
			balls[n][2] = 0;
			count--;
			*ballX += *dx;
			*ballY += *dy;	
		}

		else //if ball is in air
		{			
			float n = 1;
			if (pwrUp[4][3] > 0)		{ n = 1.5; }
			else if (pwrUp[5][3] > 0)	{ n = 0.5; }
			else						{ n = 1; }
			
			if (*ballX + *dx *n < 106)
				*ballX = 106;
			else if (*ballX + *dx*n > 594)		//while hitting side walls and upper and lower walls
				*ballX = 594;
			else
				*ballX = *ballX + *dx *n;		//so that the ball does not cross the lines

			if (*ballY + *dy *n < 86)
				*ballY = 86;
			else if (*ballY + *dy *n> 644)
				*ballY = 644;
			else
				*ballY = *ballY + *dy *n;

			
				if (*ballX >= (600 - ballR)) //ball bounces back at getting hit in left side wall
				{
					//*dx = -*dx;
					int r = rand() % 2;
					if (r == 0){ *dx = -8 * n; *dy = *dy > 0 ? 8 * n : -8 * n; }
					else if (r == 1){ *dx = -6 * n; *dy = *dy > 0 ? 8 * n : -8 * n; }
					else if (r == 2){ *dx = -8 * n; *dy = dy > 0 ? 6 * n : -6 * n; }
					
				}
				else if (*ballX <= (100 + ballR))//ball bounces back at getting hit in left side wall
				{
					//*dx = - *dx;
					int r = rand() % 2;
					if (r == 0){ *dx = 8 * n; *dy = *dy > 0 ? 8 * n : -8 * n; }
					else if (r == 1){ *dx = 6 * n; *dy = *dy > 0 ? 8 * n : -8 * n; }
					else if (r == 2){ *dx = 8 * n; *dy = *dy > 0 ? 6 * n : -6 * n; }
				}
				if (*ballY >= (650 - ballR))//bounces back at getting hit in upper wall
				{
					//*dy = -*dy;
					int r = rand() % 2;
					if (r == 0){ *dy = -8 * n; *dx = *dx > 0 ? 8 * n : -8 * n; }
					else if (r == 1){ *dy = -6 * n; *dx = *dx > 0 ? 8 * n : -8 * n; }
					else if (r == 2){ *dy = -8 * n; *dx = *dx > 0 ? 6 * n : -6 * n; }
				}
				else if (*ballY <= (80 + ballR))//angle varies with hitting different part of ballcatcher
				{
					//*dy = -*dy;
					if (*ballX <= (paddleX + paddleL / 2) + ballR)
					{
						if (*ballX > (paddleX + paddleL / 2) - (3 * paddleL / 20))
						{
							*dx = 8 * n; *dy = 6 * n;
						}
						else if (*ballX > (paddleX + paddleL / 2) - 2 * (3 * paddleL / 20))
						{
							*dx = 8 * n; *dy = 8 * n;
						}
						else if (*ballX > (paddleX + paddleL / 2) - 3 * (3 * paddleL / 20))
						{
							*dx = 6 * n; *dy = 8 * n;
						}
						else if (*ballX > (paddleX + paddleL / 2) - 3 * (3 * paddleL / 20) - (paddleL / 10))
						{
							*dx = 0; *dy = 10 * n;
						}
						else if (*ballX > (paddleX + paddleL / 2) - (paddleL / 10) - 4 * (3 * paddleL / 20))
						{
							*dx = -6 * n; *dy = 8 * n;
						}
						else if (*ballX > (paddleX + paddleL / 2) - (paddleL / 10) - 5 * (3 * paddleL / 20))
						{
							*dx = -8 * n; *dy = 8 * n;
						}
						else if (*ballX >= (paddleX + paddleL / 2) - (paddleL / 10) - 6 * (3 * paddleL / 20))
						{
							*dx = -8 * n; *dy = 6 * n;
						}
		
					}
				}
				
			for (int i = 0; i < bricksPerRow*numOfRows; i++)//checks brick hitting and turns off broken brick
			{
				lp = 1;
				for (int k = 0; k < nBullet; k++)//bullet 
				{
					if (brickFlags[i] >= 1)
					{
						if (bullets[k][2] == 1 && bullets[k][0] + 2 >= bricks[i][0] && bullets[k][0] <= bricks[i][0] + brickL
							&& bullets[k][1] + 7 >= bricks[i][1] && bullets[k][1] <= bricks[i][1] + brickW)
						{
							bullets[k][2] = 0;
							if (brickFlags[i] != 10)//10 == unbreakable brick
								brickFlags[i] -= 1;
							scr += 50;
						}
						if (bullets[k][3] == 1 && bullets[k][0] + paddleL + (3-1) >= bricks[i][0] && bullets[k][0] + paddleL <= bricks[i][0] + brickL
							&& bullets[k][1] + 7 >= bricks[i][1] && bullets[k][1] <= bricks[i][1] + brickW)
						{
							bullets[k][3] = 0;
							if (brickFlags[i] != 10)//10 == unbreakable brick
								brickFlags[i] -= 1;
							scr += 50;
						}
					}
				}

				int f=0;	//brick erasing due to ball
				if ((bricks[i][0] - (ballR - 1)) <= *ballX   &&   *ballX <= (bricks[i][0] + brickL + (ballR - 1)))
				{
					if ((bricks[i][1] - (ballR - 1)) <= *ballY   &&   *ballY <= (bricks[i][1] + brickW + (ballR - 1)))
						f = 1;
				}

				if (f==1 && brickFlags[i] >= 1)
				{
					if (brickFlags[i] != 10)//10 == unbreakable brick
						brickFlags[i] -= 1;

					//changes balls direction after hit
					int a1, a2, a3, a4;	a1 = a2 = a3 = a4 = 0;
					if (straightLine(bricks[i][0], bricks[i][1], bricks[i][0], 700, *ballX - *dx, *ballY - *dy) > 0)
						{	a1 = 1;	}//left
					if (straightLine(bricks[i][0], bricks[i][1], 600, bricks[i][1], *ballX - *dx, *ballY - *dy) > 0)
						{	a2 = 1;	}//down
					if (straightLine(bricks[i][0], bricks[i][1] + brickW, 600, bricks[i][1] + brickW, *ballX - *dx, *ballY - *dy) < 0)
						{	a3 = 1;	}//up
					if (straightLine(bricks[i][0] + brickL, bricks[i][1], bricks[i][0] + brickL, 700, *ballX - *dx, *ballY - *dy) < 0)
						{	a4 = 1;	}//right

					int random;
					srand(time(NULL));
					random = rand() % 1;
					if ((a1&&a2) || (a2&&a4) || (a4&&a3) || (a3&&a1))//if hit bricks from corners
					{												 // then ball moves randomly
						if (random == 1) *dx = -*dx;
						else			{ *dy = -*dy;		*dx = -*dx; }
					}
					else if (a1 || a4)				
						*dx = -*dx;								
					else if (a3 || a2)
						*dy = -*dy;

					//choosing powerups randomly
					int c = 0;
					int rr;
					srand(time(NULL));
					rr = (rand() % 1);
					
					if (brickFlags[i] != 10 && !rr)// if 0 is chosen release a power up
					{
					int r;
						
						for (int j = 0; j < 3; j++)
						{
							if (c == 0){ r = 6, c++; }
							else if (c == 1){ if(r!=7)r = 3, c++; }
							else if (c == 2){ if (r != 3)r = 7, c++; }
							else if (c == 3){ r = 4, c++; }
							else if (c == 4){ c=0; }
							//srand(time(NULL));
							//r =  rand() % 8;
							
							if (pwrUp[r][2] == 0 && pwrUp[r][3] == 0 && pwrUp[3][3] == 0
								&& pwrUp[7][3] == 0)
							{
								pwrUp[r][2] = 1;//pwr up flag on 
								pwrUp[r][0] = bricks[i][0] + 10;
								pwrUp[r][1] = bricks[i][1];
								break;
							}
						}
					}
					scr = (scr + 50 + (life + 1) * 4);
				}
			}

			//checking if any pwr up is on--- if yes then moving it 
			//and check if player was able to catch it 
			// if yes then activate that pwrup
			//if (lp == 1)
			{
				for (int i = 0; i < 9; i++)
				{
					if (pwrUp[i][2] == 1 && pwrUp[i][1] > (80))
					{
						pwrUp[i][1] -= 5; //printf(" yes");
					}

					else if (pwrUp[i][2] == 1 && pwrUp[i][1] <= (80))
					{
						if ((pwrUp[i][0] + 30) >= (paddleX - paddleL / 2) && (paddleX + paddleL / 2) >= pwrUp[i][0])
						{
							scr += 100;
							activatePwrUps(i);
							 pwrUp[i][3] = 1;
						}
						pwrUp[i][2] = 0;
					}
					if (i != 3 && pwrUp[i][3] >= 1 && pwrUp[i][3] <= 600)//deactivating pwr up after a few seconds
						pwrUp[i][3]++;
					else if (i != 3 && pwrUp[i][3] > 600)
					{
						deactivatePwrUps(i);
						pwrUp[i][3] = 0;
					}
				}
			}
			if (pwrUp[7][3]>1)//magnet
			{
				if (*ballY <= 86 && balls[0][0] >= (paddleX - paddleL / 2)
					&& balls[0][0] <= (paddleX + paddleL / 2))
				{
					magnet = 1;//means ball is sticked to the paddle
				}
			}
			if (bullet>0)//bullet moving
			{
				for (int i = 0; i < nBullet; i++)
				{
					if (bullets[i][2] == 1 || bullets[i][3]==1)
						bullets[i][1] += 5;
					if (bullets[i][1]>=650)
					{
						bullets[i][2] = 0; bullets[i][3] = 0;
					}
				}
			}
		}
	}
}

//bigPaddle,smallPaddle,bigBall,fire,lifeXtra,fastBall,slowBall,manyBall,magnet
void pDrawBigBall(int x,int y)
{
	iSetcolor(0,0,1);
	iFilledRectangle(x,y,30,30);
	iSetcolor(1,1,1);
	iFilledCircle(x+15,y+15,10,50);
}
void pDrawLife(int x, int y)//
{	
	iSetcolor(0,0,1);
	iFilledRectangle(x,y,30,30);
	iSetcolor(1,0,1);
	iFilledCircle(x+15,y+15,10,6);
}
void pDrawFastBall(int x, int y)
{
	iSetcolor(1,0.01,0);
	iFilledRectangle(x,y,30,30);
	iSetcolor(1,1,0.4);
	iText(x,y+11,">>>",GLUT_BITMAP_HELVETICA_18);
}
void pDrawSlowBall(int x, int y)
{
	iSetcolor(.2,.2,1);
	iFilledRectangle(x,y,30,30);
	iSetcolor(1,1,0.4);
	iText(x,y+11,"<<<",GLUT_BITMAP_HELVETICA_18);
}
void pDrawLargeBallCatcher(int x, int y)//powerups[0]
{
	iSetcolor(.2,.2,1);
	iFilledRectangle(x,y,30,30);
	iSetcolor(1,1,1);
	iFilledRectangle(x+3,y+13,25,5);
}
void pDrawSmallBallCatcher(int x, int y)//powerups[1]
{
	iSetcolor(1,0.01,0);
	iFilledRectangle(x,y,30,30);
	iSetcolor(1,1,1);
	iFilledRectangle(x+10,y+13,10,5);
}
void pDrawMultipleBall(int x,int y)
{
	iSetcolor(.2, .2, 1);
	iFilledRectangle(x,y,30,30);
	iSetcolor(1,1,1);
	for(int i=0;i<3;i++)
	{	
		iFilledCircle(x+5+i*10,y+15,4,30);
	}
}
void pDrawBullet(int x, int y)
{
	iSetcolor(.2,.2,1);
	iFilledRectangle(x,y,30,30);
	iSetcolor(1,1,1);
	iFilledRectangle(x+6,y+11,20,5);
	
	iSetcolor(1,1,1);
	iFilledRectangle(x+9,y+16,2,2);
	
	iSetcolor(1,0,0);
	iFilledRectangle(x+9,y+20,2,6);
	iSetcolor(1,0,0);
	iFilledRectangle(x+22,y+20,2,6);
}
void pDrawMagnet(int x, int y)
{
	iSetcolor(.2,.2,1);
	iFilledRectangle(x,y,30,30);
	iSetcolor(1,1,1);
	iFilledRectangle(x+6,y+13,20,5);
	
	iSetcolor(1,1,1);
	iFilledRectangle(x+9,y+18,2,4);
	iSetcolor(1,1,1);
	iFilledRectangle(x+22,y+18,2,4);
	iSetcolor(1,0,0);
	iLine(x+6,y+13+5+4,x+6+20,y+22);
}

//0 = bigpaddle 1=littlePaddle 2=big ball 3=xtra life 4= fast ball 
//5 = slow ball 6 = bullet  7 = magnet 8 = many ball
void drawPowerups()
{	
	if(pwrUp[0][2])//bigPaddle
	{
		pDrawLargeBallCatcher(pwrUp[0][0],pwrUp[0][1]);
	}
	if(pwrUp[1][2])//littlePaddle
	{
		pDrawSmallBallCatcher(pwrUp[1][0],pwrUp[1][1]);
	}
	if(pwrUp[2][2])//big ball
	{
		pDrawBigBall(pwrUp[2][0],pwrUp[2][1]);
	}
	if(pwrUp[8][2])//xtra life
	{
		pDrawLife(pwrUp[8][0],pwrUp[8][1]);
	}
	if(pwrUp[4][2])//fast ball
	{
		pDrawFastBall(pwrUp[4][0],pwrUp[4][1]);
	}
	if(pwrUp[5][2])//slow ball
	{
		pDrawSlowBall(pwrUp[5][0],pwrUp[5][1]);
	}
	if(pwrUp[6][2])//bullet
	{
		pDrawBullet(pwrUp[6][0],pwrUp[6][1]);
	}
	if(pwrUp[7][2])//magnet 
	{
		pDrawMagnet(pwrUp[7][0],pwrUp[7][1]);
	}
	if(pwrUp[3][2])//multiple ball
	{
		pDrawMultipleBall(pwrUp[3][0],pwrUp[3][1]);
	}


}

int gameFinished()
{
	if (iniflag == 1)
	{
		life = 3;
		return 0;
	}
	else
	{
		for (int i = 0; i < bricksPerRow*numOfRows; i++)
		{
			if (brickFlags[i] == 1 || brickFlags[i] == 2)
				return 0;
		}
		return 1;
	}
}


void sort(void)
{
	int i, j, temp;
	for (j = 0; j < 4; ++j)
	{
		for (i = 0; i< 5; ++i)
		{
			if (score[i] < score[i + 1])
			{
				temp = score[i];
				score[i] = score[i + 1];
				score[i + 1] = temp;
			}
		}
	}
}

void highScore(void)
{
	int i = 0;
	fp = fopen("HIGHSCORE.txt", "r");
	while (!feof(fp)) { fscanf(fp, "%d", &score[i]); i++; }
	fclose(fp);


	if (scr>score[4]) score[4] = scr;
	sort();


	fp = fopen("HIGHSCORE.txt", "w");
	for (i = 0; i<5; i++) fprintf(fp, "%d ", score[i]);
	fclose(fp);
}


void showScore(void)
{
	iSetcolor(1, 0, 0);
	iText(220, 550, "*** LEADERBOARD ***", GLUT_BITMAP_HELVETICA_18);
	int i;
	for (i = 0; i < 5; i++)
	{
		iSetcolor(1, 1, 1);
		if (i == 0) iSetcolor(1, .1, 1);

		sprintf_s(scorestr, " %d", i + 1);
		iText(260, 500 - 40 * i, scorestr, GLUT_BITMAP_HELVETICA_18);
		sprintf_s(scorestr, ".  ", i + 1);
		iText(275, 500 - 40 * i, scorestr, GLUT_BITMAP_HELVETICA_18);
		sprintf_s(scorestr, " %d", score[i]);
		iText(300, 500 - 40 * i, scorestr, GLUT_BITMAP_HELVETICA_18);
	}
	iSetcolor(0.5, 5, 0.05);
	iRectangle(250, 250, 100, 50);
	iSetcolor(1, 1, 1);
	iText(270, 270, "HOME", GLUT_BITMAP_HELVETICA_18);

}

void iKeyboard(unsigned char key)
{
	if (key == 'q')
	{
		exit(0);
	}
	if (iniflag == 5|| iniflag ==6 || iniflag == 7 )
	{ 
		if (key == 'a') 
		{
			paddleX -= 20;
			if (start == 0) balls[0][0] = paddleX;
			if (pwrUp[7][3] > 1 && balls[0][1] <= 86 && balls[0][0] >= (paddleX - paddleL / 2)
				&& balls[0][0] <= (paddleX + paddleL / 2))//magnet//??
				balls[0][0] = paddleX;
		}
		else if (key == 'd')
		{
			paddleX += 20;
			if (start == 0) balls[0][0] = paddleX;
			if (pwrUp[7][3] > 1 && balls[0][1] <= 86 && balls[0][0] >= (paddleX - paddleL / 2)
				&& balls[0][0] <= (paddleX + paddleL / 2))//magnet//??
				balls[0][0] = paddleX;//??balls[0][0] += 20;
		}
	else if (key == ' ')
	{
		if (start == 0)
		{			
			start = 1; 
			balls[0][0] = paddleX;
			balls[0][1] = (80+1 + ballR);
			balls[0][2] = 1;

			loopf = 0;
		}
		if (start == 1 && bullet > 0 && bullet<nBullet)//bullet 
		{
			bullets[bullet][0] = paddleX - (paddleL / 2);
			bullets[bullet][1] = paddleY + 10;
			bullets[bullet][2] = 1;
			bullets[bullet][3] = 1;
			bullet++;
		}
		magnet = 0;
	}
	else if (key = 'p')
	{
		if (pause == 0)
			pause = 1;
		else if (pause == 1)
			pause = 0;
	}
	
	
	}
	
	
}

void iMouse(int button, int state, int mx, int my)
{
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (iniflag == 1 )// 1 means cover
		{
			if (mx >= 190 && mx <= 408)
			{
				if (my <= 500 && my >= 450)
				{
					iniflag = 5;// 5 means moved to level 1
					level1flag = 1;//initialize array of bricks for level 1
				}
				else if (my >= 380 && my <= 420)	iniflag = 2;//levels				
				else if (my >= 200 && my <= 250)	iniflag = 4;//help				
				else if (my <= 330 && my >= 290)	iniflag = 3;//highscore
				else if (my >= 120 && my <= 180)	iniflag = 10;//credits
				else if (my >= 60 && my <= 100)		iniflag = 11;//quit
			}
		}
		else if (iniflag == 2)//levels
		{
			if (mx >= 180 && mx <= 440  )
			{ 
				if (my <= 510 && my >= 450)
				{
					iniflag = 5;//flag 5 means moved to level 1
					level1flag = 1;//initialize array of bricks for level 1
				}
				else if (my <= 410 && my >= 350)
				{
					iniflag = 6;//flag 6 means moved to level 2
					level2flag = 1;//initialize array of bricks for level 2
				}
					else if (my <= 320 && my >= 250)
				{
					iniflag = 7;//flag 7 means moved to level 3
					level3flag = 1;//initialize array of bricks for level 3
				}
					else if (my <= 230 && my >= 160)//back
				{
					iniflag = 1;//flag 1 means moved to cover
					
				}
			}
			
		}
		else if (iniflag == 3)//highscore
		{
			if (mx >= 250 && mx <= 350 && my >= 250 && my <= 300) iniflag = 1;//back
		}
		else if (iniflag == 4)//help
		{
			if (mx >= 200 && mx <= 400 && my <= 60 && my >= 20) iniflag = 1;//back
		}
		else if (iniflag == 5 || iniflag == 6 || iniflag == 7 )
		{
			if (start == 0)
			{
				start = 1;
				balls[0][0] = mx;
				balls[0][1] = (80+1 + ballR);
				balls[0][2] = 1;
				loopf = 0;
			}
			if (start == 1 && bullet > 0 && bullet<nBullet)//bullet 
			{	
				bullets[bullet][0] = paddleX - (paddleL / 2);
				bullets[bullet][1] = paddleY + 10;
				bullets[bullet][2] = 1;
				bullets[bullet][3] = 1;
				bullet++;
			}
			if (magnet == 1)
			{
				balls[0][0] = mx;
				balls[0][1] = (80 + 1 + ballR);
				balls[0][2] = 1;
				loopf = 0;
				magnet = 0;
			}

			paddleX = mx;
		}
		else if (iniflag == 8 || iniflag == 9) // 8 = game over 9 = game won
		{
			if(mx>=200 && mx<=300 && my>=250 && my<=300) iniflag=1;
		}
		else if (iniflag = 10)					//credits
		{
			if (mx >= 250 && mx <= 360 && my >= 210 && my <= 270) iniflag = 1;
		}
	}
	if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		//place your codes here	
	} 

}

void iMouseMove(int mx, int my)
{
	if ((iniflag == 5 || iniflag == 6 || iniflag == 7) && start != 0 && magnet != 1)
		paddleX=mx;
}

void iDraw()
{
	iClear();
	levelChecker();
}

void drawLevel(void)
{
	iSetcolor(1, 1, 1);
	iLine(100, 0, 100, 700);
	iLine(100, 650, 600, 650);

	if (gameFinished() && life >= 0)//won
	{
		//drawBackground(21);
		if(leaderboard1==0) {highScore(); leaderboard1=1;}	
		iniflag=9;
	}
	else if (!gameFinished() && life<0)//lost
	{
		//drawBackground(01);
		if(leaderboard1==0) {highScore(); leaderboard1=1;}
		iniflag =8;
	}
	else	//playing
	{
		drawBackground(11);
		displayLife();

		{
			iSetcolor(1, 1, 1);
			for (int i=0; i < nBall; i++)//ball
			{ 
				if (balls[i][2]==1)
					iFilledCircle(balls[i][0], balls[i][1], ballR, 50);//ball
			}

			if (paddleX < (100 + paddleL / 2))
				paddleX = 100 + paddleL / 2;
			else if (paddleX > (600 - paddleL / 2))
				paddleX = 560;

			if (pwrUp[7][3]>1 || pwrUp[6][3]>1)//magnet bullet paddle
			{
				iFilledRectangle(paddleX - (paddleL / 2), paddleY+10, 3, 7);
				iFilledRectangle(paddleX + (paddleL / 2)-3, paddleY + 10, 3, 7);
				if (pwrUp[7][3] > 1){
					iSetcolor(1, 0, 0);
					iFilledRectangle(paddleX - (paddleL / 2), paddleY + 10 + 7, paddleL, 2);
				}
			}
			iFilledRectangle(paddleX - (paddleL / 2), paddleY, paddleL, 10);//paddle
		}
		drawBricks(bricks, brickFlags, bricksPerRow, numOfRows, brickL, brickW);
		drawPowerups();
		iSetcolor(1, 1, 1);
		if (bullet > 0 )
		{
			for (int i = 0; i < nBullet; i++)
			{
				if (bullets[i][2]==1)
				{ 
					iFilledRectangle(bullets[i][0],bullets[i][1], 3, 7);
				}
				if (bullets[i][3] == 1)
				{
					iFilledRectangle(bullets[i][0] + paddleL - 3, bullets[i][1], 3, 7);//..
				}
			}
	}
}

}

void levelChecker(void)
{
	if (iniflag == 1)//new game
	{
		initialize();
		iShowBMP(0, 0, "1_cover2.bmp");
	}
	else if (iniflag == 2)//levels
	{ 
		iShowBMP(0, 0, "2_levels.bmp");
	}
	else if (iniflag == 3)//scoreboard
	{		
		if(leaderboard==0 && leaderboard1==0) {highScore(); leaderboard=1;}
		showScore();
	}
	else if (iniflag == 4)//help
	{
		iShowBMP(0, 0, "6_help.bmp");
	}
	else if (iniflag == 5)//level 1
	{
		if (level1flag == 1)
		{
			initializeArrayOfBricksnFlags(1);
			level1flag = 0;
		}
		drawLevel();
	}
	else if (iniflag == 6)//level 2
	{
		if (level2flag == 1)
		{
			initializeArrayOfBricksnFlags(1);

			int s[30][2] = { { 0, 2 }, { 1, 2 }, { 2, 2 }, { 3, 2 }, { 4, 2 }, { 5, 2 }, { 6, 2 },
			{ 7, 2 }, { 8, 2 },{ 15, 2 }, { 16, 2 }, { 23, 2 }, { 24, 2 }, { 31, 2 }, { 32, 2 }, 
			{ 39, 2 }, { 40, 2 }, { 47, 2 }, { 48, 2 },{ 55, 2 }, {56,2},{63,2},{64,10},{71,2},
			{65,2},{66,10},{67,2},{68,10},{69,2},{70,10} };

			specialBricks(s, 30);
			level2flag = 0;
		}
		drawLevel();
	}

	else if (iniflag == 7)//level 3
	{
		if (level3flag == 1)
		{
			initializeArrayOfBricksnFlags(1);

			int s[62][2] = { { 0, 0}, { 1, 0 }, { 2, 0 }, { 3, 0 }, { 4, 0 }, { 5, 2 }, { 6, 0 },{ 7, 0 },
			{ 8, 0 },{9, 0 },{ 10, 0 },{ 11, 0 },{ 12, 2 },{ 14, 2},{ 15, 0 },
			{ 16, 0 }, { 17, 0 },{ 18, 0 },{ 19, 2 },{ 23, 2 },
			{ 24, 0 },{ 25, 0 }, { 26, 0 }, { 27, 0 }, { 28, 2 }, { 30, 2 }, { 31, 0 }, 
			{ 32, 0 }, { 33, 0 }, { 34, 2 },{ 35, 0 }, { 36, 0 }, { 37, 2 }, { 38, 0 }, { 39, 0 }, 
			{ 40, 0 },{ 41, 2 },{ 43, 2 },{ 44, 0 }, { 45, 0 }, { 46, 0 },  { 47, 0 },
			{ 48, 2 },{ 52, 2 },{ 53, 0 },{ 54, 0 },{ 55, 0 }, 
			{56,0},{57,2},{59,2},{60,0},{61,0},{62,0},{63,0},
			{66,2},{64,0},{65,0},{67,0},{68,0},{69,0},{70,0},{71,0}};

			specialBricks(s, 62);
			level3flag = 0;
		}
		drawLevel();
	}
	else if(iniflag==8)//game over
	{
		drawBackground(01);
	}
	else if(iniflag==9)//playing
	{
		drawBackground(21);
	}
	else if (iniflag == 10)//won
	{
		iShowBMP(0, 0, "7_credits.bmp");
	}
	else if (iniflag == 11)//quit
	{
		exit(0);
	}

}
void initialize(void)
{
	balls[0][0] = 350;//ballx
	balls[0][1] = (80 + ballR);//bally
	balls[0][2] = 1;//flag
	balls[0][3] = 8;//dx
	balls[0][4] = 8;//dy
	paddleX = 350;
	paddleY = 70;
	life = 3;
	scr = 0;
	leaderboard1 = 0;
}

int main()
{
	bricksPerRow=8;	
	numOfRows=9;
	gapBtwnTwoBricks=2;
	gapBtwnTwoRows=2;
	brickW=20;
	brickL=60;
	firstBrickX=103;
	firstBrickY=570;
	initialize();
	
	iSettimer(30, levelChecker);
	iSettimer(30, dxball);
	

	iInitialize(600, 700, "...DX BALL...");
	return 0;
}