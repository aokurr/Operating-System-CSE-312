
#include <common/types.h>
#include <gdt.h>
#include <memorymanagement.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/pci.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>
#include <gui/desktop.h>
#include <gui/window.h>
#include <multitasking.h>


// #define GRAPHICSMODE


using namespace myos;
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;
using namespace myos::gui;



void printf(char* str)
{
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;

    static uint8_t x=0,y=0;

    for(int i = 0; str[i] != '\0'; ++i)
    {
        switch(str[i])
        {
            case '\n':
                x = 0;
                y++;
                break;
            default:
                VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | str[i];
                x++;
                break;
        }

        if(x >= 80)
        {
            x = 0;
            y++;
        }

        if(y >= 25)
        {
            for(y = 0; y < 25; y++)
                for(x = 0; x < 80; x++)
                    VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
            x = 0;
            y = 0;
        }
    }
}

void printfHex(uint8_t key)
{
    char* foo = "00";
    char* hex = "0123456789ABCDEF";
    foo[0] = hex[(key >> 4) & 0xF];
    foo[1] = hex[key & 0xF];
    printf(foo);
}

char input='a';


class PrintfKeyboardEventHandler : public KeyboardEventHandler
{
public:
    void OnKeyDown(char c)
    {
        char* foo = " ";
        foo[0] = c;
        input=c;
        printf(foo);
    }
};

class MouseToConsole : public MouseEventHandler
{
    int8_t x, y;
public:
    
    MouseToConsole()
    {
        uint16_t* VideoMemory = (uint16_t*)0xb8000;
        x = 40;
        y = 12;
        VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                            | (VideoMemory[80*y+x] & 0xF000) >> 4
                            | (VideoMemory[80*y+x] & 0x00FF);        
    }
    
    virtual void OnMouseMove(int xoffset, int yoffset)
    {
        static uint16_t* VideoMemory = (uint16_t*)0xb8000;
        VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                            | (VideoMemory[80*y+x] & 0xF000) >> 4
                            | (VideoMemory[80*y+x] & 0x00FF);

        x += xoffset;
        if(x >= 80) x = 79;
        if(x < 0) x = 0;
        y += yoffset;
        if(y >= 25) y = 24;
        if(y < 0) y = 0;

        VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                            | (VideoMemory[80*y+x] & 0xF000) >> 4
                            | (VideoMemory[80*y+x] & 0x00FF);
    }
    
};



int input_int(char number){
    switch(number){
        case '0': return 0;
        case '1': return 1;
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        case '7': return 7;
        case '8': return 8;
        case '9': return 9;
        default:printf("Incorrect entry please try again");
    }
    return -1;

}






/***********************************************HOMEWORK 2**************************************************************/



int *array;
int *memory;
int *disc;
int *R_bit;
int size_of_array;
int size_of_memory;
int size_of_disc;
int itr=0;
int *Lru_Rbit;
uint32_t time1=39000000;

uint32_t time2=0;

void print_ahmet(int number){
    printf("0x");
    printfHex(((size_t)number >> 24) & 0xFF);
    printfHex(((size_t)number >> 16) & 0xFF);
    printfHex(((size_t)number >> 8 ) & 0xFF);
    printfHex(((size_t)number      ) & 0xFF);
}
int make_int(){
    int count=0;
    int digit=0;
    int number=0;
    int int_input;
    while(true){
        
        if(count==0 && (input=='1' ||input=='2' ||input=='3' ||input=='4' ||input=='5' ||input=='6' ||input=='7' ||input=='8' ||input=='9')){
            count++;
            int_input=input_int(input);
            if(int_input!=-1){
                number=number*10;
                number=number+int_input;
            }
        }
        
        else if(input=='e'){
            count=0;
        }
        if(input=='q'){
            return number;
        }
    }

}
/*************************FİLL*********************************************/
void fill_array(){
    array[0]=7;
    array[1]=8;
    array[2]=1;
    array[3]=4;
    array[4]=13;
    array[5]=22;
    array[6]=32;
    array[7]=23;
    array[8]=37;
    array[9]=4;
    array[10]=65;
    array[11]=45;
    array[12]=41;
    array[13]=2;
    array[14]=91;
    array[15]=34;
}

