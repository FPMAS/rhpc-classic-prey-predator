#include "prey.h"

int Prey::current_prey_id = 0;

const int Prey::type = 0;
int Prey::init_energy = 4;
int Prey::energy_gain = 4;
int Prey::move_cost = 1;
float Prey::reproduction_rate = 0.05;

void Prey::move(Grid<Prey>& grid) {
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
	this->energy -= Prey::move_cost;

	std::cout << id << " moves to [" << new_location[0] << ", " << new_location[1] << "]" << std::endl;
}

void Prey::eat(Grid<Prey>& prey_grid, Grid<Grass>& grass_grid) {
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

void Prey::reproduce(repast::SharedContext<Prey>& context, Grid<Prey>& grid) {
	repast::DoubleUniformGenerator gen
		= repast::Random::instance()->createUniDoubleGenerator(0, 1);
	int rank = repast::RepastProcess::instance()->rank();

	if(gen.next() < Prey::reproduction_rate) {
		repast::AgentId new_prey_id(Prey::current_prey_id++, rank, Prey::type);
		new_prey_id.currentRank(rank);
		Prey* new_prey = new Prey(new_prey_id, this->energy / 2);
		this->energy /= 2;

		std::vector<int> location;
		grid.getLocation(this, location);

		std::cout << this->getId() << " reproduces at (" << location[0] << "," << location[1] << "). Child: " << new_prey_id << std::endl;

		context.addAgent(new_prey);
		grid.moveTo(new_prey, location);
	}
}

void Prey::die(repast::SharedContext<Prey>& context, Grid<Prey>& grid) {
	if(this->energy <= 0) {
		std::cout << this->getId() << " dies" << std::endl;
		repast::RepastProcess::instance()->agentRemoved(this->getId());
		grid.removeAgent(this);
		context.removeAgent(this);
	}
}

void Prey::update(int current_rank, int energy) {
	id.currentRank(current_rank);
	energy = energy;
}
