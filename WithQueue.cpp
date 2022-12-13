#include<bits/stdc++.h>
#define channel 10
#define lambda1 0.1 //20 //corresponding to PU
#define lambda2 0.2 //10  //corresponding to RSU
#define lambda3 0.3 //15 //corresponding to NRSU
#define mu1 2
#define mu2 4
#define mu3 4
#define newBufferSize 5
#define handoffBufferSize 7
using namespace std;

class Channels{
    public:
    string typeOfUser;
    double busyTill;
};

int main(){
    int simTime;
    cin>>simTime;

    vector<Channels> server(channel, {"", 0.0}); //so a vector of channels is created
    random_device rd;
    mt19937 gen(rd());
    exponential_distribution<>PUArr(lambda1);
    exponential_distribution<>RSUArr(lambda2);
    exponential_distribution<>NRSUArr(lambda3);
    exponential_distribution<>PUSer(mu1);
    exponential_distribution<>RSUSer(mu2);
    exponential_distribution<>NRSUSer(mu3);

    // minheap of pairs
    priority_queue<pair<double, string>, vector<pair<double, string>>, greater<pair<double, string>>> pq;
    double time =0.0;
    double iatPU, iatRSU, iatNRSU, natPU, natRSU, natNRSU, stPU, stRSU, stNRSU;
    // double totalServiceTime =0.0;
    queue<double> newBuffer, handoffBuffer;
    cout<<"Simulation Started"<<endl;
    map<double, int> arrivalCount; //this will keep track of the number of users arrived at any point of time 
    map<double, int> departureCount; //this will keep track of the number of users departed at any point of time 
    int blockedUsers =0; //this will keep track of count of blocked users
    int preemptedUsers =0; //this will keep track of count of preempted users
    bool flag1, flag2;

    while(time<simTime){
        iatPU = PUArr(gen)*10.0;
        iatRSU = RSUArr(gen)*10.0;
        iatNRSU = NRSUArr(gen)*10.0;
        natNRSU = time + iatNRSU; //next arrival time of NRSU
        natPU = time + iatPU;
        natRSU = time + iatRSU;
        pq.push({natNRSU, "NRSU"});
        pq.push({natPU, "PU"});
        pq.push({natRSU, "RSU"});

        auto nextArrival = pq.top();
        pq.pop();
        time = nextArrival.first;
        cout<<"Time of next arrival "<<time<<endl;
        string userType = nextArrival.second;

        //this loop will check that how many PUs, RSUs, NRSUs are in service. This also eliminates the possibility of considering departure as event
        int cntRSU=0, cntNRSU=0, cntPU=0; //this represent how many PU, RSU, NRSU are currently in service
        for(int i=0; i<channel; i++){
            if(time < server[i].busyTill){//this will be a part of relevant count
                if(server[i].typeOfUser=="PU"){
                    cntPU++;
                }
                else if(server[i].typeOfUser=="RSU"){
                    cntRSU++;
                }
                else{
                    cntNRSU++;
                }
            }
        }
        int freeServers = channel - cntNRSU - cntPU - cntRSU;
        if(freeServers>0){
            arrivalCount[ceil(time)]++; //so whenever there is an arrival we are incrementing the count corr. to the ceil time 
        }
         
        if(userType=="PU"){
            stPU = PUSer(gen)*10; //generating the service time of PU
            if(freeServers>0){ //if there are free servers available
                for(int i=0; i<channel; i++){
                    if(server[i].busyTill==0 || server[i].busyTill<=time){
                        // totalServiceTime+=stPU;
                        server[i].busyTill = time + stPU;
                        server[i].typeOfUser = "PU";

                        //this condition counts the proper departure of PU
                        if(ceil(server[i].busyTill)<=simTime){
                            departureCount[ceil(server[i].busyTill)]++;
                        }
                        freeServers--;
                        cout<<"Server "<<(i+1)<<" is allocated to "<<server[i].typeOfUser<<" and it is busy till "<<server[i].busyTill<<endl;
                        break;
                    }
                }
            }
            else{ //there are no free servers
                if(cntNRSU>0){
                    for(int i=0; i<channel; i++){
                        if(server[i].typeOfUser=="NRSU"){
                            double remainingTime;
                            flag2 = false;
                            if(ceil(server[i].busyTill)<=simTime){
                                //means if there is space in handoff buffer then push the remainig time of transmission in queue
                                if(handoffBuffer.size()<handoffBufferSize){
                                    flag2 = true; //means the NRSU was not preempted rather it is putted inside handoff Buffer
                                    remainingTime = server[i].busyTill - time;
                                    handoffBuffer.push(remainingTime);
                                }
                                departureCount[ceil(server[i].busyTill)]--; //we are decrementing since this NRSU was not able to transmit and it was preempted in between so this should not be the part of departure
                            }
                            // totalServiceTime+=stPU;
                            server[i].busyTill = time + stPU;
                            server[i].typeOfUser = "PU";
                            if(ceil(server[i].busyTill)<=simTime){
                                departureCount[ceil(server[i].busyTill)]++;
                            }
                            if(flag2){
                                cout<<"Server "<<(i+1)<<" is allocated to "<<server[i].typeOfUser<<" and NRSU was put inside Handoff Buffer and it is busy till "<<server[i].busyTill<<endl;
                                cout<<"Remaining Time of NRSU to transmit is "<<remainingTime<<endl;
                            }
                            else{
                                preemptedUsers++;
                                cout<<"Server "<<(i+1)<<" is allocated to "<<server[i].typeOfUser<<" by preempting the NRSU and it is busy till "<<server[i].busyTill<<endl;
                            }
                            break;
                        }
                    }
                }
                else{ //this will be executed when there is no NRSU currently in service
                    if(cntRSU>0){
                        for(int i=0; i<channel; i++){
                            if(server[i].typeOfUser=="RSU"){
                                if(ceil(server[i].busyTill)<=simTime){
                                    departureCount[ceil(server[i].busyTill)]--; //we are decrementing since this RSU was not able to transmit and it was preempted in between so this should not be the part of departure
                                }
                                // totalServiceTime+=stPU;
                                server[i].busyTill = time + stPU;
                                server[i].typeOfUser = "PU";
                                preemptedUsers++;
                                if(ceil(server[i].busyTill)<=simTime){
                                    departureCount[ceil(server[i].busyTill)]++;
                                }
                                cout<<"Server "<<(i+1)<<" is allocated to "<<server[i].typeOfUser<<" by preempting the RSU and it is busy till "<<server[i].busyTill<<endl;
                                break;
                            }
                        }
                    }
                }
            }
        }
        else if(userType=="RSU"){
            stRSU = RSUSer(gen)*10; //generating the service time
            if(freeServers>0){ //if there are free servers available
                for(int i=0; i<channel; i++){
                    if(server[i].busyTill==0 || server[i].busyTill<=time){
                        // totalServiceTime+=stRSU;
                        server[i].busyTill = time + stRSU;
                        server[i].typeOfUser = "RSU";
                        if(ceil(server[i].busyTill)<=simTime){
                            departureCount[ceil(server[i].busyTill)]++;
                        }
                        freeServers--;
                        cout<<"Server "<<(i+1)<<" is allocated to "<<server[i].typeOfUser<<" and it is busy till "<<server[i].busyTill<<endl;
                        break;
                    }
                }
            }
            else{
                blockedUsers++;
                cout<<"RSU: No free servers available so I am blocked."<<endl;
            }
        }
        else{ //when userType is NRSU
            stNRSU = NRSUSer(gen)*10; //generating the service time
            if(freeServers>0){ //if there are free servers available
                for(int i=0; i<channel; i++){
                    if(server[i].busyTill==0 || server[i].busyTill<=time){
                        // totalServiceTime+=stNRSU;
                        server[i].busyTill = time + stNRSU;
                        server[i].typeOfUser = "NRSU";
                        if(ceil(server[i].busyTill)<=simTime){
                            departureCount[ceil(server[i].busyTill)]++;
                        }
                        freeServers--;
                        cout<<"Server "<<(i+1)<<" is allocated to "<<server[i].typeOfUser<<" and it is busy till "<<server[i].busyTill<<endl;
                        break;
                    }
                }
            }
            else{
                flag1 = false;
                if(newBuffer.size()<newBufferSize){
                    flag1 = true;
                    newBuffer.push(stNRSU); //stNRSU is pushed as the NRSU has not serviced yet
                }
                if(flag1){
                    cout<<"NRSU: No free servers are available but space is present inside queue so putted inside the new buffer."<<endl;
                }
                else{
                    blockedUsers++;
                    cout<<"NRSU: No free servers available so I am blocked."<<endl;
                }
            }
        }
        if(freeServers>0 && time<simTime){
            if(handoffBuffer.size()>0){
                double remTime = handoffBuffer.front();
                handoffBuffer.pop();
                for(int i=0; i<channel; i++){
                    if(server[i].busyTill==0 || server[i].busyTill<=time){
                        // totalServiceTime+=stRSU;
                        server[i].busyTill = time + remTime;
                        server[i].typeOfUser = "NRSU";
                        if(ceil(server[i].busyTill)<=simTime){
                            departureCount[ceil(server[i].busyTill)]++;
                        }
                        freeServers--;
                        cout<<"Server "<<(i+1)<<" is allocated to "<<server[i].typeOfUser<<" which was in Handoff Buffer and it is busy till "<<server[i].busyTill<<endl;
                        break;
                    }
                }
            }
            else if(newBuffer.size()>0){
                double remTime = newBuffer.front();
                newBuffer.pop();
                for(int i=0; i<channel; i++){
                    if(server[i].busyTill==0 || server[i].busyTill<=time){
                        // totalServiceTime+=stRSU;
                        server[i].busyTill = time + remTime;
                        server[i].typeOfUser = "NRSU";
                        if(ceil(server[i].busyTill)<=simTime){
                            departureCount[ceil(server[i].busyTill)]++;
                        }
                        freeServers--;
                        cout<<"Server "<<(i+1)<<" is allocated to "<<server[i].typeOfUser<<" which was in New Buffer and it is busy till "<<server[i].busyTill<<" when size of Handoff Buffer is "<<handoffBuffer.size()<<endl;
                        break;
                    }
                }

            }
        }
    }
    // int totalArrivalCount=0;       
    cout<<"Simulation Ended. Enjoy! :)"<<endl;
    cout<<"Preempted Users: "<<preemptedUsers<<endl;
    cout<<"BLocked Users: "<<blockedUsers<<endl;
    cout<<"Time of Arrival"<<"  "<<"Count"<<endl;
    for (auto ele:arrivalCount){
        cout<<ele.first<<"    "<<ele.second<<endl;
        // totalArrivalCount+=ele.second;
    }

    cout<<"Time of Departure"<<"  "<<"Count"<<endl;
    for (auto ele:departureCount){
        cout<<ele.first<<"    "<<ele.second<<endl;
    }
    // cout<<"Total Arrival :"<<totalArrivalCount<<"  "<<"Total Serice Time :"<<totalServiceTime<<endl;
    return 0;
}