void fill_memory(){
    int j=size_of_array-1;
    /*arrayin başından biraz eleman memorye konur*/
    for(int i=0;i<(size_of_memory/2);i++){
        memory[i]=array[i];
        R_bit[i]=0;
        Lru_Rbit[i]=0;
    }
   /*geri kalan sondan konur*/ 
    for(int i=(size_of_memory/2);i<size_of_array;i++){
        memory[i]=array[j--];
        R_bit[i]=0;
        Lru_Rbit=0;
    }

}
void fill_disc(){
    int count=0;
    for(int j=0;j<size_of_disc;j++)
        disc[j]=0;
    

    for(int i=(size_of_memory)/2;i<size_of_array-(size_of_memory/2);i++)
        disc[count++]=array[i];
}


/************************************************************************************************/


/************************REPLACEMENT ALGORİTHM********************************/

int hit=0;
int miss=0;
/* i=index1  j=index2 */

void fifoPR(int i,int j){
    int count;
    int flag1=0;
    int flag2=0;
    int tmp;
    
    /*Checking whether the page exists in memory or not*/
    for(count=0;count<size_of_memory;count++){
        if(array[i]==memory[count]){
            flag1=1;  
        }
        if(array[j]==memory[count]){
            flag2=1;  
        }
    }
    
    if(flag1==1){
        hit++;

    }
    
    else{
        miss++;
        
        /*page is written from disk to memory*/
        for(count=0;count<size_of_disc;count++){
            if(array[i]==disc[count]){
                
                /*memory kaydırma işlemi dairesel arrey şeklinde yapılmıştır*/
                tmp=memory[itr];
                memory[itr]=disc[count];
                disc[count]=tmp;
                itr++;
                itr=itr%size_of_memory;        
            }
        }

    }

    if(flag2==1){
        hit++;

    }
    /*miss olması durumunda disc ten memorye bilgi çekilir*/
    
    else{
        miss++;  
        for(count=0;count<size_of_disc;count++){
            if(array[j]==disc[count]){
                
                /*memory kaydırma işlemi dairesel arrey şeklinde yapılmıştır*/
                tmp=memory[itr];
                memory[itr]=disc[count];
                disc[count]=tmp;
                itr++;
                itr=itr%size_of_memory;        
            }
        }

    }
    
}


/* i=index1  j=index2 */
void SCfifoPR(int i,int j){
    int count;
    int flag1=0;
    int flag2=0;
    int tmp;
    /*Checking whether the page exists in memory or not*/
    for(count=0;count<size_of_memory;count++){
        if(array[i]==memory[count]){
            flag1=1;
            R_bit[count]=1;  
        }
        if(array[j]==memory[count]){
            flag2=1;
            R_bit[count]=1;
        }
    }
    
    if(flag1==1){
        hit++;
        
    }
    /*miss olması durumunda disc ten memorye bilgi çekilir*/
    
    else{
        
        for(count=0;count<size_of_disc;count++){
            if(array[i]==disc[count]){
                miss++;
                /*memory kaydırma işlemi dairesel arrey şeklinde yapılmıştır*/
                while(R_bit[itr]==1){
                    R_bit[itr]=0;
                    itr++;
                    itr=itr%size_of_memory;
                }
                tmp=memory[itr];
                memory[itr]=disc[count];
                R_bit[itr]=1;
                disc[count]=tmp;
                itr++;
                itr=itr%size_of_memory;
                break;
            }
        }


    }

    if(flag2==1){

        hit++;
    }
    /*miss olması durumunda disc ten memorye bilgi çekilir*/
    
    else{
        
        for(count=0;count<size_of_disc;count++){
            if(array[j]==disc[count]){
                miss++;
                while(R_bit[itr]==1){
                    R_bit[itr]=0;
                    itr++;
                    itr=itr%size_of_memory;
                }
                
                /*memory kaydırma işlemi dairesel arrey şeklinde yapılmıştır*/
                tmp=memory[itr];
                memory[itr]=disc[count];
                R_bit[itr]=1;
                disc[count]=tmp;
                itr++;
                itr=itr%size_of_memory;
                break;     
            }
        }
    }

    
}

