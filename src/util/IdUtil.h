#pragma once
#include <string>
#include <iostream>

int NextID() {
    static int counter = 1;
    return counter++;
}

void NormalizePath(std::string path)
{
    for (char& c : path)
    {
        if (c == '/')
            c = '\\';
    }
}
