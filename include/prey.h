#include "repast_hpc/AgentId.h"
#include "repast_hpc/SharedContext.h"
#include "config.h"
#include "grass.h"

enum AgentTypes {
	PREY = 0,
	PREDATOR = 1
};

class PreyPredatorAgent {
	private:
		int move_cost;
		float reproduction_rate;

		repast::AgentId id;

		void initChildLocation(PreyPredatorAgent* child, Grid<PreyPredatorAgent>& grid);
	protected:
		int energy;

		PreyPredatorAgent(
				int move_cost, float reproduction_rate,
				repast::AgentId id, int energy) :
			move_cost(move_cost), reproduction_rate(reproduction_rate),
			id(id), energy(energy) {
			}

		virtual PreyPredatorAgent* build(int energy) = 0;

	public:
		repast::AgentId& getId() {return id;}
		const repast::AgentId& getId() const {return id;}

		int getEnergy() const {return energy;}

		void update(int current_rank, int energy);

		void move(Grid<PreyPredatorAgent>& predator_grid, Grid<PreyPredatorAgent>& prey_grid);
		void reproduce(
				repast::SharedContext<PreyPredatorAgent>& context,
				Grid<PreyPredatorAgent>& predator_grid, Grid<PreyPredatorAgent>& prey_grid
				);
		void die(
				repast::SharedContext<PreyPredatorAgent>& context,
				Grid<PreyPredatorAgent>& predator_grid, Grid<PreyPredatorAgent>& prey_grid);

		virtual void eat(
				Grid<PreyPredatorAgent>& predator_grid, Grid<PreyPredatorAgent>& prey_grid,
				Grid<Grass>& grass_grid
				) = 0;
};

class Prey : public PreyPredatorAgent {
	private:
		static int init_energy;
		static int energy_gain;
		static int move_cost;
		static float reproduction_rate;

		Prey* build(int energy) override;

	public:
		static int current_prey_id;

		Prey(repast::AgentId id) : Prey(id, Prey::init_energy) {
		}
		Prey(repast::AgentId id, int energy) : PreyPredatorAgent(
					Prey::move_cost, Prey::reproduction_rate, id, energy
					) {
		}

		void eat(
				Grid<PreyPredatorAgent>& predator_grid, Grid<PreyPredatorAgent>& prey_grid,
				Grid<Grass>& grass_grid
				) override;
};
