#pragma once
#include <string>

// Static = A variavel só existe na função e é consistente entre chamadas
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
