#include "repast_hpc/AgentId.h"
#include "repast_hpc/SharedContext.h"
#include "config.h"
#include "grass.h"

class Prey {
	private:
		repast::AgentId id;
		int energy;
		static int init_energy;
		static int energy_gain;
		static int move_cost;
		static float reproduction_rate;

	public:
		static int current_prey_id;
		static const int type;

		Prey(repast::AgentId id) : Prey(id, Prey::init_energy) {
		}
		Prey(repast::AgentId id, int energy) : id(id), energy(energy) {
		}

		repast::AgentId& getId() {return id;}
		const repast::AgentId& getId() const {return id;}

		int getEnergy() const {return energy;}

		void update(int current_rank, int energy);

		void move(Grid<Prey>& grid);
		void eat(Grid<Prey>& prey_grid, Grid<Grass>& grass_grid);
		void reproduce(repast::SharedContext<Prey>& context, Grid<Prey>& grid);
		void die(repast::SharedContext<Prey>& context, Grid<Prey>& grid);
};

struct PreyPackage {
	repast::AgentId id;
	int energy;

	PreyPackage() {};
	
	PreyPackage(const Prey& prey) :
		id(prey.getId()),
		energy(prey.getEnergy()) {
		}

	template<typename Archive>
		void serialize(Archive& ar, const unsigned int) {
			ar & id;
			ar & energy;
		}
};

