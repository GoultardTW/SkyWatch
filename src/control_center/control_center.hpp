#define SECONDS_PER_MOVE 2.4 // 2.4 seconds to move to a nearby block
#define DIMENSION 300 // The area is 300x300 blocks
#define TIME 86400 //10800 3h, 86400 24h. Seconds before stopping

#include <postgresql/libpq-fe.h>
#include <string>
#include <mutex>
#include <vector>
#include <chrono>

#include "../con2db/pgsql.h"
#include "../con2db/pgsql.cpp"

// Definition of control center class
class Control_Center {
    
    public:
        // Constructor()
        Control_Center(): dimension(DIMENSION), conn("postgres", "5432", "postgres", "postgres", "postgres"){
            // The matrix is initialized with now() for each block, which means 'not visited'
            std::chrono::time_point<std::chrono::system_clock> t = std::chrono::time_point<std::chrono::system_clock>();
            for (int i = 0; i < DIMENSION; i++) {
                grid.emplace_back(std::vector<std::chrono::time_point<std::chrono::system_clock>>());
                for (int j = 0; j < DIMENSION; j++) {
                    grid[i].emplace_back(t);
                }
            }
            // Gets ids of CC and Session from DB
            ccId = std::stoi(getValueQuery(std::string("INSERT INTO controlCenter DEFAULT VALUES RETURNING id;"), 0, 0));
            sessionId = std::stoi(getValueQuery(std::string("INSERT INTO session_ (id_cdc, start_) VALUES ("+std::to_string(ccId)+", NOW()) RETURNING id;"), 0, 0));
        }

        // It updates the clock of the point (x,y)
        void updateGrid(int x, int y){
            grid[x][y] = std::chrono::system_clock::now();
        }

        // It returns the latest visit of the point (x,y)
        std::chrono::time_point<std::chrono::system_clock> getTimeFromGrid(int x, int y){
            return grid[x][y];
        }
        
        // It allows to send a query that doesn't return any data
        void executeQuery(const std::string &query) {
            std::lock_guard<std::mutex> guard(query_mutex);
            conn.ExecSQLcmd(const_cast<char *>(query.c_str()));
        }

        // It returns the value of the query
        std::string getValueQuery(const std::string &query, int row, int column){
            PGresult* tuples = getTuples(query);
            char* value = PQgetvalue(tuples, row, column);
            return std::string(value);
        }

        // It allows to send a query that returns some data
        PGresult* getTuples(const std::string &query) {
            std::lock_guard<std::mutex> guard(query_mutex);
            PGresult *res = conn.ExecSQLtuples(const_cast<char *>(query.c_str()));
            return res;
        }

        // Returns the Id of the Session
        int getSessionId(){
            return sessionId;
        }

        // Returns the Id of the Control Center
        int getCCId(){
            return ccId;
        }

        float computePercentage(){
            int c = 0;
            std::chrono::time_point<std::chrono::system_clock> t = std::chrono::system_clock::now();
            for(int i=0; i<dimension; i++){
                for(int j=0; j<dimension; j++){
                    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(t-getTimeFromGrid(i, j));
                    if(diff.count()<=302000){
                        c++;
                    }
                }
            }
            float p = (float)c/(dimension*dimension);
            return p*100;
        }

    private:
        int dimension; // dimensionxdimension equals to the area
        int sessionId; // Id of the Session from DB
        int ccId; // Id of the Control Center from DB
        std::vector<std::vector<std::chrono::time_point<std::chrono::system_clock>>> grid; // Matrix containing timestamps for each point of the grid
        Con2DB conn;
        std::mutex query_mutex; // Mutex to avoid query at the same time
};
