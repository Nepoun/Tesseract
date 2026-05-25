#pragma once

// Static = A variavel só existe na função e é consistente entre chamadas
int NextID() {
    static int counter = 1;
    return counter++;
}