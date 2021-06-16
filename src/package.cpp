#include "package.h"

void PreyPredatorPackageProvider::providePackage(
		PreyPredatorAgent* agent, std::vector<PreyPredatorPackage>& out){
    out.push_back({*agent});
}

void PreyPredatorPackageProvider::provideContent(
		repast::AgentRequest req,
		std::vector<PreyPredatorPackage>& out) {
    std::vector<repast::AgentId> ids = req.requestedAgents();
    for(size_t i = 0; i < ids.size(); i++){
        providePackage(agents->getAgent(ids[i]), out);
    }
}

PreyPredatorAgent* PreyPredatorPackageReceiver::createAgent(PreyPredatorPackage package) {
	if(package.id.agentType() == PREY)
		return new Prey(package.id, package.energy);
	return NULL;
}

void PreyPredatorPackageReceiver::updateAgent(PreyPredatorPackage package) {
	PreyPredatorAgent* agent = agents->getAgent(package.id);
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
