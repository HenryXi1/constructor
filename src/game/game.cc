#include "game.h"
#include "builder.h"
#include "../structures/residence.h"
#include "../board/edge.h"
#include "../board/vertex.h"
#include "../board/board.h"
#include "../structures/road.h"
#include "../structures/residence.h"
#include <vector>
#include <string>
#include <fstream>

Game::Game(unsigned seed) : currentBuilder{0} {
    board = std::make_unique<Board>(generateRandomBoard(seed));
    builders.push_back(std::make_unique<Builder>(0, 'B', seed));
    builders.push_back(std::make_unique<Builder>(1, 'R', seed));
    builders.push_back(std::make_unique<Builder>(2, 'O', seed));
    builders.push_back(std::make_unique<Builder>(3, 'Y', seed));
}

Game::Game(unsigned seed, std::vector<TileInitData> data) : currentBuilder{0} {
    board = std::make_unique<Board>(data);
    builders.push_back(std::make_unique<Builder>(0, 'B', seed));
    builders.push_back(std::make_unique<Builder>(1, 'R', seed));
    builders.push_back(std::make_unique<Builder>(2, 'O', seed));
    builders.push_back(std::make_unique<Builder>(3, 'Y', seed));
}

Game::Game(unsigned seed, std::vector<TileInitData> data, std::vector<BuilderResourceData> resourceData, std::vector<BuilderStructureData> structureData, int currentBuilder, int GeeseTile) : currentBuilder{currentBuilder} {
    builders.push_back(std::make_unique<Builder>(0, 'B', seed, resourceData[0]));
    builders.push_back(std::make_unique<Builder>(1, 'R', seed, resourceData[1]));
    builders.push_back(std::make_unique<Builder>(2, 'O', seed, resourceData[2]));
    builders.push_back(std::make_unique<Builder>(3, 'Y', seed, resourceData[3]));

    std::vector<std::pair<Builder*, BuilderStructureData>> structures = {{builders.at(0).get(), structureData.at(0)}, {builders.at(1).get(), structureData.at(1)}, {builders.at(2).get(), structureData.at(2)}, {builders.at(3).get(), structureData.at(3)}};
    board = std::make_unique<Board>(data, structures);
    board->setGeeseTile(GeeseTile);
}

Game::~Game() {}

std::vector<TileInitData> Game::generateRandomBoard(unsigned seed) {
    std::default_random_engine rng{seed};
    std::vector<TileInitData> data;
    std::vector<int> tileValues = {2, 3, 3, 4, 4, 5, 5, 6, 6, 8, 8, 9, 9, 10, 10, 11, 11, 12};
    std::vector<Resource> resources;

    resources.insert(resources.end(), 3, Resource::WIFI);
    resources.insert(resources.end(), 3, Resource::HEAT);
    resources.insert(resources.end(), 4, Resource::BRICK);
    resources.insert(resources.end(), 4, Resource::ENERGY);
    resources.insert(resources.end(), 4, Resource::GLASS);

    // Shuffle resources
    shuffle(resources.begin(), resources.end(), rng);
    for (int i = 0; i < 18; i++) {
        data.push_back(TileInitData{tileValues[i], resources[i]});
    }
    data.push_back(TileInitData{7, Resource::PARK});
    shuffle(data.begin(), data.end(), rng);
    return data;
}
int Game::getCurrentBuilder() const {
    return currentBuilder;
}

const std::vector<const Builder*> Game::getBuilders() const {
    std::vector<const Builder*> builders;
    for (const std::unique_ptr<Builder>& b : this->builders)
        builders.push_back(b.get());
    return builders;
}

int Game::getGeeseLocation() const {
    return board->getGeeseTile();
}

const Board& Game::getBoard() const {
    return *board;
}

Builder& Game::getBuilder(std::string colour){
    if (colour == "Blue"){
        return *builders.at(0);
    }
    else if (colour == "Red"){
        return *builders.at(1);
    }
    else if (colour == "Orange"){
        return *builders.at(2);
    }
    else if (colour == "Yellow"){
        return *builders.at(3);
    }
}

void Game::buildInitialResidences(std::istream& in, std::ostream& out){
    int vertex;
    //build initial residences 
    for (int i = 0; i <  NUM_BUILDERS; i++) {
        out << "Builder " << builders[i]->getBuilderColourString() << ", where do you want to build a basement?";
        in >> vertex;     
        while(!board->buildInitialResidence(*builders[i], vertex, out)){
            out << "Builder " << builders[i]->getBuilderColourString() << ", where do you want to build a basement?";
            in >> vertex;   
        }
    }
    for (int i =  NUM_BUILDERS - 1; i >= 0; i--) {
        out << "Builder " << builders[i]->getBuilderColourString() << ", where do you want to build a basement?";
        in >> vertex;     
        while(!board->buildInitialResidence(*builders[i], vertex, out)){
            out << "Builder " << builders[i]->getBuilderColourString() << ", where do you want to build a basement?";
            in >> vertex;   
        }   
    } 
}

