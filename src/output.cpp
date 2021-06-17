#include "output.h"

int PreyPredatorDataSource::getData() {
	std::vector<PreyPredatorAgent*> agents;
	if(context.size() > 0) {
		context.selectAgents(
				repast::SharedContext<PreyPredatorAgent>::LOCAL, agents
				);
	}
	int count = 0;
	for(auto agent : agents)
		if(agent->getId().agentType() == type)
			count++;
	return count;
}

int GrassDataSource::getData() {
	std::vector<Grass*> agents;
	context.selectAgents(
			repast::SharedContext<Grass>::LOCAL, agents
			);
	int count = 0;
	for(auto agent : agents)
		if(agent->isGrown())
			count++;
	return count;
}
