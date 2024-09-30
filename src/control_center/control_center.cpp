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
std::string goToGrid(int* x, int* y, int step){
    std::string res = "";
    int ax = *x;
    int ay = *y;
    int temp = 0;
    while (step > 0){
        
        int direction = ay % 2;
        temp = (!direction) ? (299 - ax) : ax;
        temp = std::min(temp, step);

        for (int k=0; k<temp; k++){
            if(abs(ax-150)+abs(ay-150)==step){
                res = res + goTo(ax, ay, 150, 150);
                step = 0;
                break;
            }
            res = res + (!direction ? "r" : "l");
            ax += (!direction ? 1 : -1);
            step -= 1;
        }

        if (step > 0) {
            if (ay < 299) {
                if(abs(ax-150)+abs(ay-150)==step){
                    res = res + goTo(ax, ay, 150, 150);
                    step = 0;
                    break;
                }
                res += 'd'; 
                step -= 1; 
                ay += 1; 
            } else {
                break;
            }
        }
    }
    if(ax < 299 && (ay%2)==0){
        ax++;
    }else if (ax > 0 && (ay%2)==1){
        ax--;
    }else if (((ax==299) && (ay%2)==0 && (ay<299)) || ((ax==0) && (ay%2)==1 && (ay<299))){
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
        path = path + goToGrid(&x, &y, step);
        paths->push_back(path);
        // incrementare di 1 x per il path successivo.
    }

    if (x > 0) {
        path = goTo(150, 150, x, y);
        step = x;
        path = goToGrid(&x, &y, step);
        paths->push_back(path);
    }
}
