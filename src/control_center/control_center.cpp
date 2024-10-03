#include <cstdlib>
#include <stdlib.h>
#include <vector>
#include <string>


// Returns a path from (x, y) -> (nx, ny)
std::string goTo(int x, int y, int nx, int ny){
    std::string res = "";
    if (x > nx) {
        res = res + std::string((x-nx), 'l');
    }else if ( x < nx) {
        res = res + std::string((nx-x), 'r');
    }
    if (y > ny) {
        res = res + std::string((y-ny), 'u');
    }else if ( y < ny) {
        res = res + std::string((ny-y), 'd');
    }
    return res;
}

// Returns a path that passes through the rows
std::string goToGridX(int* x, int* y, int step){
    std::string res = "";
    int ax = *x, ay = *y;
    int temp = 0;
    bool goRight = false;

    while(step>0){
        goRight = (ay%2==0); // If it's even go to the right
        temp = (goRight) ? (299 - ax) : ax; // Steps to reach the border
        temp = std::min(temp, step); // Min between steps left and temp

        for (int k=0; k<temp; k++){
            if(step <= abs(ax-150)+abs(ay-150)){
                res = res + goTo(ax, ay, 150, 150);
                step = 0;
                break;
            }
            if(goRight){
                res = res + "r";
                ax ++;
            }else{
                res = res + "l";
                ax --;
            }
            step -= 1;
        }

        if(step>0 && ay<299){
            if(step <= abs(ax-150)+abs(ay-150)){
                res = res + goTo(ax, ay, 150, 150);
                step = 0;
                break;
            }
            res += "d"; 
            step -= 1; 
            ay += 1; 
        }
    }

    goRight = (ay%2==0);
    if(ax < 299 && goRight){
        ax++;
    }else if (ax > 0 && !goRight){
        ax--;
    }else if (((ax>=299) && goRight && (ay<299)) || ((ax<=0) && !goRight && (ay<299))){
        ay++;
    }

    *x = ax;
    *y = ay;
    return res;
}

// It determines a vector of paths
void findPaths(std::vector<std::string>* paths) {
    int x = 0, y = 0;
    int step;
    std::string path = "";

    while (y < 299) {
        step = 750;
        path = goTo(150, 150, x, y);
        step -= path.length();
        path = path + goToGridX(&x, &y, step);
        paths->emplace_back(path);
    }

    if (x > 0) {
        path = goTo(150, 150, x, y);
        step = x;
        path = path + goToGridX(&x, &y, step);
        paths->emplace_back(path);
    }
}
