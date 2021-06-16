#include "model.h"
#include "repast_hpc/initialize_random.h"
#include "repast_hpc/Random.h"

PreyPredatorModel::PreyPredatorModel(
		std::string prop_file, int argc, char** argv,
		boost::mpi::communicator* comm) :
	context(comm), grass_context(comm), props(prop_file, argc, argv, comm),
	stop_at(repast::strToInt(props.getProperty("stop.at"))),
	prey_count(repast::strToInt(props.getProperty("prey.count"))),
	local_prey_count(prey_count/comm->size()),
	predator_count(repast::strToInt(props.getProperty("predator.count"))),
	local_predator_count(predator_count/comm->size()),
	grid_width(repast::strToInt(props.getProperty("grid.width"))),
	grid_height(repast::strToInt(props.getProperty("grid.height"))),
	provider(&context), receiver(&context) {
		grid = build_grid<Prey>("PreyGrid", comm);   
		context.addProjection(grid);

		grass_grid = build_grid<Grass>("GrassGrid", comm);
		grass_context.addProjection(grass_grid);

		initializeRandom(props, comm);
	}

void PreyPredatorModel::init_preys() {
	int rank = repast::RepastProcess::instance()->rank();

	repast::IntUniformGenerator gen_x = 
		repast::Random::instance()->createUniIntGenerator(
				grid->dimensions().origin().getX(),
				grid->dimensions().origin().getX() + grid->dimensions().extents().getX()-1
				);
	repast::IntUniformGenerator gen_y = 
		repast::Random::instance()->createUniIntGenerator(
				grid->dimensions().origin().getY(),
				grid->dimensions().origin().getY() + grid->dimensions().extents().getY()-1
				);

	for(int i = 0; i < local_prey_count; i++) {
		repast::AgentId id(Prey::current_prey_id++, rank, Prey::type);
		id.currentRank(rank);

		repast::Point<int> initial_location(gen_x.next(), gen_y.next());

		Prey* prey = new Prey(id);
		context.addAgent(prey);
		grid->moveTo(prey, initial_location);
		std::cout << id << " init location: " << initial_location << std::endl;
	}
}

void PreyPredatorModel::init_grass() {
	int rank = repast::RepastProcess::instance()->rank();
	float init_grass_rate = repast::strToDouble(props.getProperty("grass.init_rate"));
	Grass::growing_rate = repast::strToInt(props.getProperty("grass.growing_rate"));
	auto rd_countdown = repast::Random::instance()->createUniIntGenerator(1, Grass::growing_rate);

	int id = 0;
	repast::GridDimensions dimensions = grass_grid->dimensions();
	for(int x = dimensions.origin().getX();
			x < dimensions.origin().getX() + dimensions.extents().getX();
			x++) {
		for(int y = dimensions.origin().getY();
				y < dimensions.origin().getY() + dimensions.extents().getY();
				y++) {
			bool grown = repast::Random::instance()->nextDouble() < init_grass_rate;
			int countdown = 0;
			if(!grown)
				countdown = rd_countdown.next();
			repast::AgentId agent_id(id++, rank, Grass::type, rank);
			Grass* grass = new Grass(agent_id, grown, countdown);
			grass_context.addAgent(grass);

			repast::Point<int> location(x, y);
			grass_grid->moveTo(grass, location);
		}
	}

}

void PreyPredatorModel::init() {
	init_grass();
	init_preys();
}

void PreyPredatorModel::initSchedule(repast::ScheduleRunner& runner) {
	runner.scheduleEvent(1.0, 1, repast::Schedule::FunctorPtr(
				new repast::MethodFunctor<PreyPredatorModel> (
					this, &PreyPredatorModel::move
					)
				));
	runner.scheduleEvent(1.1, 1, repast::Schedule::FunctorPtr(
				new repast::MethodFunctor<PreyPredatorModel> (
					this, &PreyPredatorModel::eat
					)
				));
	runner.scheduleEvent(1.2, 1, repast::Schedule::FunctorPtr(
				new repast::MethodFunctor<PreyPredatorModel> (
					this, &PreyPredatorModel::reproduce
					)
				));
	runner.scheduleEvent(1.3, 1, repast::Schedule::FunctorPtr(
				new repast::MethodFunctor<PreyPredatorModel> (
					this, &PreyPredatorModel::die
					)
				));


	runner.scheduleStop(stop_at);
}

void PreyPredatorModel::move() {
	std::cout << "MOVING AGENTS" << std::endl;
	std::vector<Prey*> preys;
	
	context.selectAgents(repast::SharedContext<Prey>::LOCAL, preys);

	for(auto agent : preys)
		agent->move(*grid);

	std::set<Prey*> local_preys;
	context.selectAgents(repast::SharedContext<Prey>::LOCAL, local_preys);
	for(auto agent : local_preys)
		std::cout << "LOCAL AGENT: " << agent->getId() << " - " << agent->getEnergy() << std::endl;

	local_preys.clear();
	context.selectAgents(repast::SharedContext<Prey>::NON_LOCAL, local_preys);
	for(auto agent : local_preys)
		std::cout << "DISTANT AGENT: " << agent->getId() << std::endl;

	this->synchronize();
}

void PreyPredatorModel::eat() {
	std::cout << "AGENTS EAT" << std::endl;
	std::vector<Prey*> preys;
	context.selectAgents(repast::SharedContext<Prey>::LOCAL, preys);

	for(auto agent : preys)
		agent->eat(*grid, *grass_grid);

	this->synchronize();
}

void PreyPredatorModel::reproduce() {
	std::cout << "AGENTS REPRODUCE" << std::endl;

	std::vector<Prey*> preys;
	context.selectAgents(repast::SharedContext<Prey>::LOCAL, preys);

	for(auto agent : preys)
		agent->reproduce(context, *grid);

	this->synchronize();
}

void PreyPredatorModel::die() {
	std::cout << "AGENTS DIE" << std::endl;
	std::vector<Prey*> preys;
	context.selectAgents(repast::SharedContext<Prey>::LOCAL, preys);

	for(auto agent : preys)
		agent->die(context, *grid);

	this->synchronize();
}

void PreyPredatorModel::synchronize() {
	std::cout << "SYNC GRID" << std::endl;
	// Marks agents to migrate
	grid->balance();

	/*
	 * Sync preys
	 */
	// Move agents across processes
	repast::RepastProcess::instance()
		->synchronizeAgentStatus<Prey, PreyPackage, PreyPackageProvider,
		PreyPackageReceiver>(context, provider, receiver, receiver);

	// Synchronizes projection: builds agent copies in buffer zones
	repast::RepastProcess::instance()
		->synchronizeProjectionInfo<Prey, PreyPackage, PreyPackageProvider,
		PreyPackageReceiver>(context, provider, receiver, receiver);

	// Updates agent data
	repast::RepastProcess::instance()
		->synchronizeAgentStates<PreyPackage, PreyPackageProvider,
		PreyPackageReceiver>(provider, receiver);
}
