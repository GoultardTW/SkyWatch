#define COVERAGE 1 // 1 block is a square 20x20 meters
#define MAX_FLIGHT_MOVES 750 // Max moves for flight
#define MIN_RECHARGE_TIME 120 // Min minutes for recharging
#define MAX_RECHARGE_TIME 180 // Max minutes for recharging
#define DIMENSION 300 // The area is 300x300 blocks

// Definition of drone class
class Drone {

    public:
        // Constructor (id)
        Drone(int id) : id(id), x(150), y(150), moves_left(MAX_FLIGHT_MOVES){
        }

        // Method to move left
        void moveLeft(){
            if(x > 0){
                x = x-1;
                moves_left = moves_left-1;
            }
        }

        // Method to move up
        void moveUp(){
            if(y > 0){
                y = y-1;
                moves_left = moves_left-1;
            }
        }

        // Method to move right
        void moveRight(){
            if(x < DIMENSION-1){
                x = x+1;
                moves_left = moves_left-1;
            }
        }

        // Method to move down
        void moveDown(){
            if(y < DIMENSION-1){
                y = y+1;
                moves_left = moves_left-1;
            }
        }

    private:
        int id; // Unique id
        float x; // Longitude
        float y; // Latitude
        int moves_left; // Seconds of life remaining
        int recharging_time; // Minutes for a recharge
};
