#include <stdio.h>
//#include <conio.h>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
using namespace std;

struct node
  {
    int burst;
    int requests;
    int priority;
    int cpuBurst = burst/requests;
    int readyTotal;
    int waitTotal;
    int ioTotal;
    bool isWaiting; //process is in waiting queue
    bool isReady;   //process is in ready queue
    bool isIO;  //process is in IO queue
    node *next;
  };

class list
  {
    private:
    node *head, *tail;
    public:
    list()
    {
      head=NULL;
      tail=NULL;
    }

  //Round Robin
  public:
  int rr_max_cpu_burst = 100;
  int rr_max_io_request = 50;
  int rr_max_waiting = 350;
  int rr_total_max_waiting = 2500; //keep track of the sum all waiting time, move to SJF if > 2500
  int rr_counter; //use this to keep track of how much time has been spent entirely

  //Shortest Job First
  int sjf_max_cpu_burst = 50;
  int sjf_max_io_request = 35;
  int sjf_total_max_waiting = 300;
  int sjf_counter;

  //Priority
void insert(int burst, int priority, int requests)
    {
      node *temp=new node;
      temp->burst=burst;
      temp->priority=priority;
      temp->requests=requests;
      //temp->next=NULL;
      if(head==NULL)
      {
        head=temp;
        tail=temp;
        temp=NULL;
      }
      else
      {
        tail->next=temp;
        tail=temp;
      }
    }

void deleteNode()
  {
    node *temp=new node;
    temp=head;
    head=head->next;
    delete temp;
  }

void setWaitTime (){
    //use burst time of current process to be waiting time of process in waiting queue
    int burst;
    node *temp1=new node;
    temp1=head;
    //use temp1 to find non waiting node
    bool found = false;
    while(!found){
        if(!temp1->isWaiting){
           burst = temp1->cpuBurst;
           found = true;
        }
        else{
            temp1=temp1->next;
        }
    }
    //add burst to processes that are waiting
    node *temp2=new node;
    temp2=head;
    for(int i=0; i<10; i++){
        bool isWaiting = temp2->isWaiting;
        int waitTotal = temp2->waitTotal;
        if(isWaiting){
           temp2->waitTotal = waitTotal+burst;
        }
        temp2 = temp2->next;
    }
}//end setWaitTime

void rrBurst(){
    do{
        int burst;
        node *temp1=new node;
        temp1 = head;
        //temp = temp1;
        //use temp1 to find non waiting node
        bool found = false;
        while(!found){
            if(!temp1->isWaiting){
               burst = temp1->cpuBurst;
               found = true;
            }
            else{
                temp1=temp1->next;
            }
        }
        //if there is an io request then after 50 move to waiting queue
        //else run for 100;
        temp1 = head;
        for(int i=0; i<10; i++){
            int requests = temp1->requests;
            bool isWaiting = temp1->isWaiting;
            int totalBurst = temp1->burst;
            if(requests > 1 && !isWaiting){
                temp1->requests--;//reduce requests
                temp1->cpuBurst = rr_max_io_request;
                temp1->burst = totalBurst - rr_max_io_request;//reduce burst
                temp1->isIO = true;//move to io
                temp1->isWaiting = true;//move to waiting
            }
            else if(requests < 1 && !isWaiting){
                temp1->requests--;//reduce requests
                temp1->cpuBurst = rr_max_cpu_burst;
                temp1->burst = totalBurst - rr_max_cpu_burst;//reduce burst
                temp1->isIO = true;//move to io
                temp1->isWaiting = true;//move to waiting
            }
            temp1 = temp1->next;
        }
        //calculate total waiting time
        setWaitTime();
        for(int i =0; i<10; i++){
                cout<<temp1->burst<<temp1->requests<<temp1->priority;
                temp1 = temp1->next;
        }
        //a process with higher priority cant wait more than 350
        //find running process temp2 and store its priority
        //if any waiting process temp3 has a higher priority & waittime>350, roundrobin(temp3)
        int priority = temp1->priority;
        node *temp3=new node;
        temp3 = head;
        node *temp4=new node;
        for(int i=0; i<10; i++){
            if(priority>temp3->priority && temp3->waitTotal>350){
                temp4 = temp3;
            }
            else{
                temp3=temp3->next;
            }
        }
        //rrBurst(temp4);

    }while(rr_counter < rr_total_max_waiting);
        node* temp1 = new node;
        temp1=head;
        //calculate waiting time of all processes
        for (int i =0; i<10; i++){
            rr_counter=rr_counter+temp1->waitTotal;
            temp1=temp1->next;
        }
        sjf();
  }//END RR BURST

  void sjf(){
    //use insertion sort into a new list
    node *temp2=new node;
    temp2 = head;
    int sjf[10];
    //copy priorities into array
    for(int i=0; i<10; i++){
        sjf[0]=temp2->burst;
        temp2 = temp2->next;
    }
    //sort array
    int tempsort;
    for(int i=0;i<10;i++)
    {
        for(int j=i+1;j<10;j++)
        {
            if(sjf[i]>sjf[j])
            {
                tempsort=sjf[i];
                sjf[i]=sjf[j];
                sjf[j]=tempsort;
            }
        }
    }
    //make temp3
    node* temp1 = new node;
    node *temp3=new node;
    temp3 = head;
    //if valueof(i) =
    for(int i =0; i<10; i++){
        if(sjf[i]==temp3->burst){
            temp1=temp3;
        }
        else{
            temp3 = temp3->next;
        }
    }
    do{
        //if there is an io request then after 35 move to waiting queue
        //else run for 50;
        for(int i=0; i<10; i++){
            int requests = temp1->requests;
            bool isWaiting = temp1->isWaiting;
            int totalBurst = temp1->burst;
            if(requests > 1 && !isWaiting){
                temp1->requests--;//reduce requests
                temp1->cpuBurst = sjf_max_io_request;
                temp1->burst = totalBurst - sjf_max_io_request;//reduce burst
                temp1->isIO = true;//move to io
                temp1->isWaiting = true;//move to waiting
            }
            else if(requests < 1 && !isWaiting){
                temp1->requests--;//reduce requests
                temp1->cpuBurst = sjf_max_cpu_burst;
                temp1->burst = totalBurst - sjf_max_cpu_burst;//reduce burst
                temp1->isIO = true;//move to io
                temp1->isWaiting = true;//move to waiting
            }
            temp1 = temp1->next;
        }
        //calculate total waiting time
        setWaitTime();
        temp1=head;
        //calculate waiting time of all processes
        for (int i =0; i<10; i++){
            sjf_counter=sjf_counter+temp1->waitTotal;
            temp1=temp1->next;
        }
    }while(sjf_counter<sjf_total_max_waiting);
    //SWITCH TO PRIORITY ONCE SJF COUNTER = 350
    //priority();
  }//END SJF

};

int main(){
    list obj;
    obj.insert(100,2,10);
    obj.insert(5000,50,5);
    obj.insert(250,4,7);
    obj.insert(1000,14,3);
    obj.insert(3000,10,2);
    obj.insert(160,1,8);
    obj.insert(160,2,9);
    obj.insert(4000,30,4);
    obj.insert(3500,40,1);
    obj.insert(1500,5,6);
    obj.rrBurst();
}
