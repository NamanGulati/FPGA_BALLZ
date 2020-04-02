#include <stdlib.h>
#include <math.h>



#define RECTANGLE_WIDTH 20
#define RECTANGLE_HEIGHT 20

struct Rectangle{
    int x ,y,hitScore;
	short int color;
};
	
void clear_screen();
void plot_pixel(int x, int y, short int line_color);
void swap (int * a, int * b);
void draw_line(int x0, int x1, int y0, int y1, short int line_color);
void draw_rectangle(struct Rectangle rect);
//void drawCircle(int xc, int yc, int x, int y, short int color) ;
//void circleBres(int xc, int yc, int r,short int color);
void horizontalLine(int x, int y, int w, short int col);
void fillCircle(int x,int y, int r, short int col);
void wait_for_vsync();
void drawBlocks();
void write_char(int x, int y, char c);



volatile int pixel_buffer_start; // global variable
int size_x;
int size_y;
int lastRow=0;
int lastExp=0;



short int blockColors []={0xF81F /*pink*/, 0x001F /*blue*/,0x07E0 /*green*/,0xF800 /*red*/};
struct Rectangle blocks[16][10];

int main(){
	volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    volatile int * pixel_res_ptr = (int *)0xFF203028;
    /* Read location of the pixel buffer from the pixel buffer controller */
    int resolution = *pixel_res_ptr;
    size_x = resolution & 0xFFFF;
    size_y = resolution >> 16;
	*(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the
    // back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(); // pixel_buffer_start points to the pixel buffer
    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
	//fillCircle(160,231,8,0xF800);
	drawBlocks();
	wait_for_vsync();
	clear_screen();
	drawBlocks();
	wait_for_vsync();
	clear_screen();
	drawBlocks();
	wait_for_vsync();
	//you lost
}

void drawBlocks(){
	int blockPos = (rand() % 0xFFFF) +1;
	int i=0;
	while(blockPos!=0){
		if(blockPos & 1){
			struct Rectangle rect;
			rect.x = RECTANGLE_WIDTH*i;
			rect.y = 0;
			rect.hitScore = (rand() % (int)pow(2,lastExp))+1;
			rect.color = blockColors [rand() % 3+1];
			draw_rectangle(rect);
			blocks[lastRow][i]=rect;
			int score = rect.hitScore;
			int x_pos = rect.x;
			while(score!=0){
                write_char(x_pos/4,(rect.y/4)+2,(char)(48+score%10));
                x_pos+=4;
                score/=10;
			}

		}
		i++;
		blockPos = blockPos >> 1;
	}
	
	for(int i=0;i<lastRow;i++){
		for(int j=0;j<10;j++){
		    if(blocks[i][j].hitScore==0)
                continue;
			blocks[i][j].y+=RECTANGLE_HEIGHT;
			draw_rectangle(blocks[i][j]);
            int score = blocks[i][j].hitScore;
            int x_pos = blocks[i][j].x;
            while(score!=0){
                write_char(x_pos/4,(blocks[i][j].y/4)+2,(char)(48+score%10));
                x_pos+=4;
                score/=10;
            }
		}
	}
	
	lastExp++;
	lastRow++;
}


void draw_rectangle(struct Rectangle rect){
    for(int i=rect.x;i<rect.x+RECTANGLE_WIDTH;i++){
        //draw_line(i,rect.y,i,rect.y+RECTANGLE_HEIGHT,color);
        for(int j=rect.y;j<rect.y+RECTANGLE_HEIGHT;j++){
            plot_pixel(i,j,rect.color);
            int numDigits = getNumDigits(rect.hitScore);
            int startX = rect.x;
            switch(numDigits){
                case 1:
                    startX+=7;
                    break;
                case 2:
                    startX+=5;
                    break;
                case 3:
                    startX+=2;
                    break;
            }
            for(int i=0;i<numDigits;i++){

            }
        }

    }
}

int getNumDigits(int num){
   int digits =0;
   while(num!=0){
       num/=10;
       digits++;
   }
   return digits;
}
void write_char(int x, int y, char c) {
    // VGA character buffer
    volatile char * character_buffer = (char *) (0xC9000000 + (y<<7) + x);
    *character_buffer = c;
}


//void draw1(int x, int y){
//    draw_line(x+1,y,x+1,y+20,0xFFFF);
//    draw_line(x+2,y,x+2,y+20,0xFFFF);
//    draw_line(x+3,y,x+3,y+20,0xFFFF);
//    draw_line(x+4,y,x+4,y+20,0xFFFF);
//}
//void draw2(int x, int y){
//    draw_line(x,y,x+5,y,0xFFFF);
//    draw_line(x+5,y,x+5,y+10,0xFFFF);
//    draw_line(x,y+10,x+5,y+10,0xFFFF);
//    draw_line(x,y+10,x,y+20,0xFFFF);
//    draw_line(x,y+20,x+5,y+20,0xFFFF);
//}
//void draw3(int x,int y){
//    draw_line(x,y,x+5,y,0xFFFF);
//    draw_line(x+5,y,x+5,y+20,0xFFFF);
//    draw_line(x,y+10,x+5,y+10,0xFFFF);
//    draw_line(x,y+20,x+5,y+20,0xFFFF);
//}
//void draw4(int x, int y){
//    draw_line(x,y,x,y+10,0xFFFF);
//    draw_line(x+5,y,x+5,y+20,0xFFFF);
//    draw_line(x,y+10,x+5,y+10,0xFFFF);
//}
void swap(int *a, int *b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}
void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}
void clear_screen() 
{
	for(int i =0; i<320;i++)
	{
		for(int j=0;j<240;j++) {
            plot_pixel(i, j, 0);
        }
	}
	for(int i=0;i<79;i++){
	    for(int j=0;j<59;j++)
	        write_char(i,j,(char)0);
	}

}
void draw_line(int x1, int y1, int x2, int y2, short int color)
{
	if(abs(y2-y1)>abs(x2-x1))
	{
		swap(&x1,&y1);
		swap(&x2,&y2);
	}
	if(x1>x2)
	{
		swap(&x1,&x2);
		swap(&y1,&y2);
	}
	int deltax = x2-x1;
	int deltay = abs(y2-y1);
	int error = -(deltax/2);
	int y=y1;
	int y_step;
	if(y1<y2)
		y_step = 1;
	else
		y_step = -1;
	for(int x=x1;x<=x2;x++)
	{
		if(abs(y2-y1)>abs(x2-x1))
			plot_pixel(y,x,color);
		else
			plot_pixel(x,y,color);
		error = error +deltay;
		if(error>=0)
		{
			y+=y_step;
			error-=deltax;
		}
	}

}
void wait_for_vsync()
{
    volatile int *pixel_ctrl_ptr = (int *)0xff203020;
    register int status;
    *pixel_ctrl_ptr = 1;
    status = *(pixel_ctrl_ptr+3);
    while((status&0x01)!=0){
    status = *(pixel_ctrl_ptr+3);
    } 
}
void horizontalLine(int x, int y, int w, short int col) {
    for (int i = 0; i <w; i++){
        plot_pixel(x + i, y, col);
    }
}
void fillCircle(int x,int y, int r, short int col) {
    int xoff =0;
    int yoff= r;
    int balance = -r;
 
    while (xoff <= yoff) {
         int p0 = x - xoff;
         int p1 = x - yoff;
         
         int w0 = xoff + xoff;
         int w1 = yoff + yoff;
         
         horizontalLine(p0, y + yoff, w0, col);
         horizontalLine(p0, y - yoff, w0, col);
         
         horizontalLine(p1, y + xoff, w1, col);
         horizontalLine(p1, y - xoff, w1, col);
       
        if ((balance += xoff++ + xoff)>= 0) {
            balance-=--yoff+yoff;
        }
    }
}
void shooter(int x,int y, int r, short int col,double xDir, double yDir, int ballNum)
{
	for(int i=1;i<=ballNum;i++)
	{
		if(x+i*(xDir+(2*r))!=320&&y-i*(yDir+(2*r))!=0)
			fillCircle(x+i*(xDir+(2*r)),y-i*(yDir+(2*r)),r,col);
	}
}
/*void drawCircle(int xc, int yc, int x, int y, short int color) 
{ 
    plot_pixel(xc+x, yc+y, color); 
    plot_pixel(xc-x, yc+y, color); 
    plot_pixel(xc+x, yc-y, color); 
    plot_pixel(xc-x, yc-y, color); 
    plot_pixel(xc+y, yc+x, color); 
    plot_pixel(xc-y, yc+x, color); 
    plot_pixel(xc+y, yc-x, color); 
    plot_pixel(xc-y, yc-x, color); 
}

void circleBres(int xc, int yc, int r,short int color) 
{ 
    int x = 0, y = r; 
    int d = 3 - 2 * r; 
    drawCircle(xc, yc, x, y,color); 
    while (y >= x) 
    { 
        x++; 
        if (d > 0) 
        { 
            y--;  
            d = d + 4 * (x - y) + 10; 
        } 
        else
            d = d + 4 * x + 6; 
        drawCircle(xc, yc, x, y,color); 
    } 
}*/
