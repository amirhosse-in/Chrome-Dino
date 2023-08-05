#include <stdio.h>
#include <stdlib.h>
#include<time.h>
#include <unistd.h>
#include <ncurses.h>
#define height 50
#define width 100

int game_state = 1;
int cactus_state =0;
int cactus_x[2] = {100,50};
int cloud_x[2] = {10, 80};
int dinasour_state =0;
char map[height][width] = {0};
int dinasour_coordinate[100][3] ={0};
int cactus_coordinate[2][100][3] = {0};
int person_coordinate[200][2] = {0};
int cloud_coordinate[2][100][3] = {0};
int ground_state =0;
int dinasour_y = 39;
int is_jumping =0;
int is_lost=0;
int arrow_x =0;
int score = 0;
int ful_score =0 ;
int randed_number =-15;
int new_randed = -5;
int time_to_sleep = 50000;
void print_map();
void set_dinasour_in_map(int x,int y,int state);
void set_person_in_map(int x, int y);
void lost();
void init_map();

char cloud[2][6][29] ={{
{"          .-~~~-.            "},
{"  .- ~ ~-(       )_ _        "},
{" /                     ~ -.  "},
{"|                           \\"},
{" \\                         .'"},
{"   ~- . _____________ . -~   "}
},
{
{"   __   _                    "},
{" _(  )_( )_                  "},
{"(_   _    _)                 "},
{"  (_) (__)                   "},
{"                             "},
{"                             "}
}};
char person[11][22] ={
{"                 .,   "},
{"        .    ____/__, "},
{"      .' \\  / \\==\\``` "},
{"     /    \\ 77 \\ |    "},
{"    /_.----\\\\__,-----."},
{"<--(\\_|_____<__|_____/"},
{"    \\  ````/|   ``/```"},
{"     `.   / |    I|   "},
{"       `./  |____I|   "},
{"            !!!!!!!   "},
{"         ------------ "}
};
char cactus[2][8][12] = {
    {
{"    ,*-.    "},
{"    |  |    "},
{",.  |  |    "},
{"| |_|  | ,. "},
{"`---.  |_| |"},
{"    |  .--` "},
{"    |  |    "},
{"    |  |    "}
    },
    {
{"   _  _     "},
{"  | || | _  "},
{" -| || || | "},
{"  | || || | "},
{"   \\_  || | "},
{"     |  _/  "},
{"    -| |    "},
{"     |_|-   "}
}};
char dinasour [3][10][12] = {
    //first state
    {
    {"     ####   "},
    {"     # ##   "},
    {"     ####   "},
    {"#   ##      "},
    {"#  ######   "},
    {"######  #   "},
    {"######      "},
    {"#   #       "},
    {"#   ##      "},
    {"##          "}
    },
    //second state
    {
        {"     ####   "},
        {"     # ##   "},
        {"     ####   "},
        {"#   ##      "},
        {"#  ######   "},
        {"######  #   "},
        {"######      "},
        {"#   #       "},
        {"##  #       "},
        {"    ##      "}
    },
    //third state
    {
       {"            "},
       {"            "},
       {"            "}, 
       {"        ####"},
       {"        # ##"},
       {"     #######"},
       {"########    "},
       {"#  ######   "},
       {"   #   #    "},
       {"   ##  ##   "},
   }
};

