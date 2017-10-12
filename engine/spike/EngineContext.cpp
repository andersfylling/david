#include "EngineContext.h"

/**
 * Constructor
 */

david::EngineContext::EngineContext()
    : NeuralNet()
    , TreeGen(NeuralNet)
    , Search(TreeGen)
    , UCI()
{}

david::EngineContext::EngineContext(const std::string ANNFileName)
    : NeuralNet(ANNFileName)
    , TreeGen(NeuralNet)
    , Search(TreeGen)
    , UCI()
{}

/**
 * Destructor
 */
david::EngineContext::~EngineContext()
{}