void Game::beginTurn(Builder& builder, std::istream& in , std::ostream& out) {
    std::string command;
    int roll;
    int loaded = 0;
    
    out << "Builder " << builder.getBuilderColourString() << "'s turn." << std::endl;
    out << builder.getStatus() << std::endl;

    in >> command;
    if (command == "load"){
        builder.setDice(true);
    }
    else if (command == "fair"){
        builder.setDice(false);
    }
    else if (command == "roll"){
        if (builder.getDice()){
            while (loaded < 2 || loaded > 12){
                out << "Input a roll between 2 and 12:" << std::endl;
                in >> loaded;
                if (loaded < 2 || loaded > 12){
                    out << "Invalid roll." << std::endl;
                }   
            }
        }
        roll = builder.rollDice(loaded);
        if (roll == 7){
            //discardHalfOfTotalResources(); //to do
            //steal  // to do
        }
        else{
            board->getResourcesFromDiceRoll(roll);
        }
        duringTurn(builder, in, out, roll);
    }
    else{
        out << "Invalid command." << std::endl;
    }
}

void Game::duringTurn(Builder& builder, std::istream& in , std::ostream& out, int roll){
    std::string command; 
    while(in >> command){
        if (command == "board"){
            board->printBoard(out);
        }
        else if (command == "status"){
            for (size_t i = 0; i < builders.size(); i++){
                out << builders[i]->getStatus() << std::endl;
            }     
        }
        else if (command == "residences"){
            out << "Builder " << builder.getBuilderColourString() << " has built:" << std::endl;
            for (size_t i = 0; i < builder.residences.size(); i++){
                out << builder.residences[i]->getLocation().getVertexNumber() << " " << builder.residences[i]->getResidenceLetter();
            }
        }
        else if (command.substr(0, 10) == "build-road"){
            board->buildRoad(builder, std::stoi(command.substr(11, 1)), out);    
        }
        else if (command.substr(0, 9) == "build-res"){
            board->buildResidence(builder, std::stoi(command.substr(10, 1)), out);
        }
        else if (command.substr(0, 7) == "improve"){
            board->upgradeResidence(builder, std::stoi(command.substr(8, 1)), out);
        }
        else if (command.substr(0, 5) == "trade"){
            

        }
        else if (command == "next"){
            return;
        }
        else if (command.substr(0, 4) == "save"){
            save(command.substr(5, command.length() - 5));
        }
        else if (command == "help"){
            out << "Valid commands:" << std::endl;
            out << "board" << std::endl;
            out << "status" << std::endl;
            out << "residences" << std::endl;
            out << "build-road <edge#>" << std::endl;
            out << "build-res <housing#>" << std::endl;
            out << "improve <housing#>" << std::endl;
            out << "trade <colour> <give> <take>" << std::endl;
            out << "next" << std::endl;
            out << "save <file>" << std::endl;
            out << "help" << std::endl;
        }
        else{
            out << "Invalid command." << std::endl;
        }
    }
}

void Game::nextTurn(){
    currentBuilder++;
    if (currentBuilder == 4){
        currentBuilder = 0;
    }
}


void Game::play(std::istream& in, std::ostream& out) {     
    buildInitialResidences(in, out);
    board->printBoard(out);  
    while (builders[0]->getBuildingPoints() < 10 || builders[1]->getBuildingPoints() < 10 || builders[2]->getBuildingPoints() < 10 || builders[3]->getBuildingPoints() < 10){
            beginTurn(*builders[currentBuilder], in, out);
            nextTurn();
    }
}


void Game::save(std::string filename) {
    std::ofstream outputFile{filename};
    outputFile << getCurrentBuilder() << std::endl;
    for (const Builder* b : getBuilders()) {
        // Resource inventory
        outputFile << b->inventory.at(Resource::BRICK) << " " << b->inventory.at(Resource::ENERGY) << " " << b->inventory.at(Resource::GLASS) << " " << b->inventory.at(Resource::HEAT) << " " << b->inventory.at(Resource::WIFI) << std::endl;
        // Roads
        outputFile << "r";
        for (const std::shared_ptr<Road>& r : b->roads) {
            outputFile << " " << r->getLocation().getEdgeNumber();
        }
        // Residences
        outputFile << " h";
        for (const std::shared_ptr<Residence>& h : b->residences) {
            outputFile << " " << h->getLocation().getVertexNumber() << " " << h->getResidenceLetter();
        }
        outputFile << std::endl;
    }
    for (int i = 0; i < Board::NUM_TILES; i++) {
        outputFile << getBoard().getTile(i)->getResource() << " " << getBoard().getTile(i)->getTileValue();
    }
    outputFile << std::endl;
    outputFile << getGeeseLocation() << std::endl;
    outputFile.close();
}


