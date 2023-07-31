#ifndef BOARD_H
#define BOARD_H

#include "../common/forward.h"
#include "../common/resource.h"
#include <iostream>
#include <memory>
#include <vector>

// Stores information used to initialize Board Tiles
struct TileInitData {
    int tileValue;
    Resource resource;
};

class Board final {
  private:
    std::vector<std::unique_ptr<AbstractTile>> tiles;
    std::vector<std::unique_ptr<Vertex>> vertices;
    std::vector<std::unique_ptr<Edge>> edges;

    int geeseTile; // Tile number that contains geese

    void setupVertices();
    void setupEdges();
    void setupTiles();

    std::string printVertex(int) const;
    std::string printEdge(int, bool) const;
    std::string printTile(int) const;
    std::string printResource(int) const;
    std::string printGeese(int) const;

  public:
    Board(std::vector<TileInitData>);
    ~Board();

    static const int NUM_TILES = 19;
    static const int NUM_EDGES = 72;
    static const int NUM_VERTICES = 54;

    AbstractTile* getTile(int) const;
    Vertex* getVertex(int) const;
    Edge* getEdge(int) const;

    /**
     * TileInitData #0 is meant for Tile #0, TileInitData #1 is meant for Tile #1, etc.
     * There must be 19 elements in the TileInitData array, with exactly ONE park tile.
     *  (The park tile must have a tileValue of 7)
     */
    // void initBoard(std::vector<TileInitData>);

    bool buildRoad(Builder&, int, std::ostream&);
    bool buildResidence(Builder&, int, std::ostream&);
    bool buildInitialResidence(Builder&, int, std::ostream&);
    bool upgradeResidence(Builder&, int, std::ostream&);

    std::vector<Resource> getResourcesFromDiceRoll(const Builder&,
                                                   int) const; // What the given Builder would obtain if the given rollNumber is rolled

    int getGeeseTile() const;
    void setGeeseTile(int);

    void printBoard(std::ostream&) const;
    // void printResidences() const;
};

#endif
