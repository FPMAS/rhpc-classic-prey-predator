#include "prey.h"

int Prey::current_prey_id = 0;

int Prey::init_energy = 4;
int Prey::energy_gain = 4;
int Prey::move_cost = 1;
float Prey::reproduction_rate = 0.05;

void PreyPredatorAgent::initChildLocation(
		PreyPredatorAgent* child,
		Grid<PreyPredatorAgent>& grid
		) {
		std::vector<int> location;
		grid.getLocation(this, location);

		std::cout << "Agent " << this->getId()
			<< " reproduces at (" << location[0] << "," << location[1] << "). "
			<< "Child: " << child->getId() << std::endl;

		grid.moveTo(child, location);
}

void PreyPredatorAgent::update(int current_rank, int energy) {
	id.currentRank(current_rank);
	energy = energy;
}

void PreyPredatorAgent::move(Grid<PreyPredatorAgent>& predator_grid, Grid<PreyPredatorAgent>& prey_grid) {
	repast::IntUniformGenerator gen
		= repast::Random::instance()->createUniIntGenerator(-1, 1);
	std::vector<int> new_location;
	Grid<PreyPredatorAgent>& current_grid = this->getId().agentType() == PREY
		? prey_grid : predator_grid;

	current_grid.getLocation(this, new_location);

	new_location[0] += gen.next();
	new_location[0] = std::max(0, new_location[0]);
	new_location[0] = std::min(
			new_location[0],
			(int) (current_grid.bounds().origin().getX() + current_grid.bounds().extents().getX()-1)
			);

	new_location[1] += gen.next();
	new_location[1] = std::max(0, new_location[1]);
	new_location[1] = std::min(
			new_location[1],
			(int) (current_grid.bounds().origin().getY() + current_grid.bounds().extents().getY()-1)
			);

	current_grid.moveTo(this, new_location);
	this->energy -= move_cost;

	std::cout << id << " moves to [" << new_location[0] << ", " << new_location[1] << "]" << std::endl;
}

void PreyPredatorAgent::reproduce(
		repast::SharedContext<PreyPredatorAgent>& context,
		Grid<PreyPredatorAgent>& predator_grid,
		Grid<PreyPredatorAgent>& prey_grid) {
	repast::DoubleUniformGenerator gen
		= repast::Random::instance()->createUniDoubleGenerator(0, 1);

	if(gen.next() < reproduction_rate) {
		PreyPredatorAgent* new_agent = this->build(this->energy / 2);
		this->energy /= 2;

		context.addAgent(new_agent);
		if(this->getId().agentType() == PREY)
			initChildLocation(new_agent, prey_grid);
		else
			initChildLocation(new_agent, predator_grid);
	}
}

void PreyPredatorAgent::die(
		repast::SharedContext<PreyPredatorAgent>& context,
		Grid<PreyPredatorAgent>& predator_grid, Grid<PreyPredatorAgent>& prey_grid) {
	if(this->energy <= 0) {
		std::cout << this->getId() << " dies" << std::endl;
		repast::RepastProcess::instance()->agentRemoved(this->getId());
		if(this->getId().agentType() == PREY)
			prey_grid.removeAgent(this);
		else
			predator_grid.removeAgent(this);
		context.removeAgent(this);
	}
}

Prey* Prey::build(int energy) {
	int rank = repast::RepastProcess::instance()->rank();
	repast::AgentId new_prey_id(Prey::current_prey_id++, rank, PREY);
	new_prey_id.currentRank(rank);
	return new Prey(new_prey_id, energy);
}

void Prey::eat(Grid<PreyPredatorAgent>&, Grid<PreyPredatorAgent>& prey_grid, Grid<Grass>& grass_grid) {
	std::vector<int> location;
	prey_grid.getLocation(this, location);
	Grass* grass = grass_grid.getObjectAt(location);

	if(grass->grown()) {
		std::cout << this->getId()
			<< " eats (" << location[0] << "," << location[1] << ")"
			<< std::endl;
		grass->reset();
		this->energy+=energy_gain;
	}
}


