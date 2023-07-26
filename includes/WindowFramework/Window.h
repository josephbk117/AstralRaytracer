#pragma once

#include <string>

class Window
{
    public:
    Window():m_name{"default"}{}
    Window(const std::string name):m_name{name}{}
    std::string getName();

    private:
    std::string m_name;
};