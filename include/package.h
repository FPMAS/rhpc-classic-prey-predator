#include "prey.h"

struct PreyPredatorPackage {
	repast::AgentId id;
	int energy;

	PreyPredatorPackage() {};
	
	PreyPredatorPackage(const PreyPredatorAgent& agent) :
		id(agent.getId()),
		energy(agent.getEnergy()) {
		}

	template<typename Archive>
		void serialize(Archive& ar, const unsigned int) {
			ar & id;
			ar & energy;
		}
};

class PreyPredatorPackageProvider {
	private:
		repast::SharedContext<PreyPredatorAgent>* agents;
	public:
		PreyPredatorPackageProvider(repast::SharedContext<PreyPredatorAgent>* agents)
			: agents(agents) {}

		void providePackage(
				PreyPredatorAgent* agent, std::vector<PreyPredatorPackage>& out);
		void provideContent(
				repast::AgentRequest req, std::vector<PreyPredatorPackage>& out);
};

class PreyPredatorPackageReceiver {
	private:
		repast::SharedContext<PreyPredatorAgent>* agents;
	public:
		PreyPredatorPackageReceiver(repast::SharedContext<PreyPredatorAgent>* agents)
			: agents(agents) {}

		PreyPredatorAgent* createAgent(PreyPredatorPackage package);
		void updateAgent(PreyPredatorPackage package);
};