void set_value_to_array(int array[100][3], int k){
    for(int i=0;i<100;i++) array[i][0] = array[i][1] = array[i][2] =k;
}
void set_arrow(int y)
{
    map[25][75] = '<';
    map[y][arrow_x] = map[y][arrow_x+1] = map[y][arrow_x+2] = map[y][arrow_x+3] = ' ';
    map[25][87] = (int)'<'; map[25][90] = (int)'|'; map [25][93] = '_';
    set_person_in_map(20,75);
    if( cactus_x[0] - 44+new_randed<2 && cactus_x[0] - 44+new_randed >-2) {
        new_randed= -(rand()%10);
        arrow_x = 73;
        map[25][87] = (int)'_'; map[25][90] = (int)'<'; map [25][93] = '|';
    }
    arrow_x-=2;
    if(arrow_x>-1){
    map[y][arrow_x] = '<';
        map[y][arrow_x+1] = map[y][arrow_x+2] = map[y][arrow_x+3] = '-';}
}
void set_person_in_map(int x, int y){
    for(int i=0;i<11;i++){
        for(int j=0;j<22;j++){
            if(person[i][j] != ' '){
                map[x+i][y+j] = person[i][j];
            }
        }
    }
}
void set_cloud_in_map(int y, int state){
    for(int i=0;i<100;i++) if(cloud_coordinate[state][i][0] != -1) map [cloud_coordinate[state][i][0]][cloud_coordinate[state][i][1]] = ' ';
    for(int i=0;i<100;i++) cloud_coordinate[state][i][0] = cloud_coordinate[state][i][1] = -1;
    int iterator =0;
    for(int i=0;i<6;i++){
        for(int j=0;j<29;j++){
            if(cloud[state][i][j] != ' '){
                cloud_coordinate[state][iterator][0] = y+i;
                cloud_coordinate[state][iterator][1] = cloud_x[state]+j;
                cloud_coordinate[state][iterator][2] = (int) cloud[state][i][j];
                if(y+j<0 || y+j>width-1) cloud_coordinate[state][iterator][0] = -1;
                iterator++;
            }
        }
    }
    if(state) cloud_x[state]-=2;
    else cloud_x[state] -=1;
    if(cloud_x[state] <-40) cloud_x[state]=105;
    for(int i=0;i<100;i++) if(cloud_coordinate[state][i][0] != -1 )  map [cloud_coordinate[state][i][0]][cloud_coordinate[state][i][1]] = (char)cloud_coordinate[state][i][2];
}
void set_cactus_in_map(int x, int y,int state){
    //clear previous 
    for(int i=0;i<100;i++) if(cactus_coordinate[state][i][0] != -1)  map [cactus_coordinate[state][i][0]][cactus_coordinate[state][i][1]] = ' ';
    //set new cactus
    for(int i=0;i<100;i++) cactus_coordinate[state][i][0] = cactus_coordinate[state][i][1] = -1;
    int iterator=0;
    for(int i=0;i<8;i++){
        for(int j=0;j<12;j++){
            if(cactus[state][i][j] != ' '){
                cactus_coordinate[state][iterator][0] = x+i;
                cactus_coordinate[state][iterator][1] = y+j;
                cactus_coordinate[state][iterator][2] = (int) cactus[state][i][j];
                if(y+j<0 || y+j>width-1) cactus_coordinate[state][iterator][0] = -1;
                iterator++;
            }
        }
    }
    cactus_x[state] -=2;
    if(cactus_x[state] <randed_number){
        cactus_x[state]=100;
        randed_number = -(rand()%20+10);
    }
    for(int i=0;i<100;i++) if(cactus_coordinate[state][i][0] != -1 )  map [cactus_coordinate[state][i][0]][cactus_coordinate[state][i][1]] = (char)cactus_coordinate[state][i][2];
}
void lost(){
    set_dinasour_in_map(dinasour_y,20,dinasour_state);
    print_map();
    if(is_lost)usleep(2000000);
    init_map();
    refresh();
    
    if(is_lost){
        if(score>=0){
           // move(30,0);
            printw("You lost :(\n\nYour Score : %d",score);}
        else printw("\t\t\tYou lost :(\n\n\t\t\tYour Score : %d (but real score is more than zero)", score);
    }
    else {
        if(score>=0)
        printw("FINISH :D\n Your Score: %d",score);
        else printw("FINISH :D\n Your Score: %d\n(but real score is more than zero)",score);
    }
    print_map();
    getchar();
    exit(0);
}
void set_dinasour_in_map(int x,int y,int state){
    //clear previous
    for(int i=0;i<100;i++) if( dinasour_coordinate[i][0] != -1) map [dinasour_coordinate[i][0]][dinasour_coordinate[i][1]] = ' ';
    //set new dinasour
    set_value_to_array(dinasour_coordinate, -1);
    int iterator = 0;
    for(int i=0;i<10;i++){
        for(int j=0;j<12;j++){
            if(dinasour[state][i][j] == '#'){
                dinasour_coordinate[iterator][0] = x+i;
                dinasour_coordinate[iterator][1] = y+j;
                dinasour_coordinate[iterator][2] = (int)dinasour[state][i][j];
                if(map[x+i][y+j] != (int)' ') is_lost= 1;
                iterator++;
            }
        }
    }
    for(int i=0;i<100;i++) if(dinasour_coordinate[i][0] != -1) map[dinasour_coordinate[i][0]][dinasour_coordinate[i][1]] = (char)dinasour_coordinate[i][2];
    dinasour_state = 1 - dinasour_state;
    for(int i=0;i<100;i++){
        for(int j=0;j<100;j++){
        if(dinasour_coordinate[i][0] != -1){

            if(cactus_coordinate[0][j][0] != -1 && cactus_coordinate[0][j][0] == dinasour_coordinate[i][0] && cactus_coordinate[0][j][1] == dinasour_coordinate[i][1])
            {
                printf("\a\n");
                is_lost=1;
            }
            if(cactus_coordinate[1][j][0] != -1 && cactus_coordinate[1][j][0] == dinasour_coordinate[i][0] && cactus_coordinate[1][j][1] == dinasour_coordinate[i][1])
            {
                printf("\a\n");
                is_lost=1;
            }
        }
        }
        
    }

}

