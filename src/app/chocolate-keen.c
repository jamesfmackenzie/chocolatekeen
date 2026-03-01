#include "core/globals.h"
#include "episodes/episode1.h"
#include "episodes/episode2.h"
#include "episodes/episode3.h"

int main(int argc, char **argv) {
    EngineArguments_T arguments;
    if (!CVort_engine_processArguments(argc, argv))
        return 0;
    /* NOTE: Relevant to an earlier C++ variant but is still worthy to mention.
     * An instance of the CVorticons class (i.e. the game) is a quite large
     * object to initialize, so having it on the stack is the path for
     * RANDOM PROBLEMS!!! (Memory related errors...)
     * Furthermore, we wish to apply polymorphism.
     *
     * Not so relevant with the migration to C code, though.
     */
    if (!CVort_engine_start()) {
        return 1;
    }
    CVort_engine_shutdown();
}
