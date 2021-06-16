#include "prey.h"

/*
 * Prey
 */

class PreyPackageProvider {
	private:
		repast::SharedContext<Prey>* agents;
	public:
		PreyPackageProvider(repast::SharedContext<Prey>* agents) : agents(agents) {}

		void providePackage(Prey * agent, std::vector<PreyPackage>& out);
		void provideContent(repast::AgentRequest req, std::vector<PreyPackage>& out);
};

class PreyPackageReceiver {
	private:
		repast::SharedContext<Prey>* agents;
	public:
		PreyPackageReceiver(repast::SharedContext<Prey>* agents) : agents(agents) {}

		Prey* createAgent(PreyPackage package);
		void updateAgent(PreyPackage package);
};

/*
 * Grass
 */

/*
 *class GrassPackageProvider {
 *    private:
 *        repast::SharedContext<Grass>* agents;
 *    public:
 *        GrassPackageProvider(repast::SharedContext<Grass>* agents) : agents(agents) {}
 *
 *        void providePackage(Grass* agent, std::vector<GrassPackage>& out);
 *        void provideContent(repast::AgentRequest req, std::vector<GrassPackage>& out);
 *};
 *
 *class GrassPackageReceiver {
 *    private:
 *        repast::SharedContext<Grass>* agents;
 *    public:
 *        GrassPackageReceiver(repast::SharedContext<Grass>* agents) : agents(agents) {}
 *
 *        Grass* createAgent(GrassPackage package);
 *        void updateAgent(GrassPackage package);
 *};
 */
