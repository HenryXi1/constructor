#include "geesetile.h"
#include "../common/inventoryupdate.h"
#include "abstracttile.h"
#include "tile.h"

GeeseTile::GeeseTile(std::unique_ptr<AbstractTile> tile) : AbstractTile(), tile{std::move(tile)} {}

GeeseTile::~GeeseTile() {}

void GeeseTile::addNeighbouringVertex(Vertex* v) {
    tile->addNeighbouringVertex(v);
}

int GeeseTile::getTileNumber() const {
    return tile->getTileNumber();
}

int GeeseTile::getTileValue() const {
    return tile->getTileValue();
}

Resource GeeseTile::getResource() const {
    return tile->getResource();
}

std::vector<int> GeeseTile::getStealCandidates(Builder& b) const {
    return tile->getStealCandidates(b);
}

BuilderInventoryUpdate GeeseTile::giveResourcesToBuilders() const {
    // Geese tiles give no resources
    return BuilderInventoryUpdate();
}

std::unique_ptr<AbstractTile> GeeseTile::removeGeese() {
    return std::move(tile);
}

bool GeeseTile::hasGeese() const {
    return true;
}
