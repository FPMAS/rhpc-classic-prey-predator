#include "package.h"
#include "types.h"

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
		return new Prey(package.id, package.energy, package.alive);
	return new Predator(package.id, package.energy, package.alive);
}

void PreyPredatorPackageReceiver::updateAgent(PreyPredatorPackage package) {
	PreyPredatorAgent* agent = agents->getAgent(package.id);
    agent->update(package.id.currentRank(), package.energy);
}