void LRU_algorithm(int i,int j){
    int flag1=0;
    int flag2=0;
    int k;
    int min;
    int min_i=0;
    int tmp;
    /*Checking whether the page exists in memory or not*/
    for(k=0;k<size_of_memory;k++){
        if(array[i]==memory[k]){
            flag1=1;
            Lru_Rbit[k]++;
            hit++;

        }
        if(array[j]==memory[k]){
            flag2=1;
            Lru_Rbit[k]++;
            hit++;
        }
    }
    
    if(flag1==0){
        miss++;

        int min=Lru_Rbit[0];
        
        for(k=0;k<size_of_disc;k++){
            
            if(array[i]==disc[k]){
                
                /*find minimum R BİT PAGE*/
                for(int a=0;a<size_of_memory;a++){
                    if(min>Lru_Rbit[a]){
                        min=Lru_Rbit[a];
                        min_i=a;
                    }
                }
                
                /*swap between memory disc*/
                Lru_Rbit[min_i]=1;
                tmp=memory[min_i];
                memory[min_i]=disc[k];
                disc[k]=tmp;
                k=size_of_disc;
            }
        }
    }

    if(flag2==0){
        miss++;

        int min=Lru_Rbit[0];
        
        for(k=0;k<size_of_disc;k++){
            
            if(array[j]==disc[k]){
                
                /*find minimum R BİT PAGE*/
                for(int a=0;a<size_of_memory;a++){
                    if(min>Lru_Rbit[a]){
                        
                        min=Lru_Rbit[a];
                        min_i=a;
                    }
                }
                
                /*swap between memory disc*/
                Lru_Rbit[min_i]=1;
                tmp=memory[min_i];
                memory[min_i]=disc[k];
                disc[k]=tmp;
                k=size_of_disc;

            }
        }
    }
}
/********************************************************************************/




/**************************************SORTS**********************************/

void bubbleSort(int n)
{
   int i, j;
   int swapped;
   int tmp;
   int count;
   
   printf("\nplease enter the page replecament algorithm\n");
   printf("a)FIFO\nb)Second Change FIFO\nc)LRU algorithm");
   while(input!='a' && input!='b' && input!='c');
   for (i = 0; i < n-1; i++)
   {
     swapped = 0;
     for (j = 0; j < n-i-1; j++)
     {
            
        switch(input){
            case 'a':fifoPR(j,j+1);
                break;
            case 'b':SCfifoPR(j,j+1);
                    break;
            case 'c':LRU_algorithm(j,j+1);
                    break;
            default : printf("please try again");
        }

        if (array[j] > array[j+1])
        {
            /*swap*/
            tmp=array[j+1];
            array[j+1]=array[j];
            array[j]=tmp;
            swapped = 1;
        }
      /*  for(count=0;count<100000000;count++);*/
     }
 
     // IF no two elements were swapped
     // by inner loop, then break
     if (swapped == 0)
        break;
   }
    

    /*
            DEMODA AÇIALCAK
    for(count=0;count<10000000;count++);
    for(count=0;count<10000000;count++);*/
}

int partition (int low, int high)
{
    int pivot = array[high]; // pivot
    int i = (low - 1); // Index of smaller element and indicates the right position of pivot found so far
    int tmp;
    for (int j = low; j <= high - 1; j++)
    {
        
        switch(input){
            case 'a':fifoPR(i,j);
                break;
            case 'b':SCfifoPR(i,j);
                    break;
            case 'c':LRU_algorithm(i,j);
                    break;
            default : printf("please try again");
        }

        // If current element is smaller than the pivot
        if (array[j] < pivot)
        {
            i++; // increment index of smaller element
            tmp=array[i];
            array[i]=array[j];
            array[j]=tmp;
        }
    }
    
    switch(input){
        case 'a':fifoPR(i+1,high);
                break;
        case 'b':SCfifoPR(i+1,high);
                break;
        case 'c':LRU_algorithm(i,high);
                break;
        default : printf("please try again");
    }


    tmp=array[i+1];
    array[i+1]=array[high];
    array[high]=tmp;
    return (i + 1);
}
 
/* The main function that implements QuickSort
arr[] --> Array to be sorted,
low --> Starting index,
high --> Ending index */
int QScount=0;
void quickSort(int low, int high)
{   
    if(QScount==0){
        QScount++;
        printf("\nplease enter the page replecament algorithm\n");
        printf("a)FIFO\nb)Second Change FIFO\nc)LRU algorithm");
        while(input!='a' && input!='b' && input!='c');
    }
    if (low < high)
    {
        /* pi is partitioning index, arr[p] is now
        at right place */
        int pi = partition(low, high);
 
        // Separately sort elements before
        // partition and after partition
        quickSort(low, pi - 1);
        quickSort(pi + 1, high);
    }
}

