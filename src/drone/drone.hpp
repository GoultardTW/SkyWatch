#define COVERAGE 1 // 1 block is a square 20x20 meters
#define MAX_FLIGHT_MOVES 750 // Max moves for flight
#define MIN_RECHARGE_TIME 120 // Min minutes for recharging
#define MAX_RECHARGE_TIME 180 // Max minutes for recharging
#define DIMENSION 300 // The area is 300x300 blocks

#include <stdio.h>

// Definition of drone class
class Drone {

    public:
        // Constructor (id)
        Drone(int id) : id(id), x(150), y(150), moves_left(MAX_FLIGHT_MOVES){
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
                printf("A move in the path is undefined...");
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

        int getMovesLeft(){
            return this->moves_left;
        }

        int getX(){
            return this->x;
        }

        int getY(){
            return this->y;
        }

    private:
        int id; // Unique id
        int x; // Longitude
        int y; // Latitude
        int moves_left; // Seconds of life remaining
        int recharging_time; // Minutes for a recharge
};
