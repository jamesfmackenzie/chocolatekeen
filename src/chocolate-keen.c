#include "cvorticons.h"
#include "cvorticons1.h"
#include "cvorticons2.h"
#include "cvorticons3.h"

int main(int argc, char **argv) {
    chdir("/apps/chocolate-keen");
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
#if 0
int main(int argc, char **argv) {
    EngineArguments_T arguments;

    if (!CVort_engine_processArguments(argc, argv, &arguments))
        return 0;
    uint8_t *exeBuffer;
#ifdef CHOCOLATE_KEEN_CONFIG_SPECIFIC_EPISODE
    gameversion_T gameVersion = GAMEVER_KEEN_SPECIFIC;
    if (!CVort_engine_loadEXE(&gameVersion, &exeBuffer)) {
#else
    if (!CVort_engine_loadEXE(&arguments.gameVersion, &exeBuffer)) {
#endif
        return 1;
    }


    /* NOTE: Relevant to earlier C++ variant but still worths to mention.
     * An instance of CVorticons (i.e. the game) is a quite large object to
     * initialize, so having it on the stack is the path for RANDOM PROBLEMS!!!
     * (Memory related errors...)
     * Furthermore, we wish to apply polymorphism.
     */
#ifdef CHOCOLATE_KEEN_CONFIG_SPECIFIC_EPISODE
    CVort_Init(gameVersion);
#else
    CVort_Init(arguments.gameVersion);
#endif
    if (!CVort_engine_init(&arguments, exeBuffer))
        return 0;
    CVort_gui_runLoop();
    CVort_engine_shutdown();
    CVort_Shutdown();
    CVort_engine_freeEXE(exeBuffer);
    return 0;
}
#endif
