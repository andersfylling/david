#pragma once

// only holds information about the engine or const that can be used.
namespace david {
// Folder to ANN networks files from project root dir.
static const std::string neuralNetworksFolder = "/engine/src/ANN/networks/";

// a standard chess board layout given as a FEN string.
static const std::string FENStartPosition = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

// ANN file
static const std::string ANNFile = "float_ANNFile_6_83_1_1497360313.net";

// information about the engine.
namespace engineInformation {
static const std::string PROJECT_NAME = "david"; // MUST BE THE SAME AS FOLDER NAME, WILL BE REMOVED BEFORE FIRST RELEASE!!
static const std::string ENGINE_NAME = "david";
static const std::string ENGINE_AUTHOR = "Anders Øen Fylling, Martin Klingenberg, Markus Jacobsen, Martin Trehjøring";
static const std::string ENGINE_VERSION = "0.2.3"; // THIS IS JUST SOME BS NUMBER.
static const std::string ENGINE_COPYRIGHT = ENGINE_NAME + ". Copyright (C) 2017-2017 " + ENGINE_AUTHOR + ". ";
}
}
