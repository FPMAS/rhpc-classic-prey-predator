#include "agent.h"
#include "types.h"

int Prey::current_prey_id = 0;

int Prey::init_energy = 4;
int Prey::energy_gain = 4;
int Prey::move_cost = 1;
float Prey::reproduction_rate = 0.05;

int Predator::current_predator_id = 0;

int Predator::init_energy = 20;
int Predator::energy_gain = 20;
int Predator::move_cost = 1;
float Predator::reproduction_rate = 0.04;

void PreyPredatorAgent::update(int current_rank, int energy) {
	id.currentRank(current_rank);
	energy = energy;
}

void PreyPredatorAgent::move(Grid<PreyPredatorAgent>& grid) {
	repast::IntUniformGenerator gen
		= repast::Random::instance()->createUniIntGenerator(-1, 1);
	std::vector<int> new_location;

	grid.getLocation(this, new_location);

	new_location[0] += gen.next();
	new_location[0] = std::max(0, new_location[0]);
	new_location[0] = std::min(
			new_location[0],
			(int) (grid.bounds().origin().getX() + grid.bounds().extents().getX()-1)
			);

	new_location[1] += gen.next();
	new_location[1] = std::max(0, new_location[1]);
	new_location[1] = std::min(
			new_location[1],
			(int) (grid.bounds().origin().getY() + grid.bounds().extents().getY()-1)
			);

	grid.moveTo(this, new_location);
	this->energy -= move_cost;

	std::cout << id << " moves to [" << new_location[0] << ", " << new_location[1] << "]" << std::endl;
}

void PreyPredatorAgent::reproduce(
		repast::SharedContext<PreyPredatorAgent>& context,
		Grid<PreyPredatorAgent>& grid) {
	repast::DoubleUniformGenerator gen
		= repast::Random::instance()->createUniDoubleGenerator(0, 1);

	if(gen.next() < reproduction_rate) {
		PreyPredatorAgent* child = this->build(this->energy / 2);
		this->energy /= 2;

		context.addAgent(child);

		std::vector<int> location;
		grid.getLocation(this, location);

		std::cout << "Agent " << this->getId()
			<< " reproduces at (" << location[0] << "," << location[1] << "). "
			<< "Child: " << child->getId() << std::endl;

		grid.moveTo(child, location);
	}
}

void PreyPredatorAgent::die(
		repast::SharedContext<PreyPredatorAgent>& context,
		Grid<PreyPredatorAgent>& grid) {
	if(this->energy <= 0)
		this->kill();

	if(!this->isAlive()) {
		std::cout << this->getId() << " dies" << std::endl;
		repast::RepastProcess::instance()->agentRemoved(this->getId());
		grid.removeAgent(this);
		context.removeAgent(this);
	}
}

Prey* Prey::build(int energy) {
	int rank = repast::RepastProcess::instance()->rank();
	repast::AgentId new_prey_id(Prey::current_prey_id++, rank, PREY);
	new_prey_id.currentRank(rank);
	return new Prey(new_prey_id, energy, true);
}

void Prey::eat(
		Grid<PreyPredatorAgent>& agent_grid,
		Grid<Grass>& grass_grid) {
	std::vector<int> location;
	agent_grid.getLocation(this, location);
	Grass* grass = grass_grid.getObjectAt(location);

	if(grass->grown()) {
		std::cout << this->getId()
			<< " eats (" << location[0] << "," << location[1] << ")"
			<< std::endl;
		grass->reset();
		this->energy+=Prey::energy_gain;
	}
}

Predator* Predator::build(int energy) {
	int rank = repast::RepastProcess::instance()->rank();
	repast::AgentId new_predator_id(Predator::current_predator_id++, rank, PREDATOR);
	new_predator_id.currentRank(rank);
	return new Predator(new_predator_id, energy, true);
}

void Predator::eat(
		Grid<PreyPredatorAgent>& agent_grid,
		Grid<Grass>& grass_grid) {
	std::vector<int> location;
	agent_grid.getLocation(this, location);
	std::vector<PreyPredatorAgent*> agents;
	agent_grid.getObjectsAt(location, agents);

	auto it = agents.begin();
	while(it != agents.end()) {
		if(Prey* prey = dynamic_cast<Prey*>(*it)) {
			if(prey->isAlive()) {
				std::cout << this->getId()
					<< " eats " << prey->getId()
					<< std::endl;
				prey->kill();
				this->energy+=Predator::energy_gain;
				break;
			}
		}
		it++;
	}
}
