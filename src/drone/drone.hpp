#define COVERAGE 1 // 1 block is a square 20x20 meters
#define MAX_FLIGHT_MOVES 750 // Max moves for flight
#define MIN_RECHARGE_TIME 120 // Min minutes for recharging
#define MAX_RECHARGE_TIME 180 // Max minutes for recharging
#define DIMENSION 300 // The area is 300x300 blocks
#define TIME 86400 //10800 3h, 86400 24h. Seconds before stopping

#include <stdio.h>
#include <random>
#include <mutex>

// Definition of drone class
class Drone {

    public:
        // Constructor ()
        Drone() : id(0), x(150), y(150), moves_left(MAX_FLIGHT_MOVES){
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distribution(7200, 10800); //7200, 10800
            recharging_time = distribution(gen);
            std::lock_guard<std::mutex> guard(count_mutex);
            id = instance_count;
            instance_count++;
        }

        // Static method to get the number of instances of the class
        static int getInstanceCount(){
            return instance_count;
        }

        // It executes a move
        void ExecuteMove(char move){
            if(move == 'l'){
                this->moveLeft();
            }else if (move == 'u') {
                this->moveUp();
            }else if(move == 'r'){
                this->moveRight();
            }else if(move == 'd'){
                this->moveDown();
            }else{
                printf("The char %c in the path is undefined...", move);
            }
        } 

        // Method to move left
        void moveLeft(){
            if(x > 0){
                x = x-1;
                moves_left = moves_left-1;
            }else{
                printf("The drone is trying to escape...");
            }
        }

        // Method to move up
        void moveUp(){
            if(y > 0){
                y = y-1;
                moves_left = moves_left-1;
            }else{
                printf("The drone is trying to escape...");
            }
        }

        // Method to move right
        void moveRight(){
            if(x < DIMENSION-1){
                x = x+1;
                moves_left = moves_left-1;
            }else{
                printf("The drone is trying to escape...");
            }
        }

        // Method to move down
        void moveDown(){
            if(y < DIMENSION-1){
                y = y+1;
                moves_left = moves_left-1;
            }else{
                printf("The drone is trying to escape...");
            }
        }

        // Sets moves_left to maximum
        void fillMovesLeft(){
            moves_left = MAX_FLIGHT_MOVES;
        }

        // Gets recharging time from a drone
        int getRecharginTime(){
            return this->recharging_time;
        }

        // Gets the moves left from a drone
        int getMovesLeft(){
            return this->moves_left;
        }

        // Gets the X coordinate from a drone
        int getX(){
            return this->x;
        }

        // Gets the Y coordinate from a drone
        int getY(){
            return this->y;
        }

        // Gets the Id from a drone
        int getId(){
            return this->id;
        }

    // Vector of ready drones
    static std::vector<Drone> readyOnes;
    // Mutex to manage readyOnes
    static std::mutex ready_mutex;

    private:
        int id; // Unique id
        int x; // Longitude
        int y; // Latitude
        int moves_left; // Remaining moves
        int recharging_time; // Seconds for a complete recharge
        // Global counter of drones
        static int instance_count;
        // Mutex to manage instance_count
        static std::mutex count_mutex;
};
