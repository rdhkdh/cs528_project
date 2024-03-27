#include<bits/stdc++.h>
#include<unistd.h>
using namespace std;

const int K = 5; //relative deadline
const int U = 5; //no of users
const int M = 5; //no of FOG nodes
const double BR = 10; //brand constant
const double f = 0.3; //failure rate
const int TOTAL_TIME = 20; //total duration of execution
int GLOBAL_CLOCK = 0;
int idle_time=0;

vector<vector<double>> DT(U, vector<double>(M, 0.5));
vector<double> ST(M, 0.5);

struct User {
    int id;
};

struct Task {
    int arrival;
    int execution_time;
    int deadline;
    double failure_probability;
    User* owner;
};

struct Node{
    int id;
    bool busy;
    int finish_time;
    Task* assigned_task;
};

std::queue <Task> Central_queue; //central queue for task requests

void update_ST(int Machine_index) {
    double sum = 0;
    for (int i = 0; i < U; i++) {
        sum += DT[i][Machine_index];
    }
    ST[Machine_index] = sum/double(U);
}

//lower the ratio, more optimum is the node
double calculate_cost_ratio(int m)
{
    double Base = 0.3*BR;
    double C = Base + (BR*ST[m]*ST[m]); //cost of execution
    double cost_ratio = C/ST[m];
    return cost_ratio;
}

//sorts Tasks in decreasing order of failure probability
bool compareByFailureProbability(const Task& a, const Task& b) {
    return a.failure_probability > b.failure_probability;
}

int getRandomInt(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(gen);
}

void generate_random_tasks(vector<User>& users) {
    default_random_engine generator(time(nullptr)); 
    poisson_distribution<int> arrival_distribution(3.0);
    poisson_distribution<int> execution_distribution(2.0);

    int current_time = 0;

    while (current_time < TOTAL_TIME) 
    {
        int freq = getRandomInt(1,7); //no of tasks having same arrival time
        int arrival_time = current_time + arrival_distribution(generator);

        // Generate 'freq' no of new tasks
        for(int i=1;i<=freq;i++)
        {
            Task new_task;
        
            int execution_time = execution_distribution(generator);
            new_task.arrival = arrival_time;
            new_task.execution_time = execution_time;
            new_task.deadline = arrival_time + K;
            new_task.failure_probability = exp(-f*double(new_task.execution_time)); //exp(-ft)

            // Assign the task to a random user
            int index = rand() % users.size();
            User& u = users[index];
            new_task.owner = &u;

            //push the created task into central queue
            Central_queue.push(new_task);
        }

        // Update current time
        current_time = arrival_time;
    }
}

void assign_nodes(vector<Node>& nodes)
{
    Task curr;
    int a=0; //arrival time of head of queue
    vector<Task> curr_tasks, pending_tasks, new_tasks;
    curr_tasks.clear(); pending_tasks.clear();

    while(GLOBAL_CLOCK<2*TOTAL_TIME)
    {
        if(!Central_queue.empty())
        {
            //update free/busy status of all nodes
            for(int i=0;i<M;i++)
            {
                if(GLOBAL_CLOCK >= nodes[i].finish_time)
                { nodes[i].busy = false; }
            }

            //put all pending tasks at front 
            curr_tasks = pending_tasks; 
            pending_tasks.clear();

            //pop from queue, all tasks of a particular arrival time (if reached)
            curr = Central_queue.front();
            a = curr.arrival;
            if(a<=GLOBAL_CLOCK) //if task has actually arrived
            {
                while(curr.arrival==a)
                {
                    Central_queue.pop(); //pop out current task
                    new_tasks.push_back(curr); //store in vector of new tasks
                    curr = Central_queue.front(); //next task in queue
                }
            }
            else{
                GLOBAL_CLOCK++;
                continue;
            }

            //order the tasks by shortest processing time, i.e, highest FP
            sort(new_tasks.begin(), new_tasks.end(), compareByFailureProbability);

            //curr_tasks = pending_tasks + new_tasks
            for(auto u: new_tasks) { curr_tasks.push_back(u); }
            new_tasks.clear();

            //iterate over all nodes to calculate cost ratio of free ones
            vector<pair<double,int>> cost_ratios; //cost_ratio, node index
            for(int i=0;i<M;i++)
            {
                if(!nodes[i].busy) //if not busy, node can be allocated
                {
                    double cr = calculate_cost_ratio(i);
                    cost_ratios.push_back(make_pair(cr,i));
                }
            }
            sort(cost_ratios.begin(),cost_ratios.end()); //sort in incr order of cost_ratio

            if(curr_tasks.size()>cost_ratios.size()) 
            { //if no of current tasks is more than no of free nodes
                int n= cost_ratios.size(); //no of free nodes
                for(int i=n; i<curr_tasks.size(); i++)
                {
                    pending_tasks.push_back(curr_tasks[i]);
                }
                //remove the tasks that cannot be assigned currently
                curr_tasks.erase(curr_tasks.begin()+n, curr_tasks.end()); 
            }

            //assign task with highest FP to node with lowest cost_ratio
            for(int j=0;j<curr_tasks.size();j++)
            {
                int node_id = cost_ratios[j].second;
                nodes[node_id].assigned_task = &curr_tasks[j]; //match task to node
                nodes[node_id].finish_time = GLOBAL_CLOCK + curr_tasks[j].execution_time; //set finish time of node
                nodes[node_id].busy = true; //mark node as busy
                
                cout<<"Task: ";
                cout<<"Arrival="<<curr_tasks[j].arrival<<" ";
                cout<<"Execution="<<curr_tasks[j].execution_time<<" ";
                cout<<"Deadline="<<curr_tasks[j].deadline<<" ";
                cout<<"FP="<<fixed<<setprecision(2)<<curr_tasks[j].failure_probability<<" "; //round to 2 decimal places
                cout<<"User="<<curr_tasks[j].owner->id<<"\n";

                cout<<"Assigned to: ";
                cout<<"Node="<<node_id<<" ";
                cout<<"Shared Trust="<<ST[node_id]<<" ";
                cout<<"Finish Time="<<nodes[node_id].finish_time<<"\n";
                cout<<endl;
            }
            curr_tasks.clear();
        }
        else
        {
            idle_time++;
        }



        GLOBAL_CLOCK++;
    }
}

int main() 
{
    srand(time(nullptr)); // Seed the random number generator

    //create and intialize users vector
    vector<User> users(U);
    for (int i = 0; i < U; ++i) { users[i].id = i; }

    //create and intialize nodes vector
    vector<Node> nodes(M);
    for(int i=0;i<M;i++)
    {
        nodes[i].id = i;
        nodes[i].busy = false;
        nodes[i].finish_time = 0;
    }

    generate_random_tasks(users);

    // // Print the central queue
    // cout << "\nCentral Queue:" << endl;
    // while (!Central_queue.empty()) {
    //     Task task = Central_queue.front();
    //     cout << "Task: Arrival=" << task.arrival
    //          << ", Execution Time=" << task.execution_time << ", Deadline=" << task.deadline<< ",User" <<task.owner->id <<  endl;
    //     Central_queue.pop();
    // }
   
    assign_nodes(nodes);

    cout<<"------------------FoG System Report--------------------\n";
    cout<<"Idle time= "<<idle_time<<"s"<<endl;

    return 0;
}
