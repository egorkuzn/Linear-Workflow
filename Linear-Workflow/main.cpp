#include <iostream>
#include "workflow.h"

using namespace wkfw;

int main(){
    const std::string i = "in.txt";
    const std::string o = "out.txt";
    Workflow wflow(i, o);
    wflow.run();
    if(wflow)
        std::cout<< "Workes" << std::endl;    
    else
        std::cout << "No workes" << std::endl;    
    return 0;
}