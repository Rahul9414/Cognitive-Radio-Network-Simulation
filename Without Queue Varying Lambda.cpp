#include<bits/stdc++.h>
#define channel 10
#define lambda1 0.1 //20
#define lambda2 0.2 //10  //corresponding to RSU
#define lambda3 0.3 //15 //corresponding to NRSU
#define mu1 2
#define mu2 4
#define mu3 4
#define iterations 8
#define maxLambda 1
#define increment 0.01
using namespace std;

class Channels{
    public:
    string typeOfUser;
    double busyTill;
};

int main(){
    int simTm;
    cin>>simTm;

    int cnt = (maxLambda - lambda2)/increment;
    vector<pair<double, int>> blocked; //so this will store the lambda, cnt of blocked users
    vector<pair<double, int>> preempted; //so this will store the lambda, cnt of preempted users
    //changing lambda2 by using starting value as 0.2 and increment as 0.05 and max value as 0.05 and corr to each lambda the no. of tertions are 4
    for(int i=0; i<cnt; i++){
        double l2 = lambda2 + (i*increment); //now this statement ensures that the l3 is varying
        int totalBlockedUsers =0;
        int totalPreemptedUsers =0;
        for(int j=0; j<iterations; j++){
            int simTime = simTm;
            vector<Channels> server(channel, {"", 0.0}); //so a vector of channels is created
            random_device rd;
            mt19937 gen(rd());
            exponential_distribution<>PUArr(lambda1);
            exponential_distribution<>RSUArr(l2);
            exponential_distribution<>NRSUArr(lambda3);
            exponential_distribution<>PUSer(mu1);
            exponential_distribution<>RSUSer(mu2);
            exponential_distribution<>NRSUSer(mu3);

            // minheap of pairs
            priority_queue<pair<double, string>, vector<pair<double, string>>, greater<pair<double, string>>> pq;
            double time =0.0;
            double iatPU, iatRSU, iatNRSU, natPU, natRSU, natNRSU, stPU, stRSU, stNRSU;
            // cout<<"Simulation Started"<<endl;
            map<double, int> arrivalCount; //this will keep track of the number of users arrived at any point of time 
            map<double, int> departureCount; //this will keep track of the number of users departed at any point of time 
            int blockedUsers =0; //this will keep track of count of blocked users
            int preemptedUsers =0; //this will keep track of count of preempted users

            while(time<simTime){
                iatPU = PUArr(gen)*10.0;
                iatRSU = RSUArr(gen)*10.0;
                iatNRSU = NRSUArr(gen)*10.0;
                natNRSU = time + iatNRSU;
                natPU = time + iatPU;
                natRSU = time + iatRSU;
                pq.push({natNRSU, "NRSU"});
                pq.push({natPU, "PU"});
                pq.push({natRSU, "RSU"});

                auto nextArrival = pq.top();
                pq.pop();
                time = nextArrival.first;
                // cout<<"Time of next arrival "<<time<<endl;
                string userType = nextArrival.second;

                //this loop will check that how many PUs, RSUs, NRSUs are in service. Thiis also eliminates the possibility of considering departure as event
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
                    arrivalCount[ceil(time)]++; //so whenever there is an arrival we are incrementing the count corr to the ceil time 
                }
                
                if(userType=="PU"){
                    stPU = PUSer(gen)*10; //generating the service time
                    if(freeServers>0){ //if there are free servers available
                        for(int i=0; i<channel; i++){
                            if(server[i].busyTill==0 || server[i].busyTill<=time){
                                server[i].busyTill = time + stPU;
                                server[i].typeOfUser = "PU";
                                if(ceil(server[i].busyTill)<=simTime){
                                    departureCount[ceil(server[i].busyTill)]++;
                                }
                                // cout<<"Server "<<(i+1)<<" is allocated to "<<userType<<" and it is busy till "<<server[i].busyTill<<endl;
                                break;
                            }
                        }
                    }
                    else{ //there are no free servers
                        if(cntNRSU>0){
                            for(int i=0; i<channel; i++){
                                if(server[i].typeOfUser=="NRSU"){
                                    if(ceil(server[i].busyTill)<=simTime){
                                        departureCount[ceil(server[i].busyTill)]--; //we are decrementing since this NRSU was not able to transmit and it was preempted in between so this should not be the part of departure
                                    }
                                    server[i].busyTill = time + stPU;
                                    server[i].typeOfUser = "PU";
                                    preemptedUsers++;
                                    if(ceil(server[i].busyTill)<=simTime){
                                        departureCount[ceil(server[i].busyTill)]++;
                                    }
                                    // cout<<"Server "<<(i+1)<<" is allocated to "<<userType<<" by preempting the NRSU and it is busy till "<<server[i].busyTill<<endl;
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
                                        server[i].busyTill = time + stPU;
                                        server[i].typeOfUser = "PU";
                                        preemptedUsers++;
                                        if(ceil(server[i].busyTill)<=simTime){
                                            departureCount[ceil(server[i].busyTill)]++;
                                        }
                                        // cout<<"Server "<<(i+1)<<" is allocated to "<<userType<<" by preempting the RSU and it is busy till "<<server[i].busyTill<<endl;
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
                                server[i].busyTill = time + stRSU;
                                server[i].typeOfUser = "RSU";
                                if(ceil(server[i].busyTill)<=simTime){
                                    departureCount[ceil(server[i].busyTill)]++;
                                }
                                // cout<<"Server "<<(i+1)<<" is allocated to "<<userType<<" and it is busy till "<<server[i].busyTill<<endl;
                                break;
                            }
                        }
                    }
                    else{
                        blockedUsers++;
                        // cout<<"RSU: No free servers available so I am blocked."<<endl;
                    }
                }
                else{ //when userType is NRSU
                    stNRSU = NRSUSer(gen)*10; //generating the service time
                    if(freeServers>0){ //if there are free servers available
                        for(int i=0; i<channel; i++){
                            if(server[i].busyTill==0 || server[i].busyTill<=time){
                                server[i].busyTill = time + stNRSU;
                                server[i].typeOfUser = "NRSU";
                                if(ceil(server[i].busyTill)<=simTime){
                                    departureCount[ceil(server[i].busyTill)]++;
                                }
                                // cout<<"Server "<<(i+1)<<" is allocated to "<<userType<<" and it is busy till "<<server[i].busyTill<<endl;
                                break;
                            }
                        }
                    }
                    else{
                        blockedUsers++;
                        // cout<<"NRSU: No free servers available so I am blocked."<<endl;
                    }
                }
            }
            totalBlockedUsers += blockedUsers;
            totalPreemptedUsers += preemptedUsers;
        }
        // blocked.push_back({l3, totalBlockedUsers/iterations});
        // preempted.push_back({l3, totalPreemptedUsers/iterations});
        blocked.push_back({l2, totalBlockedUsers/iterations});
        preempted.push_back({l2, totalPreemptedUsers/iterations});
    }
    cout<<"Lambda"<<"   "<<"Blocked Users"<<endl;
    for(auto ele: blocked){
        cout<<ele.first<<"  "<<ele.second<<endl;
    }

    // cout<<"Lambda"<<"   "<<"Preempted Users"<<endl;
    // for(auto ele: preempted){
    //     cout<<ele.first<<"  "<<ele.second<<endl;
    // }

    return 0;
}
