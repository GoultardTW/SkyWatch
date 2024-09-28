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

        res.append(temp, !direction ? 'r' : 'l');
        ax += (!direction ? temp : -temp);
        step -= temp;
        if (step > 0) {
            if (ay < 299) {
                res += 'd'; 
                step -= 1; 
                ay += 1; 
            } else {
                break;
            }
        }
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