void insertionSort(int n)
{
    int i, key, j;
    printf("\nplease enter the page replecament algorithm\n");
    printf("a)FIFO\nb)Second Change FIFO\nc)LRU algorithm");
    while(input!='a' && input!='b' && input!='c');
    for (i = 1; i < n; i++)
    {
        
        key = array[i];
        j = i - 1;
 
        // Move elements of arr[0..i-1], 
        // that are greater than key, to one
        // position ahead of their
        // current position
        switch(input){
            case 'a':fifoPR(j,j+1);
                break;
            case 'b':SCfifoPR(j,j+1);
                    break;
            case 'c':LRU_algorithm(j,j+1);
                    break;
            default : printf("please try again");
        }
        while (j >= 0 && array[j] > key)
        {
            array[j + 1] = array[j];
            j = j - 1;
        }
        array[j + 1] = key;
    }
}


/***********************************************************************************/


void print_array(){
    printf("\n");
    for(int i=0;i<size_of_array;i++){
        print_ahmet(array[i]);
        printf("-");
        if(i%3==0)
            printf("\n");
    }
}
void print_memory(){
    printf("\n");
    for(int i=0;i<size_of_memory;i++){
        print_ahmet(memory[i]);
        printf("-");
        if(i%3==0)
            printf("\n");
    }
}
void print_disc(){
    printf("\n");
    for(int i=0;i<size_of_disc;i++){
        print_ahmet(disc[i]);
        printf("-");
        if(i%3==0)
            printf("\n");
    }
}


void taskA()
{
    int count=0;
    printf("please enter the array size\n");
    while(true){
        /*create array*/
        if(count==0 && input!='q'){
            
            size_of_array=make_int();
            count++;
            array=new int[size_of_array];
            printf("\n size of array =");
            print_ahmet(size_of_array);
            printf("\nplease enter the memory size\n");
        }
        
        /*create memory array*/
        else if(count==1 && input!='q'){
            size_of_memory=make_int();
            count++;
            memory=new int[size_of_memory];
            memory[0]=size_of_memory;
            printf("\nsize of memory=");
            print_ahmet(memory[0]);
            printf("\nplease enter the disc size\n");
        }
        
        
        /*create disc array*/
        else if(count==2 && input!= 'q'){
           
            size_of_disc=make_int();
            count++;
            disc=new int[size_of_disc];
            printf("\nsize of disc=");
            print_ahmet(size_of_disc);
        }

        /*error check memory and array*/
        
        else if(count==3 && (size_of_memory>=size_of_array || size_of_disc + size_of_memory<size_of_array)){
            if(size_of_memory>=size_of_array){
                printf("\n*****!!ERROR!!*****\n");
                printf("size of memory > size of array PLEASE TRY AGAIN!! \n");
            }
            else{
                printf("\n*****!!ERROR!!*****\n");
                printf("size of disc + size_of_memory < size of array PLEASE TRY AGAIN!! \n"); 
            }
            count=0;
            delete [] array;
            delete [] memory;
            delete [] disc;
            printf("please enter the array size\n");
        }
        
        else if(count==3 && input=='q'){
            fill_array();
            fill_memory();
            fill_disc();
            count++;
        }
        
        else if(count==4){
            count++;
            printf("\n\n****Please select the sorting algorithm****\n");
            printf("1) Bubble sort\n");
            printf("2) Quick sort\n");
            printf("3) Insertion sort\n");

            while(input!='1' && input!='2' && input!= '3');
            printf("\nARRAY FIRST\n");
            print_array();
            hit=0;
            miss=0;
            while(input=='q');
            time2=0;
            switch(input){
                case '1':
                        printf("\nBUBBLE SORT\n");
                        bubbleSort(size_of_array);
                        printf("\n\n\n\n\n\n");
                        printf("\n\n\n\n\n\n");
                        break;
                case '2':
                        printf("\nQUICK SORT SORT\n");
                        quickSort(0,size_of_array-1);
                        printf("\n\n\n\n\n\n");
                        printf("\n\n\n\n\n\n");
                        //printf("\n\n\n\n\n\n");

                        break;

                case '3':
                        printf("\nINSERTION SORT SORT\n");
                        insertionSort(size_of_array);
                        printf("\n\n\n\n\n\n");
                        printf("\n\n\n\n\n\n");
                       // printf("\n\n\n\n\n\n");
                        break;

            }
            uint32_t time=time2/time1;
            printf("array\n");
            print_array();
            
            printf("time=");
            print_ahmet(time);
            
            printf("\nhit/total=");
            int total=(hit+miss);
            
            print_ahmet(hit);
            printf("/");
            print_ahmet(total);

            printf("\nmiss/total=");
            print_ahmet(miss);
            printf("/");
            print_ahmet(total);
            
            printf("\nmiss/time=");
            print_ahmet(miss/time);

            printf("\nhit/time=");
            print_ahmet(miss);

            count=0;
            printf("\nplease enter the array size\n");
            input='m';
        }
    }
}


