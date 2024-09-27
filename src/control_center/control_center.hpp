#define SECONDS_PER_MOVE 2.4 // 2.4 seconds to move to a nearby block
#define DIMENSION 300 // The area is 300x300 blocks

#include "../con2db/pgsql.h"
#include "../con2db/pgsql.cpp"
#include <string>
#include <mutex>

// Definition of control center class
class Control_Center {
    
    public:
        // Constructor(id, dimension in blocks)
        Control_Center(): dimension(DIMENSION), conn("postgres", "5432", "postgres", "postgres", "postgres"){
            // The matrix is initialized with -1 for each block, which means 'empty'
            for (int i = 0; i < DIMENSION; i++) {
                for (int j = 0; j < DIMENSION; j++) {
                    grid[i][j] = -1;
                }
            }
        }

        // It places the drone on the grid
        void place_drone(int drone_id, int x, int y) {
            grid[x][y] = drone_id;
        }

        // It removes the drone frome the grid
        void remove_drone(int x, int y) {
            grid[x][y] = -1;
        }

        // It allows to send a query that doesn't return any data
        void executeQuery(const std::string &query) {
            std::lock_guard<std::mutex> lock(query_mutex);
            conn.ExecSQLcmd(const_cast<char *>(query.c_str()));
        }

        // It allows to send a query that returns some data
        PGresult* getTuples(const std::string &query) {
            std::lock_guard<std::mutex> lock(query_mutex);
            PGresult *res = conn.ExecSQLtuples(const_cast<char *>(query.c_str()));
            return res;
        }

    private:
        int dimension; // dimensionxdimension equals to the area
        int grid[DIMENSION][DIMENSION];
        Con2DB conn;
        std::mutex query_mutex;
};


// Control center management functions
