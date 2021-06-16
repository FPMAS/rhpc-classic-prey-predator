#include "package.h"

void PreyPackageProvider::providePackage(Prey* agent, std::vector<PreyPackage>& out){
    out.push_back({*agent});
}

void PreyPackageProvider::provideContent(
		repast::AgentRequest req,
		std::vector<PreyPackage>& out) {
    std::vector<repast::AgentId> ids = req.requestedAgents();
    for(size_t i = 0; i < ids.size(); i++){
        providePackage(agents->getAgent(ids[i]), out);
    }
}

Prey* PreyPackageReceiver::createAgent(PreyPackage package) {
	return new Prey(package.id, package.energy);
}

void PreyPackageReceiver::updateAgent(PreyPackage package) {
	Prey* agent = agents->getAgent(package.id);
    agent->update(package.id.currentRank(), package.energy);
}

/*
 *void GrassPackageProvider::providePackage(Grass* agent, std::vector<GrassPackage>& out){
 *    out.push_back({*agent});
 *}
 *
 *void GrassPackageProvider::provideContent(
 *        repast::AgentRequest req,
 *        std::vector<GrassPackage>& out) {
 *    std::vector<repast::AgentId> ids = req.requestedAgents();
 *    for(size_t i = 0; i < ids.size(); i++){
 *        providePackage(agents->getAgent(ids[i]), out);
 *    }
 *}
 *
 *Grass* GrassPackageReceiver::createAgent(GrassPackage package) {
 *    return new Grass(package.id, package.grown, package.countdown);
 *}
 *
 *void GrassPackageReceiver::updateAgent(GrassPackage package) {
 *    Grass* agent = agents->getAgent(package.id);
 *    agent->update(package.id.currentRank(), package.grown, package.countdown);
 *}
 */