/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^HOMEWORK 2^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

void taskB(){
    while(true){
        time2++;
    }
}

void taskC(){
    if(time2!=0){
        print_ahmet(time2);
        printf("\n");
    }
}



typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for(constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}



extern "C" void kernelMain(const void* multiboot_structure, uint32_t /*multiboot_magic*/)
{
    printf("Hello World! --- http://www.AlgorithMan.de\n");

    GlobalDescriptorTable gdt;
    
    
    uint32_t* memupper = (uint32_t*)(((size_t)multiboot_structure) + 8);
    size_t heap = 10*1024*1024;
    MemoryManager memoryManager(heap, (*memupper)*1024 - heap - 10*1024);
    /*
    printf("heap: 0x");
    printfHex((heap >> 24) & 0xFF);
    printfHex((heap >> 16) & 0xFF);
    printfHex((heap >> 8 ) & 0xFF);
    printfHex((heap      ) & 0xFF);
    */
    void* allocated = memoryManager.malloc(1024);
    /*printf("\nallocated: 0x");
    printfHex(((size_t)allocated >> 24) & 0xFF);
    printfHex(((size_t)allocated >> 16) & 0xFF);
    printfHex(((size_t)allocated >> 8 ) & 0xFF);
    printfHex(((size_t)allocated      ) & 0xFF);
    printf("\n");*/
    
   TaskManager taskManager;
    
    Task task1(&gdt, taskA);
    taskManager.AddTask(&task1);
   Task task2(&gdt, taskB);
   
    taskManager.AddTask(&task2);
    
  /* BU KISIM DEMODA GÖSTERİLECEK
   Task task3(&gdt, taskC);
   
    taskManager.AddTask(&task3);*/        
    
    InterruptManager interrupts(0x20, &gdt, &taskManager);
    
    printf("Initializing Hardware, Stage 1\n");
    
    #ifdef GRAPHICSMODE
        Desktop desktop(320,200, 0x00,0x00,0xA8);
    #endif
    
    DriverManager drvManager;
    
        #ifdef GRAPHICSMODE
            KeyboardDriver keyboard(&interrupts, &desktop);
        #else
            PrintfKeyboardEventHandler kbhandler;
            KeyboardDriver keyboard(&interrupts, &kbhandler);
        #endif
        drvManager.AddDriver(&keyboard);
        
    
        #ifdef GRAPHICSMODE
            MouseDriver mouse(&interrupts, &desktop);
        #else
            MouseToConsole mousehandler;
            MouseDriver mouse(&interrupts, &mousehandler);
        #endif
        drvManager.AddDriver(&mouse);
        
        PeripheralComponentInterconnectController PCIController;
        PCIController.SelectDrivers(&drvManager, &interrupts);

        VideoGraphicsArray vga;
        
    printf("Initializing Hardware, Stage 2\n");
        drvManager.ActivateAll();
        
    printf("Initializing Hardware, Stage 3\n");

    #ifdef GRAPHICSMODE
        vga.SetMode(320,200,8);
        Window win1(&desktop, 10,10,20,20, 0xA8,0x00,0x00);
        desktop.AddChild(&win1);
        Window win2(&desktop, 40,15,30,30, 0x00,0xA8,0x00);
        desktop.AddChild(&win2);
    #endif


    interrupts.Activate();
    
    while(1)
    {
        #ifdef GRAPHICSMODE
            desktop.Draw(&vga);
        #endif
    }
}
