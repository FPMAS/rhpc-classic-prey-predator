#ifndef PP_AGENT_H
#define PP_AGENT_H

#include "repast_hpc/AgentId.h"
#include "repast_hpc/SharedContext.h"
#include "config.h"
#include "grass.h"

class PreyPredatorAgent {
	private:
		int move_cost;
		float reproduction_rate;

		repast::AgentId id;

	protected:
		int energy;
		bool alive;

		PreyPredatorAgent(
				int move_cost, float reproduction_rate,
				repast::AgentId id, int energy, bool alive) :
			move_cost(move_cost), reproduction_rate(reproduction_rate),
			id(id), energy(energy), alive(alive) {
			}

		virtual PreyPredatorAgent* build(int energy) = 0;

	public:
		repast::AgentId& getId() {return id;}
		const repast::AgentId& getId() const {return id;}

		int getEnergy() const {return energy;}

		void kill() {this->alive = false;}
		bool isAlive() const {return alive;}

		void update(int current_rank, int energy);

		void move(Grid<PreyPredatorAgent>& grid);
		void reproduce(
				repast::SharedContext<PreyPredatorAgent>& context,
				Grid<PreyPredatorAgent>& grid
				);
		void die(
				repast::SharedContext<PreyPredatorAgent>& context,
				Grid<PreyPredatorAgent>& grid
				);

		virtual void eat(
				Grid<PreyPredatorAgent>& agent_grid,
				Grid<Grass>& grass_grid
				) = 0;
};

class Prey : public PreyPredatorAgent {
	private:
		Prey* build(int energy) override;

	public:
		static int init_energy;
		static int energy_gain;
		static int move_cost;
		static float reproduction_rate;

		static int current_prey_id;

		Prey(repast::AgentId id) : Prey(id, Prey::init_energy, true) {
		}
		Prey(repast::AgentId id, int energy, bool alive) : PreyPredatorAgent(
					Prey::move_cost, Prey::reproduction_rate, id, energy, alive
					) {
		}

		void eat(
				Grid<PreyPredatorAgent>& agent_grid,
				Grid<Grass>& grass_grid
				) override;
};

class Predator : public PreyPredatorAgent {
	private:
		Predator* build(int energy) override;

	public:
		static int init_energy;
		static int energy_gain;
		static int move_cost;
		static float reproduction_rate;

		static int current_predator_id;

		Predator(repast::AgentId id) : Predator(id, Predator::init_energy, true) {
		}
		Predator(repast::AgentId id, int energy, bool alive) : PreyPredatorAgent(
					Predator::move_cost, Predator::reproduction_rate, id, energy, alive
					) {
		}

		void eat(
				Grid<PreyPredatorAgent>& agent_grid,
				Grid<Grass>& grass_grid
				) override;

};
#endif
