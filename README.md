# Cognitive Radio Network Simulation
#### WithoutQueue.cpp => 
This file contains simulation code in which we have not used the queue for NRSU users.
#### WithQueue.cpp => 
This file contains simulation code in which we have used the 2 types of queue for NRSU users i.e. handoff buffer and new buffer. Handoff Buffer contains those NRSU users which was not able to transmit completely on the channel and was preempted in between by PU. New Buffer contains those NRSU users which when arrived then there was no server empty and was put inside that.
#### Without Queue Varying Lambda.cpp => 
This file contains simulation code in which we have not used the queue for NRSU users. And for finding the graphs of number of blocked/preempted users at different lambda values I used this file. Basically checking the pattern of blocked/preempted users i.e. whether they are increasing or decreasing.
#### With Queue Varying Lambda.cpp => 
This file contains simulation code in which we have used the queue for NRSU users. And for finding the graphs of number of blocked/preempted users at different lambda values I used this file. Basically checking the pattern of blocked/preempted users i.e. whether they are increasing or decreasing.
