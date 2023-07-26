#include <iostream>
#include "../includes/WindowFramework/Window.h"
#include "../includes/WindowFramework/WindowUtils.h"

int main()
{
    std::cout<<"Hello World!";
    Window window("new window test");

    std::cout<<"\n window name :"<<window.getName()<<"\nMax window count "<<WindowUtils::GetMaxWindowsAllowed();
    return 0;
}