void init_map(){
    for(int i=0;i<height;i++) for(int j=0;j<width;j++) map[i][j] =' ';
}
void print_map(){
    for(int i=0;i<width; i++) {
        if(i%5 == ground_state) map[height-1][i] = '@';
            else map[height-1][i] = '#';
    }
    ground_state--;
    if(ground_state <=-1) ground_state=5;
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            printw("%c", map[i][j]);
        }
        printw("\n");
    }
    move(0,0);
    refresh();
    usleep(time_to_sleep);
}

void jump(){
    for(int i=6;i>0;i--){
            dinasour_y -= i;
            set_arrow(25);
            set_dinasour_in_map(dinasour_y,20,dinasour_state);
            set_cactus_in_map(42,cactus_x[0] , 0);
            set_cactus_in_map(42,cactus_x[1] , 1);
            set_cloud_in_map(2,0);
            set_cloud_in_map(7,1);
            ful_score++; score=ful_score/5;printw("%d",score);
            print_map();
            if(is_lost) lost();
        
    }
    for(int i=0;i<1;i++) {
        set_arrow(25);
        set_dinasour_in_map(dinasour_y,20,dinasour_state);
        set_cactus_in_map(42,cactus_x[0] , 0);
        set_cactus_in_map(42,cactus_x[1] , 1);
        set_cloud_in_map(2,0);
        set_cloud_in_map(7,1);
        ful_score++; score=ful_score/5;printw("%d",score); print_map();if(is_lost) lost();
    }
    for(int i=0;i<=6;i++){
            dinasour_y += i;
            set_arrow(25);
            set_dinasour_in_map(dinasour_y,20,dinasour_state);
            set_cactus_in_map(42,cactus_x[0] , 0);
            set_cactus_in_map(42,cactus_x[1] , 1);
            set_cloud_in_map(2,0);
            set_cloud_in_map(7,1);
            
            ful_score++; score=ful_score/5;printw("%d",score);
            print_map();
            if(is_lost) lost();
        
    }
    is_jumping =0 ;
}

int main(){
    
    init_map();
    char key_press = '~';
    char c;
    int counter=0;
    int color_state=1;
    srand(time(0));
    
    initscr();
    start_color();
    init_pair(1,COLOR_GREEN,COLOR_BLACK);
    init_pair(2,COLOR_GREEN, COLOR_BLACK);
    init_pair(3,COLOR_MAGENTA, COLOR_WHITE);
    wattron(stdscr, COLOR_PAIR(1));
    printw("Hey, press a key to start :D");
    noecho();
    keypad(stdscr,TRUE);
    getch();
    printf("\a\n");
    wrefresh(stdscr);
    curs_set(FALSE);
    int ch = '.';
    nodelay(stdscr, TRUE);
    set_person_in_map(20,75);
    while(game_state){
        if(!is_jumping) ch = getch();
        if (ch == ERR){
            
           // system("clear && printf '\e]3J'");
            set_arrow(25);
            set_dinasour_in_map(dinasour_y,  20,dinasour_state);
            set_cactus_in_map(42,cactus_x[0] , 0);
            set_cactus_in_map(42,cactus_x[1] , 1);
            set_cloud_in_map(2,0);
            set_cloud_in_map(7,1);
            ful_score++; score=ful_score/5;
            printw("%d",score);
            print_map();
            if(is_lost){ lost();}
            
        }else {
            if((ch == KEY_UP || ch == (int)' ')&& (is_jumping==0)) {is_jumping =1;jump();}
            else if(ch == 27) lost();
            else if(ch == (int)'G') {
                ful_score+=5;
                score = ful_score/5;
                printw("%d",score);
                print_map();
            }
            else if(ch == (int)'H'){
                ful_score-=5;
                score = ful_score/5;
                printw("%d",score);
                print_map();
            }
        }
    }
    getch();
    return 0;
}
